
#include "TrackerState.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerState::TrackerState() {
	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configOutput(OUTPUT_GATE, "Play gate");
	configOutput(OUTPUT_PLAY, "Play trigger");
	configOutput(OUTPUT_STOP, "Stop trigger");
	configOutput(OUTPUT_PLAY_STOP, "Play + stop trigger");
}

void TrackerState::process(const ProcessArgs& args) {
	bool	trig_play;
	bool	trig_stop;
	bool	trig_both;

	if (g_module == NULL || g_timeline == NULL)
		return;

	trig_play = g_timeline->play_trigger.remaining > 0;
	trig_stop = g_timeline->stop_trigger.remaining > 0;
	trig_both = trig_play || trig_stop;
	this->outputs[OUTPUT_GATE].setVoltage(
	/**/ (g_timeline->play != TIMELINE_MODE_STOP) ? 10.0 : 0.0);
	this->outputs[OUTPUT_PLAY].setVoltage(trig_play ? 10.0 : 0.0);
	this->outputs[OUTPUT_STOP].setVoltage(trig_stop ? 10.0 : 0.0);
	this->outputs[OUTPUT_PLAY_STOP].setVoltage(trig_both ? 10.0 : 0.0);
}

Model* modelTrackerState = createModel<TrackerState, TrackerStateWidget>("Biset-Tracker-State");
