
#include "TrackerControl.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerControl::TrackerControl() {
	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configInput(INPUT_RUN, "Trigger / gate run");
	configSwitch(PARAM_RUN_MODE, 0, 1, 0, "Run mode",
	/**/ {"Pattern", "Song"});
	configSwitch(PARAM_RUN_GATE_MODE, 0, 1, 0, "Run gate mode",
	/**/ {"Trigger", "Gate"});

	configInput(INPUT_CLOCK, "Clock");
	configSwitch(PARAM_CLOCK_MODE, 0, 3, 0, "Clock mode",
	/**/ {"24", "48", "96", "Phase"});

	configInput(INPUT_PATTERN_NEXT, "Trigger next pattern");
	configInput(INPUT_PATTERN_PREV, "Trigger previous pattern");
	configInput(INPUT_PATTERN_RAND, "Trigger random pattern");

	this->trigger_run.reset();
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

TrackerControl::~TrackerControl() {
	if (g_module && g_module->control == this)
		g_module->control = NULL;
}

void TrackerControl::process(const ProcessArgs& args) {
	int		p_clock_mode;
	bool	p_run_mode;
	bool	p_gate_mode;
	float	phase;
	int		play_mode;
	int		ppqn;

	if (g_module == NULL || g_timeline == NULL)
		return;
	if (g_module->control == NULL)
		g_module->control = this;
	if (g_module->control != this)
		return;

	/// [1] GET PARAMETERS
	p_run_mode = params[PARAM_RUN_MODE].getValue();
	if (p_run_mode == TCONTROL_RUN_PATTERN)
		play_mode = TIMELINE_MODE_PLAY_PATTERN_SOLO;
	else
		play_mode = TIMELINE_MODE_PLAY_SONG;
	p_gate_mode = params[PARAM_RUN_GATE_MODE].getValue();
	p_clock_mode = params[PARAM_CLOCK_MODE].getValue();
	if (p_clock_mode == TCONTROL_CLOCK_24)
		ppqn = 24;
	else if (p_clock_mode == TCONTROL_CLOCK_48)
		ppqn = 48;
	else if (p_clock_mode == TCONTROL_CLOCK_96)
		ppqn = 96;
	else
		ppqn = 0;

	/// [2] HANDLE RUN MODE
	//// RUN GATE MODE
	if (p_gate_mode == TCONTROL_RUN_GATE) {
		/// RUN ON
		if (inputs[INPUT_RUN].getVoltage() >= 5.0) {
			if (g_timeline->play != play_mode)
				this->play(play_mode);
		/// RUN OFF
		} else {
			if (g_timeline->play != TIMELINE_MODE_STOP)
				this->stop();
		}
	//// RUN TRIGGER MODE
	} else {
		/// ON TRIGGER
		if (this->trigger_run.process(inputs[INPUT_RUN].getVoltage())) {
			/// RUN ON
			if (g_timeline->play != play_mode)
				this->play(play_mode);
			/// RUN ON
			else
				this->stop();
		}
	}

	/// [3] HANDLE RUN CLOCK
	if (g_timeline->play != TIMELINE_MODE_STOP && g_editor->pattern) {
		if (inputs[INPUT_CLOCK].isConnected()) {
			this->clock_master = true;
			/// CLOCK MODE PHASE
			if (p_clock_mode == TCONTROL_CLOCK_PHASE) {
				phase = inputs[INPUT_CLOCK].getVoltage() / 10.0;
				if (phase > 0.999)
					phase = 0.999;
				if (phase < 0.0)
					phase = 0.0;
				/// RUN PATTERN
				if (p_run_mode == TCONTROL_RUN_PATTERN) {
					g_timeline->clock.beat =
					/**/ phase * g_editor->pattern->beat_count;
					g_timeline->clock.phase =
					/**/ fmod(phase * g_editor->pattern->beat_count, 1.0);
				/// RUN SONG
				} else {
				}
			/// CLOCK MODE TRIGGER
			} else {
				this->clock_between_count += 1;
				/// ON TRIGGER
				if (this->trigger_clock.process(inputs[INPUT_CLOCK].getVoltage())) {
					/// COMPUTE CLOCK RATE
					this->clock_between = this->clock_between_count;
					this->clock_between_count = 0;
					/// INCREASE CLOCK
					this->clock_count += 1;
					if (this->clock_count >= ppqn) {
						g_timeline->clock.beat += 1;
						g_timeline->clock.phase = 0.0;
						this->clock_count = 0;
					}
					this->clock_phase = (float)this->clock_count / (float)ppqn;
				}
				phase = (float)this->clock_between_count / this->clock_between;
				if (phase > 1.0)
					phase = 1.0;
				g_timeline->clock.phase = this->clock_phase + phase / (float)ppqn;
			}
		} else {
			this->clock_master = false;
		}
	}

	/// [4] HANDLE PATTERN SELECTION TRIGGERS
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

void TrackerControl::play(int mode) {
	/// SEND T-STATE SIGNALS
	g_timeline->play_trigger.trigger(0.01);
	if (g_timeline->play != TIMELINE_MODE_STOP)
		g_timeline->stop_trigger.trigger(0.01);
	/// RESET TIMELINE
	g_timeline->stop();
	g_timeline->clock.reset();
	/// PLAY TIMELINE
	g_timeline->play = mode;
}

void TrackerControl::stop(void) {
	g_timeline->stop();
	g_timeline->play = TIMELINE_MODE_STOP;
	g_timeline->stop_trigger.trigger(0.01);
}

Model* modelTrackerControl = createModel<TrackerControl, TrackerControlWidget>("Biset-Tracker-Control");
