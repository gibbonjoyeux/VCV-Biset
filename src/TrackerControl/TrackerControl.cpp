
#include "TrackerControl.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerControl::TrackerControl() {
	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configInput(INPUT_PLAY_SONG, "Trigger play song");
	configInput(INPUT_PLAY_PATTERN, "Trigger play pattern");
	configInput(INPUT_CLOCK, "Clock");
	configInput(INPUT_PATTERN_NEXT, "Trigger next pattern");
	configInput(INPUT_PATTERN_PREV, "Trigger previous pattern");
	configInput(INPUT_PATTERN_RAND, "Trigger random pattern");

	this->trigger_play_song.reset();
	this->trigger_play_pattern.reset();
	this->trigger_clock.reset();
	this->trigger_pattern_next.reset();
	this->trigger_pattern_prev.reset();
	this->trigger_pattern_rand.reset();

	this->clock_master = false;
	this->clock_phase = 0.0;
	this->clock_between = 1;
	this->clock_between_count = 0;
	this->clock_count = 0;
}

void TrackerControl::process(const ProcessArgs& args) {

	if (g_module == NULL || g_timeline == NULL)
		return;
	if (g_module->control == NULL)
		g_module->control = this;
	if (g_module->control != this)
		return;

	/// [1] HANDLE PLAY TRIGGERS
	if (this->trigger_play_song.process(inputs[INPUT_PLAY_SONG].getVoltage())) {
		/// SEND T-STATE SIGNALS
		g_timeline->play_trigger.trigger(0.01);
		if (g_timeline->play != TIMELINE_MODE_STOP)
			g_timeline->stop_trigger.trigger(0.01);
		/// RESET TIMELINE
		g_timeline->stop();
		g_timeline->clock.reset();
		/// PLAY TIMELINE
		g_timeline->play = TIMELINE_MODE_PLAY_SONG;
	}
	if (this->trigger_play_pattern.process(inputs[INPUT_PLAY_PATTERN].getVoltage())) {
		if (g_timeline->pattern_count > 0) {
			/// SEND T-STATE SIGNALS
			g_timeline->play_trigger.trigger(0.01);
			if (g_timeline->play != TIMELINE_MODE_STOP)
				g_timeline->stop_trigger.trigger(0.01);
			/// RESET TIMELINE
			g_timeline->stop();
			g_timeline->clock.reset();
			/// PLAY TIMELINE
			g_timeline->play = TIMELINE_MODE_PLAY_PATTERN_SOLO;
		}
	}

	/// [2] HANDLE CLOCK TRIGGER
	if (inputs[INPUT_CLOCK].isConnected()) {

		this->clock_master = true;
		this->clock_between_count += 1;

		if (this->trigger_clock.process(inputs[INPUT_CLOCK].getVoltage())) {

			/// COMPUTE CLOCK RATE
			this->clock_between = this->clock_between_count;
			this->clock_between_count = 0;

			/// INCREASE CLOCK
			this->clock_count += 1;
			if (this->clock_count >= 24) {
				g_timeline->clock.beat += 1;
				g_timeline->clock.phase = 0.0;
				this->clock_count = 0;
			}
			this->clock_phase = (float)this->clock_count / 24.0;

			/// PPQN 24 / 48 / 96 / 192
			//g_timeline->clock.process(1.0 / 24.0);

		}

		g_timeline->clock.phase = this->clock_phase
		/**/ + ((float)this->clock_between_count / this->clock_between) / 24.0;

	} else {
		this->clock_master = false;
	}

	/// [3] HANDLE PATTERN SELECTION TRIGGERS
	if (this->trigger_pattern_next.process(inputs[INPUT_PATTERN_NEXT].getVoltage())) {
		if (g_timeline->pattern_count > 0) {
			if (g_editor->pattern_id < g_timeline->pattern_count - 1)
				g_editor->pattern_id += 1;
			else
				g_editor->pattern_id = 0;
			g_editor->pattern = &(g_timeline->patterns[g_editor->pattern_id]);
		}
	}
	if (this->trigger_pattern_prev.process(inputs[INPUT_PATTERN_PREV].getVoltage())) {
		if (g_timeline->pattern_count > 0) {
			if (g_editor->pattern_id > 0)
				g_editor->pattern_id -= 1;
			else
				g_editor->pattern_id = g_timeline->pattern_count - 1;
			g_editor->pattern = &(g_timeline->patterns[g_editor->pattern_id]);
		}
	}
	if (this->trigger_pattern_rand.process(inputs[INPUT_PATTERN_RAND].getVoltage())) {
		if (g_timeline->pattern_count > 0) {
			g_editor->pattern_id = random::uniform() * g_timeline->pattern_count;
			g_editor->pattern = &(g_timeline->patterns[g_editor->pattern_id]);
		}
	}

}

Model* modelTrackerControl = createModel<TrackerControl, TrackerControlWidget>("Biset-Tracker-Control");
