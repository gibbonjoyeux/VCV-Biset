
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// Timeline stores every PatternSource & Synth.
// Timeline stores the main clock;
// Timeline stores a TimelineRow array (one per track / col)
// Timeline stores the timeline cells array (2D).
//  1D: Track / Row
//  2D: Cell (TimelineCell)
// There are 32 tracks / cols and their length is based on the beat count.
// Each cell / line of the tracks represent one beat.

Timeline::Timeline() {
	int						i;

	this->thread_flag.clear();
	/// [1] INIT PATTERNS & SYNTHS
	this->pattern_count = 0;
	this->synth_count = 0;
	/// [2] INIT TIMELINE READING
	for (i = 0; i < 32; ++i) {
		this->pattern_it[i] = this->timeline[i].begin();
		this->pattern_it_end[i] = this->timeline[i].end();
		this->pattern_reader[i].reset();
		this->pattern_state[i] = false;
	}
	/// [3] INIT CLOCK
	this->clock.reset();
	/// [4] INIT TIMELINE
	this->play = TIMELINE_MODE_STOP;
	this->play_trigger.reset();
	this->stop_trigger.reset();
	this->timeline_length = 0;
	/// [5] INIT SAVE BUFFER
	this->save_buffer = NULL;
	this->save_length = 0;

	//debug_str[0] = 0;
}

void Timeline::process(i64 frame, float dt_sec, float dt_beat) {
	//PatternSource*				pattern;
	Clock							clock_relative;
	list<PatternInstance>::iterator	it, it_cur, it_end;
	int								rate_divider;
	int								count;
	int								i;

	/// [1] UPDATE CLOCK
	if (g_timeline->play != TIMELINE_MODE_STOP)
		if (!g_module->control || g_module->control->clock_master == false)
			this->clock.process(dt_beat);

	/// [2] TRUNCATE FRAMERATE
	rate_divider = g_module->params[Tracker::PARAM_RATE].getValue();
	if (frame % rate_divider != 0)
		return;

	/// [3] CHECK THREAD FLAG
	if (g_timeline->thread_flag.test_and_set())
		return;

	/// [4] COMPUTE TUNING
	//// BASE PITCH OFFSET (FROM 440Hz)
	this->pitch_base_offset =
	/**/ log2(g_module->params[Tracker::PARAM_PITCH_BASE].getValue() / 440.0);
	//// TUNING SCALE
	for (i = 0; i < 12; ++i) {
		this->pitch_scale[i] =
		/**/ g_module->params[Tracker::PARAM_TUNING + i].getValue() / 100.0;
	}

	/// [5] PLAY
	//// MODE PLAY SONG & PLAY PATTERN
	if (g_timeline->play == TIMELINE_MODE_PLAY_SONG
	|| g_timeline->play == TIMELINE_MODE_PLAY_PATTERN) {
		count = 0;
		/// COMPUTE ROWS
		for (i = 0; i < 32; ++i) {
			it = this->pattern_it[i];
			it_end = this->timeline[i].end();
			it_cur = it_end;
			/// FIND PLAYING INSTANCE
			while (it != it_end) {
				/// CURRENT INSTANCE
				if (this->clock.beat >= it->beat
				&& this->clock.beat < it->beat + it->beat_length) {
					it_cur = it;
				/// FINISHED INSTANCE
				} else if (this->pattern_state[i] == true
				&& this->pattern_it[i] == it) {
					this->pattern_state[i] = false;
					this->pattern_reader[i].stop();
					if (g_timeline->play == TIMELINE_MODE_PLAY_PATTERN) {
						if (&(*it) == this->pattern_instance) {
							if (g_editor->instance)
								this->pattern_instance = g_editor->instance;
							this->clock.reset();
							this->clock.beat = this->pattern_instance->beat;
							this->stop();
						}
					}
				}
				it = std::next(it);
			}
			/// PLAY INSTANCE (IF FOUND)
			if (it_cur != it_end) {
				if (this->clock.beat >= it_cur->beat) {
					if (it_cur->muted == false) {
						/// ACTIVATE INSTANCE
						this->pattern_state[i] = true;
						this->pattern_it[i] = it_cur;
						/// COMPUTE RELATIVE CLOCK
						clock_relative.beat =
						(it_cur->beat_start + (this->clock.beat - it_cur->beat))
						/**/ % it_cur->source->beat_count;
						clock_relative.phase = this->clock.phase;
						/// COMPUTE PATTERN PROCESS
						this->pattern_reader[i].process(this->synths,
						/**/ it_cur->source, clock_relative);
					}
				}
			} else {
				count += 1;
			}
		}
		/// ROWS ALL ENDED
		if (g_timeline->play == TIMELINE_MODE_PLAY_SONG
		&& count >= 32) {
			/// RESET CLOCK
			//this->clock.beat = 0;
			this->clock.reset();
			/// RESET RUNNING PATTERNS
			this->stop();
		}
	//// MODE PLAY PATTERN SOLO
	} else if (g_timeline->play == TIMELINE_MODE_PLAY_PATTERN_SOLO) {
		if (g_editor->pattern) {
			if (this->clock.beat >= g_editor->pattern->beat_count) {
				//this->pattern_reader[0].stop();
				this->clock.reset();
			}
			/// COMPUTE PATTERN
			this->pattern_reader[0].process(this->synths,
			/**/ g_editor->pattern, this->clock);
		}
	//// MODE PLAY MATRIX (LIVE)
	} else if (g_timeline->play == TIMELINE_MODE_PLAY_MATRIX) {
	}

	//// -> ! ! ! BOTTLENECK ! ! !
	//// -> Truncate framerate to run only every 32 / 64 frames
	//// -> Use boolean that defines if a Synth is used or not
	////    (TrackerOut / TrackerDrumOut set the boolean when they pull from the
	////    corresponding synth
	///// [6] UPDATE SYNTHS (WITH TRUNCATED FRAMERATE)
	for (i = 0; i < this->synth_count; ++i)
		synths[i].process(dt_sec * rate_divider, dt_beat * rate_divider);
	
	this->play_trigger.process(dt_sec * rate_divider);
	this->stop_trigger.process(dt_sec * rate_divider);

	/// [7] CLEAR THREAD FLAG
	g_timeline->thread_flag.clear();
}

void Timeline::stop(void) {
	int		i;

	/// RESET TIMELINE
	for (i = 0; i < 32; ++i) {
		this->pattern_it[i] = this->timeline[i].begin();
		this->pattern_it_end[i] = this->timeline[i].end();
		this->pattern_reader[i].stop();
		this->pattern_state[i] = false;
	}
}

void Timeline::clear(void) {
	int		i;

	/// REMOVE INSTANCES
	for (i = 0; i < 32; ++i)
		while (this->timeline[i].begin() != this->timeline[i].end())
			this->instance_del(&(*this->timeline[i].begin()));
	/// REMOVE PATTERNS
	while (this->pattern_count > 0)
		this->pattern_del(&(this->patterns[this->pattern_count - 1]));
	/// REMOVE SYNTHS
	while (this->synth_count > 0)
		this->synth_del(&(this->synths[this->synth_count - 1]));
}

void Timeline::compute_length(void) {
	list<PatternInstance>::iterator	it, it_end;
	int								instance_end;
	int								i;

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
