
#include "TrackerClock.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerClock::TrackerClock() {
	int		i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	this->phase_play = 0;
	this->trigger_restart.reset();
	for (i = 0; i < 4; ++i) {
		this->count[i] = 0;
		configParam(PARAM_FREQ + i, -96, 96, 0, "Frequency")->snapEnabled = true;
		configParam(PARAM_PHASE + i, 0, 1, 0, "Phase");
		configParam(PARAM_PW + i, 0, 1, 0.5, "Pulse Width");
		configParam(PARAM_SWING + i, -1, 1, 0, "Swing");
	}
	configParam(PARAM_MODE, 0, 1, 0)->snapEnabled = true;
}

void TrackerClock::process(const ProcessArgs& args) {
	int		knob_freq;
	float	knob_phase;
	float	knob_pw;
	float	knob_swing;
	int		divider;
	int		multiplier;
	float	phase_main;
	float	phase;
	bool	trigger;
	bool	mode;
	float	out;
	int		i;

	if (g_module == NULL || g_timeline == NULL)
		return;

	mode = this->params[PARAM_MODE].getValue();

	/// [1] CHECK GLOBAL PLAY TRIGGER (RESET)
	if (this->trigger_restart.process(g_timeline->play_trigger.remaining > 0.0)) {
		for (i = 0; i < 4; ++i)
			this->count[i] = 0;
		this->phase_play = g_timeline->clock.phase;
	}

	/// [2] CHECK GLOBAL CLOCK TRIGGER
	trigger = (g_timeline->clock.phase < this->phase_play);
	this->phase_play = g_timeline->clock.phase;

	//// TIMELINE STOP
	if (g_timeline->play == TIMELINE_MODE_STOP) {
		for (i = 0; i < 4; ++i)
			this->outputs[OUTPUT_CLOCK + i].setVoltage(0.0);
		return;
	}
	//// TIMELINE PLAY
	for (i = 0; i < 4; ++i) {
		
		knob_freq = this->params[PARAM_FREQ + i].getValue();
		knob_phase = this->params[PARAM_PHASE + i].getValue();
		knob_pw = this->params[PARAM_PW + i].getValue();
		knob_swing = this->params[PARAM_SWING + i].getValue();

		/// [3] COMPUTE PHASE SWING
		knob_swing = (knob_swing * 0.9) * 0.5 + 0.5;
		if (this->phase_play <= knob_swing) {
			phase_main = (this->phase_play / knob_swing) * 0.5;
		} else {
			phase_main = 0.5
			/**/ + ((this->phase_play - knob_swing) / (1.0 - knob_swing)) * 0.5;
		}

		/// [4] COMPUTE CLOCK DIV / MULT
		//// CLOCK MULT
		if (knob_freq >= -1.0) {
			/// COMPUTE FREQ
			multiplier = knob_freq;
			if (multiplier < 1)
				multiplier = 1;
			/// COMPUTE PHASE
			phase = phase_main * (float)multiplier + knob_phase;
			phase = phase - (int)phase;
			/// COMPUTE TRIGGER
			out = (phase < knob_pw);
		//// CLOCK DIV
		} else {
			/// COMPUTE FREQ
			divider = -knob_freq;
			/// COMPUTE PHASE
			if (trigger)
				this->count[i] += 1;
			/// COMPUTE PHASE
			//// MODE FIXED (RESTART ON LOOP)
			if (mode == TCLOCK_MODE_FIXED) {
				phase = ((float)g_timeline->clock.beat + phase_main)
				/**/ / (float)divider + knob_phase;
			//// MODE LOOP (KEEP GOING ON LOOP)
			} else {
				// TODO: Fix double increment phase
				phase = ((float)this->count[i] + phase_main)
				/**/ / (float)divider + knob_phase;
			}
			phase = phase - (int)phase;
			/// COMPUTE TRIGGER
			out = (phase < knob_pw);
		}
		
		/// [5] OUTPUT TRIGGERS
		this->outputs[OUTPUT_CLOCK + i].setVoltage(out * 10.0);
	}
}

Model* modelTrackerClock = createModel<TrackerClock, TrackerClockWidget>("Biset-Tracker-Clock");
