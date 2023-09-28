
#include "TrackerState.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerState::TrackerState() {
	int		i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configOutput(OUTPUT_GATE, "Play gate");
	configOutput(OUTPUT_PLAY, "Play trigger");
	configOutput(OUTPUT_STOP, "Stop trigger");
	configOutput(OUTPUT_PLAY_STOP, "Play + stop trigger");

	for (i = 0; i < 400; ++i)
		this->scale[i] = 0.0;
}

void TrackerState::process(const ProcessArgs& args) {
	Synth	*synth;
	float	*scale;
	float	gate;
	bool	trig_play;
	bool	trig_stop;
	bool	trig_both;
	int		rate_divider;
	int		i, j;

	if (g_module == NULL || g_timeline == NULL)
		return;

	rate_divider = g_module->params[Tracker::PARAM_RATE].getValue();
	if (args.frame % rate_divider == 0) {
		for (i = 0; i < g_timeline->synth_count; ++i) {
			synth = &(g_timeline->synths[i]);
			for (j = 0; j < synth->channel_count; ++j) {
				scale = &(this->scale[i * 16 + j]);
				gate = synth->out_synth[j * 4 + 1];
				/// MODE GATE
				if (synth->mode == SYNTH_MODE_GATE) {
					*scale = *scale * 0.99 + gate * 0.01;
				/// MODE TRIGGER + DRUM
				} else {
					if (gate > *scale)
						*scale = gate;
					else
						*scale = *scale * 0.99 + gate * 0.01;
				}
			}
		}
	}

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
