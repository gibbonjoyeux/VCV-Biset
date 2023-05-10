
#include "TrackerDrum.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerDrum::TrackerDrum() {
	int			i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);
	/// CONFIG PARAMETERS
	configParam(PARAM_SYNTH, 0, 63, 0, "Synth")->snapEnabled = true;
	for (i = 0; i < 8; ++i)
		configParam(PARAM_OUT_MIN + i, -10.0, 10.0, 0.0, "Min");
	for (i = 0; i < 8; ++i)
		configParam(PARAM_OUT_MAX + i, -10.0, 10.0, 10.0, "Max");
	/// CONFIG OUTPUTS
	for (i = 0; i < 12; ++i) {
		configOutput(OUTPUT_TRIGGER + i, string::f("Trigger %d", i + 1));
		configOutput(OUTPUT_VELOCITY + i, string::f("Velocity %d", i + 1));
		configOutput(OUTPUT_PANNING + i, string::f("Panning %d", i + 1));
	}
	for (i = 0; i < 8; ++i)
		configOutput(OUTPUT_CV + i, string::f("CV %d", i + 1));
}

void TrackerDrum::process(const ProcessArgs& args) {
	Synth		*synth;
	float		cv;
	float		cv_min, cv_max;
	int			i;

	synth = &(g_timeline.synths[(int)this->params[PARAM_SYNTH].getValue()]);
	/// SET OUTPUT SYNTH
	for (i = 0; i < 12; ++i) {
		this->outputs[OUTPUT_TRIGGER + i].setVoltage(synth->out_synth[i * 4 + 1]);
		this->outputs[OUTPUT_VELOCITY + i].setVoltage(synth->out_synth[i * 4 + 2]);
		this->outputs[OUTPUT_PANNING + i].setVoltage(synth->out_synth[i * 4 + 3]);
	}
	/// SET OUTPUT CV
	for (i = 0; i < 8; ++i) {
		if (this->outputs[OUTPUT_CV + i].isConnected()) {
			/// MAP FROM [0:1] TO [MIN:MAX]
			cv_min = this->params[PARAM_OUT_MIN + i].getValue();
			cv_max = this->params[PARAM_OUT_MAX + i].getValue();
			cv = (synth->out_cv[i] * (cv_max - cv_min)) + cv_min;
			this->outputs[OUTPUT_CV + i].setVoltage(cv);
		}
	}
}

Model* modelTrackerDrum = createModel<TrackerDrum, TrackerDrumWidget>("TrackerDrum");
