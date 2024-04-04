
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
	configParam(PARAM_OFFSET, -1, +1, 0, "Offset", " V", 0, 10);
	configParam(PARAM_SCALE, -1, +1, 1, "Scale", "%", 0, 100);
	configParam(PARAM_PHASE, -1, 1, 0, "Phase", "%", 0, 100);

	configSwitch(PARAM_CURVE_ORDER, 0, 1, 0, "Curve order");

	configInput(INPUT_OFFSET, "Offset");
	configInput(INPUT_SCALE, "Scale");
	configInput(INPUT_PHASE, "Phase");

	configOutput(OUTPUT_CV, "CV");
}

void Omega3::process(const ProcessArgs& args) {
	float	phase;
	float	offset;
	float	scale;
	float	curve;
	float	shape;
	bool	curve_order;
	int		channels;

	if (args.frame % 32 != 0)
		return;

	/// [1] GET PARAMETERS

	channels = params[PARAM_POLYPHONY].getValue();
	curve = params[PARAM_CURVE].getValue();
	shape = params[PARAM_SHAPE].getValue();
	curve_order = params[PARAM_CURVE_ORDER].getValue();
	phase = params[PARAM_PHASE].getValue()
	/**/ + inputs[INPUT_PHASE].getVoltageSum() * 0.1;

	offset = params[PARAM_OFFSET].getValue() * 10.0
	/**/ + inputs[INPUT_OFFSET].getVoltageSum();
	scale = params[PARAM_SCALE].getValue() * 10.0
	/**/ + inputs[INPUT_SCALE].getVoltageSum();

	/// [2] COMPUTE SPREAD

	Omega::spread(&(outputs[OUTPUT_CV]),
	/**/ channels, phase, shape, curve, curve_order,
	/**/ offset, offset + scale);
}

Model* modelOmega3 = createModel<Omega3, Omega3Widget>("Biset-Omega3");
