
#include "Blank.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static math::Vec get_pos_slump(math::Vec pos1, math::Vec pos2) {
	float		dist;
	math::Vec	avg;

	dist = pos1.minus(pos2).norm();
	avg = pos1.plus(pos2).div(2);
	// Lower average point as distance increases
	avg.y += (1.0 - settings::cableTension) * (150.0 + 1.0 * dist);
	return avg;
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

BlankCables::BlankCables() {
}

void BlankCables::draw(const DrawArgs &args) {
}

void BlankCables::drawLayer(const DrawArgs &args, int layer) {
	BlankCable		*cable;
	math::Vec		pos_in;
	math::Vec		pos_out;
	math::Vec		pos_slump;
	math::Vec		pos_point, pos_point_prev;
	math::Vec		pos_angle;
	NVGcolor		color, color_light;
	NVGcolor		col_in, col_out;
	Rect			rect;
	Rect			rect_module;
	bool			brightness;
	bool			fast;
	bool			light;
	float			t;
	float			angle;
	float			amp;
	float			length;
	float			scale;
	float			slew;
	float			radius, radius_out;
	float			voltage, voltage_prev;
	float			voltage_diff, voltage_diff_max, voltage_max;
	float			orientation;
	int				buffer_phase, buffer_phase_prev;
	int				i, j;

	if (layer != 1)
		return;

	if (this->module == NULL)
		return;
	if (g_blank != this->module)
		return;
	brightness = this->module->params[Blank::PARAM_CABLE_BRIGHTNESS].getValue();
	light = this->module->params[Blank::PARAM_CABLE_LIGHT].getValue();
	scale = this->module->params[Blank::PARAM_CABLE_SCALE].getValue();
	fast = this->module->params[Blank::PARAM_CABLE_FAST].getValue();
	slew = this->module->params[Blank::PARAM_CABLE_SLEW].getValue();
	slew = (slew * slew) * 0.8;

	rect = box.zeroPos();
	rect_module = this->parent->box;

	/// [1] DRAW CABLES
	nvgLineCap(args.vg, 1);
	nvgLineJoin(args.vg, 1);
	for (i = 0; i < this->module->cable_count; ++i) {

		cable = &(this->module->cables[i]);

		/// COMPUTE CABLE POSITION
		pos_in = cable->pos_in;
		pos_out = cable->pos_out;
		pos_slump = get_pos_slump(pos_out, pos_in);
		//pos_out =
		///**/ pos_out.plus(pos_slump.minus(pos_out).normalize().mult(10.5));
		//pos_in =
		///**/ pos_in.plus(pos_slump.minus(pos_in).normalize().mult(10.5));
		//pos_out =
		///**/ pos_out.plus(pos_slump.minus(pos_out).normalize().mult(13.0));
		//pos_in =
		///**/ pos_in.plus(pos_slump.minus(pos_in).normalize().mult(13.0));
		orientation = (pos_in.x > pos_out.x) ? 1.0 : -1.0;

		/// COMPUTE CABLE LENGTH
		length = sqrt(
		/**/ (pos_in.x - pos_out.x) * (pos_in.x - pos_out.x)
		/**/ + (pos_in.y - pos_out.y) * (pos_in.y - pos_out.y));
		if (length > BLANK_DIST_MAX)
			length = BLANK_DIST_MAX;
		if (length < 1.0)
			length = 1.0;
		length = length / BLANK_DIST_MAX;

		/// SET CABLE COLOR
		if (brightness) {
			color_light = color::mult(cable->color, settings::rackBrightness);
			nvgStrokeColor(args.vg, color_light);
			nvgFillColor(args.vg, color_light);
		} else {
			nvgStrokeColor(args.vg, cable->color);
			nvgFillColor(args.vg, cable->color);
		}

		/// DRAW CABLE PLUGS
		nvgGlobalAlpha(args.vg, 1.0);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, cable->pos_in.x, cable->pos_in.y, 8.5);
		nvgStrokeWidth(args.vg, 4.0);
		nvgStroke(args.vg);
		if (light == false)
			nvgFill(args.vg);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, cable->pos_out.x, cable->pos_out.y, 8.5);
		nvgStrokeWidth(args.vg, 4.0);
		nvgStroke(args.vg);
		if (light == false)
			nvgFill(args.vg);

		/// DRAW CABLE
		nvgGlobalAlpha(args.vg, settings::cableOpacity);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, VEC_ARGS(pos_in));
		buffer_phase_prev = this->module->buffer_i;
		voltage_prev = 0.0;
		for (j = 0; j < BLANK_PRECISION; ++j) {
			t = (float)(j + 1) / (float)BLANK_PRECISION;

			/// COMPUTE POINT TANGENT
			pos_angle.x = 2.0 * (1.0 - t) * (pos_slump.x - pos_in.x)
			/**/ + 2.0 * t * (pos_out.x - pos_slump.x);
			pos_angle.y = 2.0 * (1.0 - t) * (pos_slump.y - pos_in.y)
			/**/ + 2.0 * t * (pos_out.y - pos_slump.y);
			angle = atan2(pos_angle.y, pos_angle.x);

			/// COMPUTE POINT POSITION
			pos_point.x =
			/**/ (1.0 - t) * (1.0 - t) * pos_in.x
			/**/ + 2 * (1.0 - t) * t * pos_slump.x
			/**/ + t * t * pos_out.x;
			pos_point.y =
			/**/ (1.0 - t) * (1.0 - t) * pos_in.y
			/**/ + 2 * (1.0 - t) * t * pos_slump.y
			/**/ + t * t * pos_out.y;

			/// COMPUTE POINT PHASE
			buffer_phase = this->module->buffer_i
			/**/ - t * ((float)BLANK_BUFFER * length);
			if (buffer_phase < 0)
				buffer_phase += BLANK_BUFFER;

			/// COMPUTE POINT ANIMATED POSITION
			//// MODE FAST
			if (fast) {
				voltage = cable->buffer[buffer_phase];
			//// MODE PRECISION
			} else {
				voltage_diff_max = 0;
				voltage_max = voltage_prev;
				while (buffer_phase_prev != buffer_phase) {
					voltage = cable->buffer[buffer_phase_prev];
					voltage_diff = voltage_prev - voltage;
					if (voltage_diff < 0)
						voltage_diff = -voltage_diff;
					if (voltage_diff > voltage_diff_max) {
						voltage_diff_max = voltage_diff;
						voltage_max = voltage;
					}
					buffer_phase_prev -= 1;
					if (buffer_phase_prev < 0)
						buffer_phase_prev += BLANK_BUFFER;
				}
				voltage = voltage_max;
			}
			voltage = voltage * (1.0 - slew) + voltage_prev * slew;
			voltage_prev = voltage;
			angle += M_PI * 0.5;
			if (t < 0.2)
				amp = t * 5.0;
			else if (t > 0.8)
				amp = (1.0 - t) * 5.0;
			else
				amp = 1.0;
			amp *= scale * orientation;
			pos_point.x += cos(angle) * voltage * amp;
			pos_point.y += sin(angle) * voltage * amp;

			nvgLineTo(args.vg, VEC_ARGS(pos_point));
		}
		nvgStrokeWidth(args.vg, (cable->thick) ? 9.0 : 6.0);
		nvgStroke(args.vg);

		if (light) {

			/// DRAW CABLE LIGHT
			buffer_phase = this->module->buffer_i - 1;
			if (buffer_phase < 0)
				buffer_phase += BLANK_BUFFER;
			voltage = cable->buffer[buffer_phase];
			if (voltage > 0)
				color = color::mult({0.6274, 0.8235, 0.2862}, voltage * 0.1);
			else
				color = color::mult({0.9450, 0.2078, 0.1725}, voltage * -0.1);
			color.a = 1.0;
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, cable->pos_in.x, cable->pos_in.y, 5.75);
			nvgCircle(args.vg, cable->pos_out.x, cable->pos_out.y, 5.75);
			nvgFillColor(args.vg, color);
			nvgFill(args.vg);
			if (brightness) {
				nvgStrokeColor(args.vg,
				/**/ color::mult({0.9019, 0.8823, 0.8823, 1.0},
				/**/ settings::rackBrightness));
			} else {
				nvgStrokeColor(args.vg, {0.9019, 0.8823, 0.8823, 1.0});
			}
			nvgStrokeWidth(args.vg, 1.5);
			nvgStroke(args.vg);

			/// DRAW CABLE LIGHT HALO
			if (settings::haloBrightness > 0) {

				nvgGlobalCompositeBlendFunc(args.vg,
				/**/ NVG_ONE_MINUS_DST_COLOR, NVG_ONE);
				radius = 5.0;
				radius_out = radius + std::min(radius * 4.f, 15.f);
				col_in = color::mult(color, settings::haloBrightness);
				col_out = nvgRGBA(0, 0, 0, 0);

				/// INPUT PORT HALO
				pos_in = cable->pos_in;
				nvgBeginPath(args.vg);
				nvgRect(args.vg,
				/**/ pos_in.x - radius_out, pos_in.y - radius_out,
				/**/ 2 * radius_out, 2 * radius_out);
				NVGpaint paint = nvgRadialGradient(args.vg,
				/**/ pos_in.x, pos_in.y,
				/**/ radius, radius_out,
				/**/ col_in, col_out);
				nvgFillPaint(args.vg, paint);
				nvgFill(args.vg);

				/// OUTPUT PORT HALO
				pos_out = cable->pos_out;
				nvgBeginPath(args.vg);
				nvgRect(args.vg,
				/**/ pos_out.x - radius_out, pos_out.y - radius_out,
				/**/ 2 * radius_out, 2 * radius_out);
				paint = nvgRadialGradient(args.vg,
				/**/ pos_out.x, pos_out.y,
				/**/ radius, radius_out,
				/**/ col_in, col_out);
				nvgFillPaint(args.vg, paint);
				nvgFill(args.vg);

				nvgGlobalCompositeOperation(args.vg, NVG_SOURCE_OVER);
			}
		}
	}

	/// [2] DRAW INCOMPLETE CABLE
	if (this->module->cable_incomplete != BLANK_CABLE_INCOMPLETE_OFF) {
		cable = &(this->module->cables[BLANK_CABLES]);
		/// COMPUTE CABLE POSITION
		pos_in = cable->pos_in;
		pos_out = cable->pos_out;
		pos_slump = get_pos_slump(pos_out, pos_in);
		//pos_out =
		///**/ pos_out.plus(pos_slump.minus(pos_out).normalize().mult(10.5));
		//pos_in =
		///**/ pos_in.plus(pos_slump.minus(pos_in).normalize().mult(10.5));
		//pos_out =
		///**/ pos_out.plus(pos_slump.minus(pos_out).normalize().mult(13.0));
		//pos_in =
		///**/ pos_in.plus(pos_slump.minus(pos_in).normalize().mult(13.0));

		/// SET CABLE COLOR
		if (brightness) {
			color_light = color::mult(cable->color, settings::rackBrightness);
			nvgStrokeColor(args.vg, color_light);
			nvgFillColor(args.vg, color_light);
		} else {
			nvgStrokeColor(args.vg, cable->color);
			nvgFillColor(args.vg, cable->color);
		}
		nvgGlobalAlpha(args.vg, 1.0);

		/// DRAW CABLE PLUG
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, cable->pos_in.x, cable->pos_in.y, 8.5);
		nvgStrokeWidth(args.vg, 4.0);
		nvgStroke(args.vg);
		nvgFill(args.vg);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, cable->pos_out.x, cable->pos_out.y, 8.5);
		nvgStrokeWidth(args.vg, 4.0);
		nvgStroke(args.vg);
		nvgFill(args.vg);

		/// DRAW CABLE
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, VEC_ARGS(pos_in));
		nvgQuadTo(args.vg, VEC_ARGS(pos_slump), VEC_ARGS(pos_out));
		nvgStrokeWidth(args.vg, 6.0);
		nvgStroke(args.vg);
	}
}

