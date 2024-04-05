
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
	configParam(PARAM_PHASE, -1, 1, 0, "Phase", "%", 0, 100);
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
	from = params[PARAM_FROM].getValue()
	/**/ + inputs[INPUT_FROM].getVoltageSum();
	to = params[PARAM_TO].getValue()
	/**/ + inputs[INPUT_TO].getVoltageSum();

	/// [2] COMPUTE SPREAD

	Omega::spread(&(outputs[OUTPUT_CV]),
	/**/ channels, phase, shape, curve, curve_order,
	/**/ from, to);
}

Model* modelOmega6 = createModel<Omega6, Omega6Widget>("Biset-Omega6");
