
#include "Igc.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

IgcDisplay::IgcDisplay() {
}

void IgcDisplay::draw(const DrawArgs &args) {
	//Rect		rect;

	//rect = box.zeroPos();
	///// RECT BACKGROUND
	//nvgBeginPath(args.vg);
	//nvgFillColor(args.vg, colors[14]);
	//nvgRect(args.vg, RECT_ARGS(rect));
	//nvgFill(args.vg);
}

void IgcDisplay::drawLayer(const DrawArgs &args, int layer) {
	IgcPlayhead	*playhead;
	Rect		rect;
	int			buffer_phase;
	int			buffer_phase_prev;
	float		voltage_l, voltage_r;
	float		voltage_max_l, voltage_max_r;
	float		voltage_prev_l, voltage_prev_r;
	float		voltage_diff_l, voltage_diff_r;
	float		voltage_diff_max_l, voltage_diff_max_r;
	float		delay_time;
	float		t;
	int			i;

	if (this->module == NULL || layer != 1)
		return;

	rect = box.zeroPos();
	delay_time = this->module->delay_time;

	nvgScissor(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y);

	/// [1] DRAW AUDIO RECORDING

	nvgBeginPath(args.vg);
	buffer_phase_prev = this->module->audio_index - 1;
	if (buffer_phase_prev < 0)
		buffer_phase_prev += IGC_BUFFER;
	voltage_prev_l = 0.0;
	voltage_prev_r = 0.0;
	for (i = 0; i < IGC_BUFFER_PRECISION; ++i) {

		t = (float)i / (float)IGC_BUFFER_PRECISION;

		/// COMPUTE VOLTAGE
		/// COMPUTE BUFFER PHASE
		buffer_phase = this->module->audio_index - 1
		/**/ - t * (float)IGC_BUFFER * (delay_time * 0.1);
		if (buffer_phase < 0)
			buffer_phase += IGC_BUFFER;
		/// COMPUTE VOLTAGE
		voltage_diff_max_l = 0;
		voltage_diff_max_r = 0;
		voltage_max_l = voltage_prev_l;
		voltage_max_r = voltage_prev_r;
		while (buffer_phase_prev != buffer_phase) {
			voltage_l = this->module->audio[0][buffer_phase_prev];
			voltage_r = this->module->audio[1][buffer_phase_prev];
			/// COMPUTE DIFF LEFT
			voltage_diff_l = voltage_prev_l - voltage_l;
			if (voltage_diff_l < 0)
				voltage_diff_l = -voltage_diff_l;
			if (voltage_diff_l > voltage_diff_max_l) {
				voltage_diff_max_l = voltage_diff_l;
				voltage_max_l = voltage_l;
			}
			/// COMPUTE DIFF RIGHT
			voltage_diff_r = voltage_prev_r - voltage_r;
			if (voltage_diff_r < 0)
				voltage_diff_r = -voltage_diff_r;
			if (voltage_diff_r > voltage_diff_max_r) {
				voltage_diff_max_r = voltage_diff_r;
				voltage_max_r = voltage_r;
			}
			buffer_phase_prev -= 1;
			if (buffer_phase_prev < 0)
				buffer_phase_prev += IGC_BUFFER;
		}
		voltage_prev_l = voltage_max_l;
		voltage_prev_r = voltage_max_r;
		voltage_l = voltage_max_l;
		voltage_r = voltage_max_r;

		/// DRAW POINT LEFT
		if (voltage_l > 10.0)
			voltage_l = 10.0;
		if (voltage_l < -10.0)
			voltage_l = -10.0;
		nvgRect(args.vg,
		/**/ rect.size.x * t,
		/**/ rect.size.y * 0.25 - (voltage_l * 0.2) * rect.size.y * 0.25,
		/**/ rect.size.x / (float)IGC_BUFFER_PRECISION * 0.5,
		/**/ rect.size.y * 0.25 * (voltage_l * 0.2) * 2.0);
		/// DRAW POINT RIGHT
		if (voltage_r > 10.0)
			voltage_r = 10.0;
		if (voltage_r < -10.0)
			voltage_r = -10.0;
		nvgRect(args.vg,
		/**/ rect.size.x * t,
		/**/ rect.size.y * 0.75 - (voltage_l * 0.2) * rect.size.y * 0.25,
		/**/ rect.size.x / (float)IGC_BUFFER_PRECISION * 0.5,
		/**/ rect.size.y * 0.25 * (voltage_l * 0.2) * 2.0);
	}
	nvgFillColor(args.vg, {0xec / 255.0, 0xae / 255.0, 0x52 / 255.0, 1.0});
	nvgFill(args.vg);

	/// [2] DRAW PLAYHEADS

	nvgBeginPath(args.vg);
	for (i = 0; i < this->module->playhead_count; ++i) {
		playhead = &(this->module->playheads[i]);
		nvgRect(args.vg,
		/**/ playhead->phase * rect.size.x,
		/**/ rect.size.y,
		/**/ 0.5,
		/**/ -rect.size.y * playhead->level);
	}
	nvgFillColor(args.vg, colors[12]);
	nvgFill(args.vg);

	nvgResetScissor(args.vg);
}
