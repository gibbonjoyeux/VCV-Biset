
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Regex::Regex() {
	ParamQuantityOptions	*param;
	int						i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);
	/// CONFIG INPUT
	configInput(INPUT_MASTER, "Master clock");
	/// COMFIG PARAMS
	for (i = 0; i < 8; ++i) {
		param = configParam<ParamQuantityOptions>(PARAM_MODE + i, 0, 3, 0, string::f("Mode %d", i + 1));
		param->snapEnabled = true;
		param->options = {"Clock", "Rachet", "Pitch", "Octave"};
	}
	/// CONFIG INPUTS / OUTPUTS
	for (i = 0; i < 8; ++i) {
		configInput(INPUT_EXP + i, string::f("%d", i + 1));
		configOutput(OUTPUT_EXP + i, string::f("%d", i + 1));
		this->sequences[i].in = &(this->inputs[INPUT_EXP + i]);
		this->sequences[i].out = &(this->outputs[OUTPUT_EXP + i]);
	}

	this->clock.reset();
}

void Regex::process(const ProcessArgs& args) {
	float	clock_master;
	int		mode;
	int		i;

	clock_master = this->clock.process(this->inputs[INPUT_MASTER].getVoltage());
	for (i = 0; i < 8; ++i) {
		/// UPDATE SEQUENCES MODES
		mode = this->params[PARAM_MODE + i].getValue();
		if (args.frame % 64 != 0) {
			if (mode != this->sequences[i].mode) {
				this->sequences[i].mode = mode;
				this->sequences[i].reset();
				this->sequences[i].display->check_syntax();
			}
		}
		/// PROCESS SEQUENCE
		this->sequences[i].process(args.sampleTime, clock_master);
	}
}

Model* modelRegex = createModel<Regex, RegexWidget>("Regex");
