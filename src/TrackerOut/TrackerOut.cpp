
#include "TrackerOut.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerOut::TrackerOut() {
	int			i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);
	configParam(PARAM_SYNTH, 0, 63, 0, "Synth")->snapEnabled = true;
	configOutput(OUTPUT_PITCH, "Pitch");
	configOutput(OUTPUT_GATE, "Gate");
	configOutput(OUTPUT_VELOCITY, "Velocity");
	configOutput(OUTPUT_PANNING, "Panning");
	for (i = 0; i < 8; ++i)
		configOutput(OUTPUT_CV + i, string::f("CV %d", i + 1));
}

void TrackerOut::process(const ProcessArgs& args) {
	Synth		*synth;
	int			i;

	synth = &(g_timeline.synths[(int)this->params[PARAM_SYNTH].getValue()]);
	this->outputs[OUTPUT_PITCH].setChannels(synth->channel_count);
	this->outputs[OUTPUT_GATE].setChannels(synth->channel_count);
	this->outputs[OUTPUT_VELOCITY].setChannels(synth->channel_count);
	this->outputs[OUTPUT_PANNING].setChannels(synth->channel_count);
	/// SET OUTPUT SYNTH
	for (i = 0; i < synth->channel_count; ++i) {
		this->outputs[OUTPUT_PITCH].setVoltage(synth->out_synth[i * 4 + 0], i);
		this->outputs[OUTPUT_GATE].setVoltage(synth->out_synth[i * 4 + 1], i);
		this->outputs[OUTPUT_VELOCITY].setVoltage(synth->out_synth[i * 4 + 2], i);
		this->outputs[OUTPUT_PANNING].setVoltage(synth->out_synth[i * 4 + 3], i);
	}
	/// SET OUTPUT CV
	for (i = 0; i < 8; ++i)
		this->outputs[OUTPUT_CV + i].setVoltage(synth->out_cv[i]);
}

Model* modelTrackerOut = createModel<TrackerOut, TrackerOutWidget>("TrackerOut");
