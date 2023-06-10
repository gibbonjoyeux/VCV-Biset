
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Regex::Regex() {
	//int			i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);
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
}

void Regex::process(const ProcessArgs& args) {
}

Model* modelRegex = createModel<Regex, RegexWidget>("Regex");
