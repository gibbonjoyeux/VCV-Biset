
#include "Omega3.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Omega3::Omega3() {

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configParam(PARAM_POLYPHONY, 1, 16, 16, "Polyphony")->snapEnabled = true;
	configParam(PARAM_SHAPE, 0, 1, 1, "Shape", "%", 0, 100);
	configParam(PARAM_CURVE, -1, 1, 0, "Curve", "%", 0, 100);
	configParam(PARAM_OFFSET, -1, +1, 0, "Offset", " V", 0, 5);
	configParam(PARAM_SCALE, 0, 1, 1, "Scale", "%", 0, 100);
	configParam(PARAM_PHASE, -1, 1, 0, "Phase");

	configSwitch(PARAM_CURVE_ORDER, 0, 1, 0, "Curve order");

	configInput(INPUT_PHASE, "Phase");

	configOutput(OUTPUT_CV, "CV");
}

void Omega3::process(const ProcessArgs& args) {
	float	input;
	float	phase;
	float	offset;
	float	scale;
	float	curve;
	float	shape;
	float	t;
	bool	curve_order;
	int		channels;
	int		i;

	if (args.frame % 32 != 0)
		return;

	channels = params[PARAM_POLYPHONY].getValue();
	offset = params[PARAM_OFFSET].getValue() * 5.0;
	scale = params[PARAM_SCALE].getValue() * 10.0;
	curve = params[PARAM_CURVE].getValue();
	shape = params[PARAM_SHAPE].getValue();
	phase = params[PARAM_PHASE].getValue();
	curve_order = params[PARAM_CURVE_ORDER].getValue();

	input = inputs[INPUT_PHASE].getVoltageSum();

	outputs[OUTPUT_CV].setChannels(channels);

	for (i = 0; i < channels; ++i) {

		/// [1] INIT PHASE
		t = (float)i / (float)channels;

		/// [2] HANDLE CURVE (PRE)
		if (curve_order == 0)
			t = std::pow(t, std::pow(2, curve * 2.0));

		/// [3] HANDLE PHASE
		t += phase + input * 0.1;
		t = fmod(fmod(t, 1.0) + 1.0, 1.0);

		/// [4] HANDLE SHAPE
		//// WAVE UP
		if (t < shape) {
			/// AVOID ZERO DIVISION
			if (shape > 0.0001)
				t = 1.0 - (1.0 - (t / shape));
		//// WAVE DOWN
		} else {
			/// AVOID ZERO DIVISION
			if (shape < 0.9999)
				t = 1.0 - ((t - shape) / (1.0 - shape));
		}

		/// [5] HANDLE CURVE (POST)
		if (curve_order == 1)
			t = std::pow(t, std::pow(2, curve * 2.0));

		/// [6] HANDLE RANGE
		outputs[OUTPUT_CV].setVoltage(t * scale + offset, i);
	}
}

Model* modelOmega3 = createModel<Omega3, Omega3Widget>("Biset-Omega3");
