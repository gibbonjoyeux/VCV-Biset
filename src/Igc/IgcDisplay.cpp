
#include "Igc.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static math::Vec getSlumpPos(math::Vec pos1, math::Vec pos2) {
	float dist = pos1.minus(pos2).norm();
	math::Vec avg = pos1.plus(pos2).div(2);
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
	Rect	rect;

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[12]);
	//nvgRect(args.vg, RECT_ARGS(rect));
	nvgRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0);
	nvgFill(args.vg);

}

void IgcDisplay::drawLayer(const DrawArgs &args, int layer) {
	IgcCable		*cable;
	math::Vec		pos_in;
	math::Vec		pos_out;
	math::Vec		pos_slump;
	math::Vec		pos_point;
	math::Vec		pos_angle;
	Rect			rect;
	Rect			rect_module;
	float			t;
	float			angle;
	float			amp;
	int				buffer_phase;
	int				i, j;

	if (this->module == NULL || layer != 1)
		return;

	rect = box.zeroPos();
	rect_module = this->parent->box;

	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[14]);
	//nvgRect(args.vg, RECT_ARGS(rect));
	//nvgRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0);
	nvgRect(args.vg, 0, 0, rect.size.x, rect.size.y + 1.0);
	nvgFill(args.vg);

	nvgTranslate(args.vg, -rect_module.pos.x, -rect_module.pos.y);
	nvgLineCap(args.vg, 1);
	nvgLineJoin(args.vg, 1);
	for (i = 0; i < this->module->count; ++i) {
		cable = &(this->module->cables[i]);

		pos_in = cable->pos_in;
		pos_out = cable->pos_out;
		pos_slump = getSlumpPos(pos_out, pos_in);
		pos_out =
		/**/ pos_out.plus(pos_slump.minus(pos_out).normalize().mult(13.0));
		pos_in =
		/**/ pos_in.plus(pos_slump.minus(pos_in).normalize().mult(13.0));

		// Draw cable outline
		nvgBeginPath(args.vg);
		//nvgMoveTo(args.vg, VEC_ARGS(pos_out));
		//nvgQuadTo(args.vg, VEC_ARGS(pos_slump), VEC_ARGS(pos_in));
		nvgMoveTo(args.vg, VEC_ARGS(pos_in));
		for (j = 0; j < IGC_PRECISION; ++j) {
			t = (float)(j + 1) / (float)IGC_PRECISION;

			pos_angle.x = 2.0 * (1.0 - t) * (pos_slump.x - pos_in.x)
			/**/ + 2.0 * t * (pos_out.x - pos_slump.x);
			pos_angle.y = 2.0 * (1.0 - t) * (pos_slump.y - pos_in.y)
			/**/ + 2.0 * t * (pos_out.y - pos_slump.y);
			angle = atan2(pos_angle.y, pos_angle.x);

			pos_point.x =
			/**/ (1.0 - t) * (1.0 - t) * pos_in.x
			/**/ + 2 * (1.0 - t) * t * pos_slump.x
			/**/ + t * t * pos_out.x;
			pos_point.y =
			/**/ (1.0 - t) * (1.0 - t) * pos_in.y
			/**/ + 2 * (1.0 - t) * t * pos_slump.y
			/**/ + t * t * pos_out.y;

			buffer_phase = this->module->buffer_i - t * IGC_BUFFER;
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
		//nvgStrokeWidth(args.vg, thickness - 1.0);
		nvgStroke(args.vg);
	}
	nvgResetTransform(args.vg);
}
