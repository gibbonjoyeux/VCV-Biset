
#include "Blank.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

BlankScope::BlankScope() {
}

void BlankScope::draw(const DrawArgs &args) {
	math::Vec	pos_point;
	BlankCable	*cable;
	Rect		box;
	int			buffer_phase;
	int			position;
	bool		details;
	bool		mode;
	bool		background;
	float		scale;
	float		alpha;
	float		t;
	int			i;


	if (g_igc != this->module)
		return;
	if (this->module->params[Blank::PARAM_SCOPE_ENABLED].getValue() == 0.0)
		return;
	if (this->module->scope_index < 0)
		return;

	cable = &(this->module->cables[this->module->scope_index]);
	scale = this->module->params[Blank::PARAM_SCOPE_SCALE].getValue();
	alpha = this->module->params[Blank::PARAM_SCOPE_ALPHA].getValue();
	position = this->module->params[Blank::PARAM_SCOPE_POSITION].getValue();
	mode = this->module->params[Blank::PARAM_SCOPE_MODE].getValue();
	details = this->module->params[Blank::PARAM_SCOPE_DETAILS].getValue();
	background = this->module->params[Blank::PARAM_SCOPE_BACKGROUND].getValue();
	box.size.x = scale * APP->scene->box.size.x;
	box.size.y = scale * APP->scene->box.size.x * 0.5;
	if (position == BLANK_SCOPE_TOP_LEFT) {
		box.pos.x = 10.0;
		box.pos.y = 40.0;
	} else if (position == BLANK_SCOPE_TOP_RIGHT) {
		box.pos.x = APP->scene->box.size.x - (box.size.x + 10.0);
		box.pos.y = 40.0;
	} else if (position == BLANK_SCOPE_BOTTOM_LEFT) {
		box.pos.x = 10.0;
		box.pos.y = APP->scene->box.size.y - (box.size.y + 10.0);
	} else if (position == BLANK_SCOPE_BOTTOM_RIGHT) {
		box.pos.x = APP->scene->box.size.x - (box.size.x + 10.0);
		box.pos.y = APP->scene->box.size.y - (box.size.y + 10.0);
	} else {
		box.pos.x = APP->scene->box.size.x * 0.5 - box.size.x * 0.5;
		box.pos.y = APP->scene->box.size.y * 0.5 - box.size.y * 0.5;
	}

	nvgAlpha(args.vg, alpha);

	if (background) {
		/// DRAW BOX
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, (NVGcolor){0, 0, 0, 1});
		nvgRect(args.vg, box.pos.x, box.pos.y, box.size.x, box.size.y);
		nvgFill(args.vg);

		/// DRAW DETAILS
		if (details) {
			nvgStrokeColor(args.vg, (NVGcolor){0.3, 0.3, 0.3, 1});
			nvgStrokeWidth(args.vg, 1.0);
			/// 0V LINE
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg,
			/**/ box.pos.x, box.pos.y + box.size.y * 0.5);
			nvgLineTo(args.vg,
			/**/ box.pos.x + box.size.x, box.pos.y + box.size.y * 0.5);
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
			nvgStroke(args.vg);
		}
	}

	/// DRAW WAVE
	nvgScissor(args.vg, box.pos.x, box.pos.y, box.size.x, box.size.y);
	nvgBeginPath(args.vg);
	for (i = 0; i < BLANK_PRECISION_SCOPE; ++i) {
		t = (float)i / (float)BLANK_PRECISION_SCOPE;

		/// COMPUTE BUFFER PLAYHEAD
		if (mode == BLANK_SCOPE_CIRCULAR) {
			buffer_phase = this->module->buffer_i - 1 - t * (float)BLANK_BUFFER;
			if (buffer_phase < 0)
				buffer_phase += BLANK_BUFFER;
		} else {
			buffer_phase = t * (float)BLANK_BUFFER;
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
	nvgStrokeColor(args.vg, cable->color);
	nvgStrokeWidth(args.vg, 2.0);
	nvgStroke(args.vg);
	nvgResetScissor(args.vg);
}