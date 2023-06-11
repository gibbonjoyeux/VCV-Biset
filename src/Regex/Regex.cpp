
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Regex::Regex() {
	int			i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);
	/// CONFIG INPUT
	configInput(INPUT_CLOCK, "Clock");
	/// CONFIG PARAMETERS
	//configParam(PARAM_SYNTH, 0, 99, 0, "Synth")->snapEnabled = true;
	//for (i = 0; i < 8; ++i)
	//	configParam(PARAM_OUT_MIN + i, -10.0, 10.0, 0.0, "Min");
	//for (i = 0; i < 8; ++i)
	//	configParam(PARAM_OUT_MAX + i, -10.0, 10.0, 10.0, "Max");
	///// CONFIG OUTPUTS
	//configOutput(OUTPUT_PITCH, "Pitch");
	//configOutput(OUTPUT_GATE, "Gate");
	//configOutput(OUTPUT_VELOCITY, "Velocity");
	//configOutput(OUTPUT_PANNING, "Panning");
	//for (i = 0; i < 8; ++i)
	//	configOutput(OUTPUT_CV + i, string::f("CV %d", i + 1));

	/// CONFIG OUTPUTS
	for (i = 0; i < 4; ++i) {
		configOutput(OUTPUT_GATE + i, string::f("Trigger %d", i + 1));
		configOutput(OUTPUT_PITCH + i, string::f("Pitch %d", i + 1));
		this->sequences[i].out_rythm = &(this->outputs[OUTPUT_GATE + i]);
		this->sequences[i].out_pitch = &(this->outputs[OUTPUT_PITCH + i]);
	}

	this->clock.reset();
}

void Regex::process(const ProcessArgs& args) {
	bool	trigger;
	int		i;

	trigger = this->clock.process(this->inputs[INPUT_CLOCK].getVoltage());

	for (i = 0; i < 4; ++i)
		this->sequences[i].process(args.sampleTime, trigger);
}

Model* modelRegex = createModel<Regex, RegexWidget>("Regex");
