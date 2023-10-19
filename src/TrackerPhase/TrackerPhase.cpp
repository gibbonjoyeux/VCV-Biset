
#include "TrackerPhase.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerPhase::TrackerPhase() {
	int		i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);
	for (i = 0; i < 4; ++i) {
		this->count[i] = 0;
		configParam(PARAM_TYPE + i, 0, 3, 0, "Wave")->snapEnabled = true;
		configParam<ParamQuantityClock>(PARAM_FREQ + i, -32, 32, 0, "Frequency")->snapEnabled = true;
		configParam(PARAM_PHASE + i, 0, 1, 0, "Phase");
		configParam(PARAM_WARP + i, 0, 1, 0.5, "Warp / Pulse Width");
		configParam(PARAM_OFFSET + i, -5, 5, 0, "Offset", "V");
		configParam(PARAM_SCALE + i, -100, 100, 100, "Scale", "%");
		configSwitch(PARAM_INVERT + i, 0, 1, 0, "Invert", {"Off", "On"});
	}
	configParam(PARAM_MODE, 0, 1, 0)->snapEnabled = true;
}

void TrackerPhase::process(const ProcessArgs& args) {
	int		knob_type;
	float	knob_freq;
	float	knob_phase;
	float	knob_warp;
	float	knob_offset;
	float	knob_scale;
	bool	mode;
	int		switch_invert;
	int		multiplier;
	int		divider;
	float	phase;
	float	out;
	bool	trigger;
	int		i;

	if (g_module == NULL || g_timeline == NULL)
		return;

	mode = this->params[PARAM_MODE].getValue();

	/// [1] CHECK GLOBAL PLAY TRIGGER (RESET)
	if (this->trigger_restart.process(g_timeline->play_trigger.remaining > 0.0)) {
		for (i = 0; i < 4; ++i) {
			this->count[i] = 0;
		}
		this->phase_play = g_timeline->clock.phase;
	}

	/// [2] CHECK GLOBAL CLOCK TRIGGER (ON BEATS)
	trigger = (g_timeline->clock.phase < this->phase_play);
	this->phase_play = g_timeline->clock.phase;

	/// TIMELINE STOP
	if (g_timeline->play == TIMELINE_MODE_STOP) {
		for (i = 0; i < 4; ++i) {
			knob_offset = this->params[PARAM_OFFSET + i].getValue();
			this->outputs[OUTPUT_PHASE + i].setVoltage(knob_offset);
		}
		return;
	}
	//// TIMELINE PLAY
	for (i = 0; i < 4; ++i) {
		
		knob_type = this->params[PARAM_TYPE + i].getValue();
		knob_freq = this->params[PARAM_FREQ + i].getValue();
		knob_phase = this->params[PARAM_PHASE + i].getValue();
		knob_warp = this->params[PARAM_WARP + i].getValue();
		knob_offset = this->params[PARAM_OFFSET + i].getValue();
		knob_scale = this->params[PARAM_SCALE + i].getValue();
		switch_invert = this->params[PARAM_INVERT + i].getValue();

		/// [3] COMPUTE PHASE DIV / MULT
		//// CLOCK MULT
		if (knob_freq >= -1.0) {
			/// COMPUTE FREQ
			multiplier = knob_freq;
			if (multiplier < 1)
				multiplier = 1;
			/// COMPUTE PHASE
			phase = g_timeline->clock.phase * (float)multiplier + knob_phase;
			phase = phase - (int)phase;
		//// CLOCK DIV
		} else {
			/// COMPUTE FREQ
			divider = -knob_freq;
			/// COMPUTE PHASE INC
			if (trigger) {
				this->count[i] += 1;
				if (this->count[i] >= divider)
					this->count[i] = 0;
			}
			/// COMPUTE PHASE
			//// MODE FIXED (RESTART ON LOOP)
			if (mode == TPHASE_MODE_FIXED) {
				phase = ((float)g_timeline->clock.beat + g_timeline->clock.phase)
				/**/ / (float)divider + knob_phase;
			//// MODE LOOP (KEEP GOING ON LOOP)
			} else {
				// TODO: Fix double increment phase
				phase = ((float)this->count[i] + this->phase_play)
				/**/ / (float)divider + knob_phase;
			}
			phase = phase - (int)phase;
		}

		/// [4] COMPUTE WAVE
		//// WAVE RAMP UP
		if (knob_type == 0) {
			phase = std::pow(phase, std::pow(2, knob_warp * 4.0 - 2.0));
			out = phase;
		//// WAVE TRIANGLE
		} else if (knob_type == 1) {
			phase = std::pow(phase, std::pow(2, knob_warp * 4.0 - 2.0));
			if (phase < 0.5)
				out = phase * 2.0;
			else
				out = (1.0 - phase) * 2.0;
		//// WAVE SIN
		} else if (knob_type == 2) {
			phase = std::pow(phase, std::pow(2, knob_warp * 4.0 - 2.0));
			out = sin(phase * 2 * M_PI) * 0.5 + 0.5;
		//// WAVE SQUARE
		} else if (knob_type == 3) {
			out = (phase < knob_warp) ? 0.0 : 1.0;
		} else {
			out = 0;
		}

		/// [5] INVERT WAVE
		if (switch_invert)
			out = 1.0 - out;

		/// [6] MAP WAVE
		out = (out * (knob_scale / 100.0) * 10.0) + knob_offset;

		/// [7] OUTPUT VOLTAGE
		this->outputs[OUTPUT_PHASE + i].setVoltage(out);
	}
}

Model* modelTrackerPhase = createModel<TrackerPhase, TrackerPhaseWidget>("Biset-Tracker-Phase");
