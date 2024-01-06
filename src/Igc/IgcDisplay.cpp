
#include "Igc.hpp"

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

IgcDisplay::IgcDisplay() {
}

void IgcDisplay::draw(const DrawArgs &args) {
}

void IgcDisplay::drawLayer(const DrawArgs &args, int layer) {
	IgcCable		*cable;
	math::Vec		pos_in;
	math::Vec		pos_out;
	math::Vec		pos_slump;
	math::Vec		pos_point, pos_point_prev;
	math::Vec		pos_angle;
	Rect			rect;
	Rect			rect_module;
	float			t;
	float			angle;
	float			amp;
	float			length;
	int				buffer_phase;
	int				i, j;

	if (this->module == NULL || layer != 1)
		return;
	if (g_igc != this->module)
		return;

	rect = box.zeroPos();
	rect_module = this->parent->box;

	nvgLineCap(args.vg, 1);
	nvgLineJoin(args.vg, 1);
	for (i = 0; i < this->module->count; ++i) {

		cable = &(this->module->cables[i]);

		/// COMPUTE CABLE POSITION
		pos_in = cable->pos_in;
		pos_out = cable->pos_out;
		pos_slump = get_pos_slump(pos_out, pos_in);
		pos_out =
		/**/ pos_out.plus(pos_slump.minus(pos_out).normalize().mult(13.0));
		pos_in =
		/**/ pos_in.plus(pos_slump.minus(pos_in).normalize().mult(13.0));

		/// COMPUTE CABLE LENGTH
		length = sqrt(
		/**/ (pos_in.x - pos_out.x) * (pos_in.x - pos_out.x)
		/**/ + (pos_in.y - pos_out.y) * (pos_in.y - pos_out.y));
		if (length > IGC_DIST_MAX)
			length = IGC_DIST_MAX;
		if (length < 1.0)
			length = 1.0;
		length = length / IGC_DIST_MAX;

		/// DRAW CABLE
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, VEC_ARGS(pos_in));
		for (j = 0; j < IGC_PRECISION; ++j) {
			t = (float)(j + 1) / (float)IGC_PRECISION;

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

			/// COMPUTE POINT ANIMATED POSITION
			buffer_phase = this->module->buffer_i
			/**/ - t * ((float)IGC_BUFFER * length);
			if (buffer_phase < 0)
				buffer_phase += IGC_BUFFER;
			angle += M_PI * 0.5;
			if (t < 0.2)
				amp = t * 5.0;
			else if (t > 0.8)
				amp = (1.0 - t) * 5.0;
			else
				amp = 1.0;
			pos_point.x += cos(angle) * cable->buffer[buffer_phase] * amp;
			pos_point.y += sin(angle) * cable->buffer[buffer_phase] * amp;

			nvgLineTo(args.vg, VEC_ARGS(pos_point));
		}

		// Draw cable
		nvgStrokeColor(args.vg, cable->color);
		//nvgStrokeColor(args.vg, color::mult(color, 0.95));
		nvgStrokeWidth(args.vg, 6.0);
		nvgStroke(args.vg);
	}
}
