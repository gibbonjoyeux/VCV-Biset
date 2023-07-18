
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

	this->phase_global = 0;
	for (i = 0; i < 4; ++i) {
		this->count[i] = 0;
		this->phase[i] = 0.0;
		this->trigger[i].reset();
		configParam(PARAM_FREQ + i, -64, 64, 0, "Frequency")->snapEnabled = true;
	}
}

void TrackerClock::process(const ProcessArgs& args) {
	int		knob_freq;
	int		divider;
	int		multiplier;
	float	phase;
	bool	trigger;
	int		i;

	/// [1] COMPUTE MAIN TRIGGER
	trigger = (g_timeline.clock_phase.time < this->phase_global);
	this->phase_global = g_timeline.clock_phase.time;

	for (i = 0; i < 4; ++i) {
		
		if (g_timeline.clock_phase_step == 0) {
			this->count[i] = 0;
			this->phase[i] = 0.0;
			this->trigger[i].reset();
			continue;
		}

		knob_freq = this->params[PARAM_FREQ + i].getValue();

		/// [2] COMPUTE CLOCK DIV / MULT
		//// CLOCK MULT
		if (knob_freq >= -1.0) {
			/// COMPUTE FREQ
			multiplier = knob_freq;
			if (multiplier < 1)
				multiplier = 1;
			/// COMPUTE PHASE
			phase = g_timeline.clock_phase.time * (float)multiplier;
			phase = phase - (int)phase;
			/// COMPUTE TRIGGER
			if (phase < this->phase[i])
				this->trigger[i].trigger();
			this->phase[i] = phase;
		//// CLOCK DIV
		} else {
			divider = -knob_freq;
			if (trigger) {
				this->count[i] += 1;
				if (this->count[i] >= divider) {
					this->trigger[i].trigger();
					this->count[i] = 0;
				}
			}
		}
		
		/// [3] OUTPUT TRIGGERS
		this->outputs[OUTPUT_CLOCK + i].setVoltage(
		/**/ this->trigger[i].process(args.sampleTime) * 10.0);
	}
}

Model* modelTrackerClock = createModel<TrackerClock, TrackerClockWidget>("Biset-Tracker-Clock");
