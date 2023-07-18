
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
		this->phase[i] = 0.0;
		configParam(PARAM_TYPE + i, 0, 3, 0, "Wave")->snapEnabled = true;
		configParam<ParamQuantityClock>(PARAM_FREQ + i, -31, 31, 0, "Frequency");
		configParam(PARAM_WIDTH + i, 0, 100, 50, "Width", "%");
		configParam(PARAM_OFFSET + i, -5, 5, 0, "Offset", "V");
		configParam(PARAM_SCALE + i, -100, 100, 100, "Scale", "%");
		configSwitch(PARAM_ROUND + i, 0, 1, 1, "Round frequency", {"Off", "On"});
		configSwitch(PARAM_INVERT + i, 0, 1, 0, "Invert", {"Off", "On"});
	}
}

void TrackerPhase::process(const ProcessArgs& args) {
	int		knob_type;
	float	knob_freq;
	float	knob_width;
	float	knob_offset;
	float	knob_scale;
	int		switch_round;
	int		switch_invert;
	float	freq;
	float	out;
	int		i;

	for (i = 0; i < 4; ++i) {
		
		if (g_timeline.clock_phase_step == 0) {
			this->phase[i] = 0;
			continue;
		}

		knob_type = this->params[PARAM_TYPE + i].getValue();
		knob_freq = this->params[PARAM_FREQ + i].getValue();
		knob_width = this->params[PARAM_WIDTH + i].getValue();
		knob_offset = this->params[PARAM_OFFSET + i].getValue();
		knob_scale = this->params[PARAM_SCALE + i].getValue();
		switch_round = this->params[PARAM_ROUND + i].getValue();
		switch_invert = this->params[PARAM_INVERT + i].getValue();
		if (switch_round)
			knob_freq = (int)knob_freq;

		/// [1] COMPUTE FREQUENCY
		//// FREQ MULT
		if (knob_freq >= 0)
			freq = 1.0 + knob_freq;
		//// FREQ DIV
		else
			freq = 1.0 / (1.0 - knob_freq);

		/// [2] COMPUTE PHASE
		this->phase[i] += g_timeline.clock_phase_step * freq;
		this->phase[i] = this->phase[i] - (int)this->phase[i];

		/// [3] COMPUTE WAVE
		//// WAVE RAMP UP
		if (knob_type == 0) {
			out = this->phase[i];
		//// WAVE TRIANGLE
		} else if (knob_type == 1) {
			if (this->phase[i] < 0.5)
				out = this->phase[i] * 2.0;
			else
				out = (1.0 - this->phase[i]) * 2.0;
		//// WAVE SIN
		} else if (knob_type == 2) {
			out = sin(this->phase[i] * 2 * M_PI) * 0.5 + 0.5;
		//// WAVE SQUARE
		} else if (knob_type == 3) {
			out = (this->phase[i] < knob_width / 100.0) ? 0.0 : 1.0;
		} else {
			out = 0;
		}

		/// [4] INVERT WAVE
		if (switch_invert)
			out = 1.0 - out;

		/// [5] MAP WAVE
		out = (out * (knob_scale / 100.0) * 10.0) + knob_offset;

		/// [6] OUTPUT VOLTAGE
		this->outputs[OUTPUT_PHASE + i].setVoltage(out);
	}
}

Model* modelTrackerPhase = createModel<TrackerPhase, TrackerPhaseWidget>("Biset-Tracker-Phase");
