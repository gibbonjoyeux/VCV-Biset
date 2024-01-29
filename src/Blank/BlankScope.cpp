
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
	bool		mode;
	float		details;
	float		background;
	float		scale;
	float		alpha;
	float		thickness;
	float		t;
	float		voltage, voltage_prev;
	float		voltage_diff, voltage_diff_max, voltage_max;
	int			buffer_phase, buffer_phase_prev;
	int			position;
	int			i;


	if (g_blank != this->module)
		return;
	if (this->module->params[Blank::PARAM_SCOPE_ENABLED].getValue() == 0.0)
		return;
	if (this->module->scope_index < 0)
		return;

	cable = &(this->module->cables[this->module->scope_index]);
	scale = this->module->params[Blank::PARAM_SCOPE_SCALE].getValue();
	position = this->module->params[Blank::PARAM_SCOPE_POSITION].getValue();
	mode = this->module->params[Blank::PARAM_SCOPE_MODE].getValue();
	thickness = this->module->params[Blank::PARAM_SCOPE_THICKNESS].getValue();
	details = this->module->params[Blank::PARAM_SCOPE_VOLT_ALPHA].getValue();
	background = this->module->params[Blank::PARAM_SCOPE_BACK_ALPHA].getValue();
	alpha = this->module->params[Blank::PARAM_SCOPE_ALPHA].getValue();
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

	/// DRAW BOX
	if (background >= 0) {
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, (NVGcolor){0, 0, 0, background});
		nvgRect(args.vg, box.pos.x, box.pos.y, box.size.x, box.size.y);
		nvgFill(args.vg);

	}

	/// DRAW DETAILS
	if (details >= 0) {
		nvgStrokeColor(args.vg, (NVGcolor){1, 1, 1, details});
		nvgStrokeWidth(args.vg, 1.0);
		/// 0V LINE
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg,
		/**/ box.pos.x, box.pos.y + box.size.y * 0.5);
		nvgLineTo(args.vg,
		/**/ box.pos.x + box.size.x, box.pos.y + box.size.y * 0.5);
		/// 5V LINES
		//// 0.5 - 0.25 * 0.8
		//// 0.5 + 0.25 * 0.8
		nvgMoveTo(args.vg,
		/**/ box.pos.x, box.pos.y + box.size.y * 0.3);
		nvgLineTo(args.vg,
		/**/ box.pos.x + box.size.x, box.pos.y + box.size.y * 0.3);
		nvgMoveTo(args.vg,
		/**/ box.pos.x, box.pos.y + box.size.y * 0.7);
		nvgLineTo(args.vg,
		/**/ box.pos.x + box.size.x, box.pos.y + box.size.y * 0.7);
		/// 10V LINES
		//// 0.5 - 0.5 * 0.8
		//// 0.5 + 0.5 * 0.8
		nvgMoveTo(args.vg,
		/**/ box.pos.x, box.pos.y + box.size.y * 0.1);
		nvgLineTo(args.vg,
		/**/ box.pos.x + box.size.x, box.pos.y + box.size.y * 0.1);
		nvgMoveTo(args.vg,
		/**/ box.pos.x, box.pos.y + box.size.y * 0.9);
		nvgLineTo(args.vg,
		/**/ box.pos.x + box.size.x, box.pos.y + box.size.y * 0.9);
		nvgStroke(args.vg);
	}

	/// DRAW WAVE
	nvgScissor(args.vg, box.pos.x, box.pos.y, box.size.x, box.size.y);
	nvgBeginPath(args.vg);
	buffer_phase_prev = this->module->buffer_i;
	voltage_prev = 0.0;
	for (i = 0; i < BLANK_PRECISION_SCOPE; ++i) {
		t = (float)i / (float)BLANK_PRECISION_SCOPE;

		/// COMPUTE VOLTAGE
		if (mode == BLANK_SCOPE_CIRCULAR) {
			/// COMPUTE BUFFER PHASE
			buffer_phase = this->module->buffer_i - 1 - t * (float)BLANK_BUFFER;
			if (buffer_phase < 0)
				buffer_phase += BLANK_BUFFER;
			/// COMPUTE VOLTAGE
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
			voltage_prev = voltage_max;
			voltage = voltage_max;

		} else {
			buffer_phase = t * (float)BLANK_BUFFER;
			voltage = cable->buffer[buffer_phase];
		}

		/// DRAW POINT
		pos_point.x = box.pos.x + t * box.size.x;
		pos_point.y = box.pos.y + box.size.y * 0.5
		/**/ - voltage * 0.05 * box.size.y * 0.8;
		if (i == 0)
			nvgMoveTo(args.vg, VEC_ARGS(pos_point));
		else
			nvgLineTo(args.vg, VEC_ARGS(pos_point));
	}
	nvgStrokeColor(args.vg, cable->color);
	nvgStrokeWidth(args.vg, thickness);
	nvgStroke(args.vg);
	nvgResetScissor(args.vg);
}
