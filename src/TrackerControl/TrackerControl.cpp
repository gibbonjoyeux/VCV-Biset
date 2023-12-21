

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
	configInput(INPUT_RESET, "Reset");
	configSwitch(PARAM_CLOCK_MODE, 0, 3, 0, "Clock mode",
	/**/ {"24", "48", "96", "Phase"});

	configInput(INPUT_PATTERN_NEXT, "Trigger next pattern");
	configInput(INPUT_PATTERN_PREV, "Trigger previous pattern");
	configInput(INPUT_PATTERN_RAND, "Trigger random pattern");
	configSwitch(PARAM_PATTERN_NEXT, 0, 1, 0, "Next pattern");
	configSwitch(PARAM_PATTERN_PREV, 0, 1, 0, "Prev pattern");
	configSwitch(PARAM_PATTERN_RAND, 0, 1, 0, "Random pattern");

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
	if (args.frame % 32 == 0) {
		lights[LIGHT_CLOCK_24].setBrightness(ppqn == 24);
		lights[LIGHT_CLOCK_48].setBrightness(ppqn == 48);
		lights[LIGHT_CLOCK_96].setBrightness(ppqn == 96);
		lights[LIGHT_CLOCK_PHASE].setBrightness(ppqn == 0);
		lights[LIGHT_PATTERN].setBrightness(p_run_mode == TCONTROL_RUN_PATTERN);
		lights[LIGHT_SONG].setBrightness(p_run_mode == TCONTROL_RUN_SONG);
		lights[LIGHT_TRIGGER].setBrightness(p_gate_mode == TCONTROL_RUN_TRIGGER);
		lights[LIGHT_GATE].setBrightness(p_gate_mode == TCONTROL_RUN_GATE);
	}

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

	/// [3] HANDLE CLOCK
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
					phase *= g_editor->pattern->beat_count;
					g_timeline->clock.beat = phase;
					g_timeline->clock.phase = phase - (int)phase;
				/// RUN SONG
				} else {
					phase *= this->timeline_length;
					g_timeline->clock.beat = phase;
					g_timeline->clock.phase = phase - (int)phase;
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

	/// [4] HANDLE RESET
	if (this->trigger_reset.process(inputs[INPUT_RESET].getVoltage())) {
		g_timeline->stop();
		g_timeline->clock.reset();
	}

	/// [5] HANDLE PATTERN SELECTION TRIGGERS
	if (this->trigger_pattern_next.process(
	inputs[INPUT_PATTERN_NEXT].getVoltage()
	+ params[PARAM_PATTERN_NEXT].getValue())) {
		if (g_timeline->pattern_count > 0) {
			if (g_editor->pattern_id < g_timeline->pattern_count - 1)
				g_editor->pattern_id += 1;
			else
				g_editor->pattern_id = 0;
			g_editor->pattern = &(g_timeline->patterns[g_editor->pattern_id]);
		}
	}
	if (this->trigger_pattern_prev.process(
	inputs[INPUT_PATTERN_PREV].getVoltage()
	+ params[PARAM_PATTERN_PREV].getValue())) {
		if (g_timeline->pattern_count > 0) {
			if (g_editor->pattern_id > 0)
				g_editor->pattern_id -= 1;
			else
				g_editor->pattern_id = g_timeline->pattern_count - 1;
			g_editor->pattern = &(g_timeline->patterns[g_editor->pattern_id]);
		}
	}
	if (this->trigger_pattern_rand.process(
	inputs[INPUT_PATTERN_RAND].getVoltage()
	+ params[PARAM_PATTERN_RAND].getValue())) {
		if (g_timeline->pattern_count > 0) {
			g_editor->pattern_id = random::uniform() * g_timeline->pattern_count;
			g_editor->pattern = &(g_timeline->patterns[g_editor->pattern_id]);
		}
	}

}

void TrackerControl::play(int mode) {
	list<PatternInstance>::iterator	it, it_end;
	int								instance_end;
	int								i;

	/// SEND T-STATE SIGNALS
	g_timeline->play_trigger.trigger(0.01);
	if (g_timeline->play != TIMELINE_MODE_STOP)
		g_timeline->stop_trigger.trigger(0.01);
	/// RESET TIMELINE
	g_timeline->stop();
	g_timeline->clock.reset();
	this->reset();
	/// PLAY TIMELINE
	g_timeline->play = mode;

	/// COMPUTE TIMELINE LENGTH
	this->timeline_length = 0;
	for (i = 0; i < 32; ++i) {
		it = g_timeline->timeline[i].begin();
		it_end = g_timeline->timeline[i].end();
		while (it != it_end) {
			instance_end = it->beat + it->beat_length;
			if (instance_end > this->timeline_length)
				this->timeline_length = instance_end;
			it = std::next(it);
		}
	}
}

void TrackerControl::stop(void) {
	g_timeline->stop();
	g_timeline->play = TIMELINE_MODE_STOP;
	g_timeline->stop_trigger.trigger(0.01);
}

void TrackerControl::reset(void) {
	this->clock_phase = 0.0;
	this->clock_between_count = 0;
	this->clock_count = 0;
}

Model* modelTrackerControl = createModel<TrackerControl, TrackerControlWidget>("Biset-Tracker-Control");
