
#include "Igc.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

IgcScope::IgcScope() {
}

void IgcScope::draw(const DrawArgs &args) {
	math::Vec	pos_point;
	IgcCable	*cable;
	Rect		box;
	int			buffer_phase;
	int			position;
	bool		details;
	bool		mode;
	float		scale;
	float		t;
	int			i;


	if (g_igc != this->module)
		return;
	if (this->module->params[Igc::PARAM_SCOPE_ENABLED].getValue() == 0.0)
		return;
	if (this->module->scope_index < 0)
		return;

	cable = &(this->module->cables[this->module->scope_index]);
	scale = this->module->params[Igc::PARAM_SCOPE_SCALE].getValue();
	position = this->module->params[Igc::PARAM_SCOPE_POSITION].getValue();
	mode = this->module->params[Igc::PARAM_SCOPE_MODE].getValue();
	details = this->module->params[Igc::PARAM_SCOPE_DETAILS].getValue();
	box.size.x = scale * APP->scene->box.size.x;
	box.size.y = scale * APP->scene->box.size.x * 0.5;
	if (position == IGC_SCOPE_TOP_LEFT) {
		box.pos.x = 10.0;
		box.pos.y = 40.0;
	} else if (position == IGC_SCOPE_TOP_RIGHT) {
		box.pos.x = APP->scene->box.size.x - (box.size.x + 10.0);
		box.pos.y = 40.0;
	} else if (position == IGC_SCOPE_BOTTOM_LEFT) {
		box.pos.x = 10.0;
		box.pos.y = APP->scene->box.size.y - (box.size.y + 10.0);
	} else if (position == IGC_SCOPE_BOTTOM_RIGHT) {
		box.pos.x = APP->scene->box.size.x - (box.size.x + 10.0);
		box.pos.y = APP->scene->box.size.y - (box.size.y + 10.0);
	} else {
		box.pos.x = APP->scene->box.size.x * 0.5 - box.size.x * 0.5;
		box.pos.y = APP->scene->box.size.y * 0.5 - box.size.y * 0.5;
	}

	/// DRAW BOX
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[12]);
	nvgRect(args.vg, box.pos.x, box.pos.y, box.size.x, box.size.y);
	nvgFill(args.vg);

	/// DRAW DETAILS
	if (details) {
		/// 0V LINE
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg,
		/**/ box.pos.x, box.pos.y + box.size.y * 0.5);
		nvgLineTo(args.vg,
		/**/ box.pos.x + box.size.x, box.pos.y + box.size.y * 0.5);
		nvgStrokeColor(args.vg, colors[15]);
		nvgStrokeWidth(args.vg, 1.0);
		nvgStroke(args.vg);
		/// 5V LINES
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg,
		/**/ box.pos.x, box.pos.y + box.size.y * 0.25);
		nvgLineTo(args.vg,
		/**/ box.pos.x + box.size.x, box.pos.y + box.size.y * 0.25);
		nvgMoveTo(args.vg,
		/**/ box.pos.x, box.pos.y + box.size.y * 0.75);
		nvgLineTo(args.vg,
		/**/ box.pos.x + box.size.x, box.pos.y + box.size.y * 0.75);
		nvgStrokeColor(args.vg, colors[14]);
		nvgStrokeWidth(args.vg, 1.0);
		nvgStroke(args.vg);
	}

	/// DRAW WAVE
	nvgScissor(args.vg, box.pos.x, box.pos.y, box.size.x, box.size.y);
	nvgBeginPath(args.vg);
	for (i = 0; i < IGC_PRECISION_SCOPE; ++i) {
		t = (float)i / (float)IGC_PRECISION_SCOPE;

		/// COMPUTE BUFFER PLAYHEAD
		if (mode == IGC_SCOPE_CIRCULAR) {
			buffer_phase = this->module->buffer_i - 1 - t * (float)IGC_BUFFER;
			if (buffer_phase < 0)
				buffer_phase += IGC_BUFFER;
		} else {
			buffer_phase = t * (float)IGC_BUFFER;
		}

		/// DRAW POINT
		pos_point.x = box.pos.x + t * box.size.x;
		pos_point.y = box.pos.y + box.size.y * 0.5
		/**/ - cable->buffer[buffer_phase] * 0.05 * box.size.y;
		if (i == 0)
			nvgMoveTo(args.vg, VEC_ARGS(pos_point));
		else
			nvgLineTo(args.vg, VEC_ARGS(pos_point));
	}
	nvgStrokeColor(args.vg, colors[0]);
	nvgStrokeWidth(args.vg, 2.0);
	nvgStroke(args.vg);
	nvgResetScissor(args.vg);
}
