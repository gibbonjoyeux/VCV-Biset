
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
	}
	configParam(PARAM_MODE, 0, 1, 0)->snapEnabled = true;
}

void TrackerClock::process(const ProcessArgs& args) {
	int		knob_freq;
	float	knob_phase;
	float	knob_pw;
	int		divider;
	int		multiplier;
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
		for (i = 0; i < 4; ++i) {
			this->count[i] = 0;
		}
		this->phase_play = g_timeline->clock.phase;
	}

	/// [2] CHECK GLOBAL CLOCK TRIGGER
	trigger = (g_timeline->clock.phase < this->phase_play);
	this->phase_play = g_timeline->clock.phase;

	for (i = 0; i < 4; ++i) {
		
		knob_freq = this->params[PARAM_FREQ + i].getValue();
		knob_phase = this->params[PARAM_PHASE + i].getValue();
		knob_pw = this->params[PARAM_PW + i].getValue();

		/// [3] COMPUTE CLOCK DIV / MULT
		//// CLOCK MULT
		if (knob_freq >= -1.0) {
			/// COMPUTE FREQ
			multiplier = knob_freq;
			if (multiplier < 1)
				multiplier = 1;
			/// COMPUTE PHASE
			phase = g_timeline->clock.phase * (float)multiplier + knob_phase;
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
				phase = ((float)g_timeline->clock.beat + g_timeline->clock.phase)
				/**/ / (float)divider + knob_phase;
			//// MODE LOOP (KEEP GOING ON LOOP)
			} else {
				// TODO: Fix double increment phase
				phase = ((float)this->count[i] + g_timeline->clock.phase)
				/**/ / (float)divider + knob_phase;
			}
			phase = phase - (int)phase;
			/// COMPUTE TRIGGER
			out = (phase < knob_pw);
		}
		
		/// [4] OUTPUT TRIGGERS
		this->outputs[OUTPUT_CLOCK + i].setVoltage(out * 10.0);
	}
}

Model* modelTrackerClock = createModel<TrackerClock, TrackerClockWidget>("Biset-Tracker-Clock");
