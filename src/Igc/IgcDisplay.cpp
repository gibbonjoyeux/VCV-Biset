
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
	bool		stereo;
	float		voltage_l, voltage_r;
	float		voltage_max_l, voltage_max_r;
	float		voltage_prev_l, voltage_prev_r;
	float		voltage_diff_l, voltage_diff_r;
	float		voltage_diff_max_l, voltage_diff_max_r;
	float		delay_time;
	float		dist_1, dist_1_abs;
	float		dist_2, dist_2_abs;
	float		offset, offset_abs;
	float		t;
	int			i;

	if (this->module == NULL || layer != 1)
		return;

	rect = box.zeroPos();
	delay_time = this->module->delay_time;
	stereo = this->module->is_stereo;

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

		if (voltage_l > 10.0)
			voltage_l = 10.0;
		if (voltage_l < -10.0)
			voltage_l = -10.0;
		if (voltage_r > 10.0)
			voltage_r = 10.0;
		if (voltage_r < -10.0)
			voltage_r = -10.0;
		if (stereo) {
			/// DRAW POINT LEFT
			nvgRect(args.vg,
			/**/ rect.size.x * t,
			/**/ rect.size.y * 0.25 - (voltage_l * 0.2) * rect.size.y * 0.25,
			/**/ rect.size.x / (float)IGC_BUFFER_PRECISION * 0.5,
			/**/ rect.size.y * 0.25 * (voltage_l * 0.2) * 2.0);
			/// DRAW POINT RIGHT
			nvgRect(args.vg,
			/**/ rect.size.x * t,
			/**/ rect.size.y * 0.75 - (voltage_r * 0.2) * rect.size.y * 0.25,
			/**/ rect.size.x / (float)IGC_BUFFER_PRECISION * 0.5,
			/**/ rect.size.y * 0.25 * (voltage_r * 0.2) * 2.0);
		} else {
			/// DRAW POINT MONO
			nvgRect(args.vg,
			/**/ rect.size.x * t,
			/**/ rect.size.y * 0.5 - (voltage_l * 0.2) * rect.size.y * 0.5,
			/**/ rect.size.x / (float)IGC_BUFFER_PRECISION * 0.5,
			/**/ rect.size.y * 0.5 * (voltage_l * 0.2) * 2.0);
		}
	}
	nvgFillColor(args.vg, {0xec / 255.0, 0xae / 255.0, 0x52 / 255.0, 1.0});
	nvgFill(args.vg);

	/// [2] DRAW PLAYHEADS

	nvgGlobalAlpha(args.vg, 0.5);
	nvgFillColor(args.vg, colors[12]);
	nvgStrokeColor(args.vg, colors[12]);
	nvgStrokeWidth(args.vg, 0.5);
	/// DRAW TRAIL
	for (i = 0; i < this->module->playhead_count; ++i) {

		playhead = &(this->module->playheads[i]);
		if (playhead->level < 0.005)
			continue;

		/// COMPUTE TRAIL
		//// JUMP DIRECT
		dist_1 = playhead->phase - playhead->phase_prev;
		dist_1_abs = (dist_1 <  0) ? -dist_1 : +dist_1;
		//// JUMP CIRCULAR
		if (playhead->phase < playhead->phase_prev) {
			dist_2_abs = playhead->phase + (1.0 - playhead->phase_prev);
			dist_2 = dist_2_abs;
		} else {
			dist_2_abs = playhead->phase_prev + (1.0 - playhead->phase);
			dist_2 = -dist_2_abs;
		}
		//// JUMP SMALLEST
		if (dist_1_abs < dist_2_abs)
			offset = dist_1 * rect.size.x;
		else
			offset = dist_2 * rect.size.x;
		offset_abs = (offset < 0) ? -offset : offset;
		if (offset_abs > 10)
			offset = (offset < 0) ? -10 : 10;

		/// DRAW PLAYHEAD
		nvgBeginPath(args.vg);

		nvgMoveTo(args.vg,
		/**/ playhead->phase * rect.size.x,
		/**/ rect.size.y);
		nvgLineTo(args.vg,
		/**/ playhead->phase * rect.size.x,
		/**/ (1.0 - playhead->level) * rect.size.y);
		nvgQuadTo(args.vg,
		/**/ playhead->phase * rect.size.x - offset * 0.333,
		/**/ rect.size.y,
		/**/ playhead->phase * rect.size.x - offset,
		/**/ rect.size.y);
		nvgLineTo(args.vg,
		/**/ playhead->phase * rect.size.x,
		/**/ rect.size.y);

		nvgFill(args.vg);
		nvgStroke(args.vg);
		nvgClosePath(args.vg);

		/// UPDATE PREVIOUS PHASE (TRAIL)
		playhead->phase_prev = playhead->phase;
	}
	nvgGlobalAlpha(args.vg, 1.0);

	nvgResetScissor(args.vg);
}
