
#include "Omega6.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Omega6::Omega6() {

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configParam(PARAM_POLYPHONY, 1, 16, 16, "Polyphony")->snapEnabled = true;
	configParam(PARAM_SHAPE, 0, 1, 1, "Shape", "%", 0, 100);
	configParam(PARAM_CURVE, -1, 1, 0, "Curve", "%", 0, 100);
	configParam(PARAM_PHASE, -1, 1, 0, "Phase");
	configParam(PARAM_FROM, -10, 10, 0, "From", " V");
	configParam(PARAM_TO, -10, 10, 0, "To", " V");

	configSwitch(PARAM_CURVE_ORDER, 0, 1, 0, "Curve order");

	configInput(INPUT_PHASE, "Phase");
	configInput(INPUT_FROM, "From");
	configInput(INPUT_TO, "To");

	configOutput(OUTPUT_CV, "CV");
}

void Omega6::process(const ProcessArgs& args) {
	float	phase;
	float	from;
	float	to;
	float	curve;
	float	shape;
	float	t;
	bool	curve_order;
	int		channels;
	int		i;

	if (args.frame % 32 != 0)
		return;

	channels = params[PARAM_POLYPHONY].getValue();
	curve = params[PARAM_CURVE].getValue();
	shape = params[PARAM_SHAPE].getValue();
	curve_order = params[PARAM_CURVE_ORDER].getValue();

	phase = params[PARAM_PHASE].getValue()
	/**/ + inputs[INPUT_PHASE].getVoltageSum() * 0.1;
	from = params[PARAM_FROM].getValue()
	/**/ + inputs[INPUT_FROM].getVoltageSum();
	to = params[PARAM_TO].getValue()
	/**/ + inputs[INPUT_TO].getVoltageSum();

	outputs[OUTPUT_CV].setChannels(channels);

	for (i = 0; i < channels; ++i) {

		/// [1] INIT PHASE
		t = (float)i / (float)channels;

		/// [2] HANDLE CURVE (PRE)
		if (curve_order == 0)
			t = std::pow(t, std::pow(2, curve * 2.0));

		/// [3] HANDLE PHASE
		t = fmod(fmod(t + phase, 1.0) + 1.0, 1.0);

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
		outputs[OUTPUT_CV].setVoltage(from * (1.0 - t) + to * t, i);
	}
}

Model* modelOmega6 = createModel<Omega6, Omega6Widget>("Biset-Omega6");
