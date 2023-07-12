
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
		this->phases[i] = 0.0;
		configParam(PARAM_TYPE + i, 0, 4, 0, "Wave")->snapEnabled = true;
		configParam<ParamQuantityClock>(PARAM_FREQ + i, -31, 31, 0, "Frequency");
		configParam(PARAM_WIDTH + i, 0, 100, 50, "Width", "%");
		configParam(PARAM_OFFSET + i, -5, 5, 0, "Offset", "V");
		configParam(PARAM_SCALE + i, -100, 100, 100, "Scale", "%");
		configSwitch(PARAM_ROUND + i, 0, 1, 1, "Round frequency", {"On", "Off"});
		configSwitch(PARAM_INVERSE + i, 0, 1, 0, "Inverse", {"On", "Off"});
	}
}

void TrackerPhase::process(const ProcessArgs& args) {
	int		knob_type;
	float	knob_freq;
	float	knob_width;
	int		switch_round;
	int		switch_inverse;
	float	freq;
	float	out;
	int		i;

	for (i = 0; i < 4; ++i) {
		
		if (this->outputs[OUTPUT_PHASE + i].isConnected() == false)
			continue;

		knob_type = this->params[PARAM_TYPE + i].getValue();
		knob_freq = this->params[PARAM_FREQ + i].getValue();
		knob_width = this->params[PARAM_WIDTH + i].getValue();
		switch_round = this->params[PARAM_ROUND + i].getValue();
		switch_inverse = this->params[PARAM_INVERSE + i].getValue();

		/// [1] COMPUTE FREQUENCY
		//// FREQ MULT
		if (knob_freq >= 0)
			freq = 1.0 + knob_freq;
		//// FREQ DIV
		else
			freq = 1.0 / (1.0 - knob_freq);

		/// [2] COMPUTE PHASE
		this->phases[i] += g_timeline.clock_phase_step * freq;
		this->phases[i] = this->phases[i] - (int)this->phases[i];

		/// [3] COMPUTE WAVE
		//// WAVE RAMP UP
		if (knob_type == 0) {
			out = this->phases[i];
		//// WAVE RAMP DOWN
		} else if (knob_type == 1) {
			out = 1.0 - this->phases[i];
		//// WAVE TRIANGLE
		} else if (knob_type == 2) {
			if (this->phases[i] < 0.5)
				out = this->phases[i] * 2.0;
			else
				out = (1.0 - this->phases[i]) * 2.0;
		//// WAVE SIN
		} else if (knob_type == 3) {
			out = sin(this->phases[i] * 2 * M_PI) * 0.5 + 0.5;
		//// WAVE SQUARE
		} else if (knob_type == 4) {
			out = (this->phases[i] < knob_width / 100.0) ? 0.0 : 1.0;
		} else {
			out = 0;
		}

		/// [4] OUTPUT VOLTAGE
		this->outputs[OUTPUT_PHASE + i].setVoltage(out * 10.0);
	}
}

Model* modelTrackerPhase = createModel<TrackerPhase, TrackerPhaseWidget>("Biset-Tracker-Phase");
