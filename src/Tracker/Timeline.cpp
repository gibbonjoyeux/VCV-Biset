
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
		//this->pattern_source[i] = NULL;
		//this->pattern_cell[i] = NULL;
		//this->pattern_start[i] = 0;
		//this->pattern_reader[i].reset();
		this->pattern_it[i] = this->timeline[i].begin();
		this->pattern_it_end[i] = this->timeline[i].end();
		this->pattern_reader[i].reset();
		this->pattern_state[i] = false;
	}
	/// [3] INIT CLOCK
	this->clock.reset();
	/// [4] INIT TIMELINE
	this->play = TIMELINE_MODE_STOP;
	/// [5] INIT SAVE BUFFER
	this->save_buffer = NULL;
	this->save_length = 0;

	debug = 0;
	debug_2 = 0;
	debug_str[0] = 0;
}

void Timeline::process(i64 frame, float dt_sec, float dt_beat) {
	//PatternSource*				pattern;
	Clock							clock_relative;
	list<PatternInstance>::iterator	it;
	int								rate_divider;
	int								count;
	int								i;

	/// [1] UPDATE CLOCK
	if (g_timeline.play != TIMELINE_MODE_STOP)
		this->clock.process(dt_beat);
	//// MODE PLAY SONG
	//if (g_timeline.play == TIMELINE_MODE_PLAY_SONG) {
	//	if (this->clock.beat >= this->beat_count) {
	//		/// RESET CLOCK
	//		this->clock.beat = 0;
	//		/// RESET RUNNING PATTERNS
	//		this->stop();
	//	}
	////// MODE PLAY PATTERN
	//} else if (g_timeline.play == TIMELINE_MODE_PLAY_PATTERN) {
	//	if (this->pattern_source[0]
	//	&& this->clock.beat >= this->pattern_source[0]->beat_count) {
	//		/// RESET CLOCK
	//		this->clock.beat = 0;
	//		/// RESET RUNNING PATTERNS
	//		this->stop();
	//	}
	//}

	/// [2] TRUNCATE FRAMERATE
	rate_divider = g_module->params[Tracker::PARAM_RATE].getValue();
	if (frame % rate_divider != 0)
		return;

	/// [3] CHECK THREAD FLAG
	if (g_timeline.thread_flag.test_and_set())
		return;

	/// [4] COMPUTE TEMPERAMENT
	//// BASE PITCH OFFSET (FROM 440Hz)
	this->pitch_base_offset =
	/**/ log2(g_module->params[Tracker::PARAM_PITCH_BASE].getValue() / 440.0);
	//// TEMPERAMENT SCALE
	for (i = 0; i < 12; ++i)
		this->pitch_scale[i] = g_module->params[Tracker::PARAM_TEMPERAMENT + i].getValue();

	/// [5] PLAY
	//// MODE PLAY SONG
	if (g_timeline.play == TIMELINE_MODE_PLAY_SONG) {
		count = 0;
		/// COMPUTE ROWS
		for (i = 0; i < 32; ++i) {
			it = this->pattern_it[i];
			/// ROW PLAY
			if (it != this->timeline[i].end()) {
				if (this->clock.beat >= it->beat) {
					/// PATTERN PLAY
					if (this->clock.beat < it->beat + it->beat_length) {
						this->pattern_state[i] = true;
						/// COMPUTE RELATIVE CLOCK
						clock_relative.beat =
						(it->beat_start + (this->clock.beat - it->beat))
						/**/ % it->source->beat_count;
						clock_relative.phase = this->clock.phase;
						/// COMPUTE PATTERN PROCESS
						this->pattern_reader[i].process(this->synths,
						/**/ it->source, clock_relative);
					/// PATTERN END
					} else {
						if (this->pattern_state[i] == true) {
							it = std::next(it);
							this->pattern_state[i] = false;
							this->pattern_reader[i].stop();
							this->pattern_it[i] = it;
							/// PATTERN NEW
							if (it != this->timeline[i].end()) {
							} else {
								count += 1;
							}
						}
					}
				}
			/// ROW END
			} else {
				count += 1;
			}
		}
		/// ROWS ALL ENDED
		if (count >= 32) {
			/// RESET CLOCK
			this->clock.beat = 0;
			/// RESET RUNNING PATTERNS
			this->stop();
		}
	}

	///// [5] PLAY
	////// MODE PLAY SONG
	//if (g_timeline.play == TIMELINE_MODE_PLAY_SONG) {
	//	/// UPDATE TIMELINE COLS
	//	for (i = 0; i < 12; ++i) {
	//		cell = &(this->timeline[i][clock.beat]);
	//		/// PATTERN CHANGE
	//		if (cell->mode == TIMELINE_CELL_NEW
	//		&& this->pattern_cell[i] != cell) {
	//			if (cell->pattern < 256) {
	//				/// COMPUTE PATTERN CHANGE / TIMING
	//				pattern = &(this->patterns[cell->pattern]);
	//				/// RESET RELATIVE CLOCK
	//				clock_relative.beat = cell->beat % pattern->beat_count;
	//				clock_relative.phase = this->clock.phase;
	//				/// RESET PATTERN
	//				this->pattern_reader[i].stop();
	//				this->pattern_start[i] = clock.beat;
	//				this->pattern_cell[i] = cell;
	//				this->pattern_source[i] = pattern;
	//				/// COMPUTE PATTERN PROCESS
	//				this->pattern_reader[i].process(this->synths,
	//				/**/ this->pattern_source[i], clock_relative,
	//				/**/ &debug, &debug_2, debug_str);
	//			}
	//		/// PATTERN STOP
	//		} else if (cell->mode == TIMELINE_CELL_STOP) {
	//			/// PATTERN SWITCH OFF
	//			if (this->pattern_source[i] != NULL) {
	//				this->pattern_reader[i].stop();
	//				this->pattern_source[i] = NULL;
	//				this->pattern_source[i] = NULL;
	//				this->pattern_cell[i] = NULL;
	//				this->pattern_start[i] = 0;
	//			}
	//		/// PATTERN KEEP
	//		} else {
	//			if (this->pattern_source[i]) {
	//				pattern = this->pattern_source[i];
	//				/// COMPUTE RELATIVE CLOCK
	//				clock_relative.beat =
	//				/**/ (this->clock.beat - this->pattern_start[i]
	//				/**/ + this->pattern_cell[i]->beat) % pattern->beat_count;
	//				clock_relative.phase = this->clock.phase;
	//				/// COMPUTE PATTERN PROCESS
	//				this->pattern_reader[i].process(this->synths,
	//				/**/ this->pattern_source[i], clock_relative,
	//				/**/ &debug, &debug_2, debug_str);
	//			}
	//		}
	//	}
	////// MODE PLAY PATTERN
	//} else if (g_timeline.play == TIMELINE_MODE_PLAY_PATTERN) {
	//	pattern = g_editor.pattern;
	//	/// UPDATE PATTERN ON END
	//	if (this->pattern_source[0] == NULL)
	//		this->pattern_source[0] = pattern;
	//	/// COMPUTE PATTERN
	//	this->pattern_reader[0].process(this->synths,
	//	/**/ this->pattern_source[0], this->clock,
	//	/**/ &debug, &debug_2, debug_str);
	//}

	//// -> ! ! ! BOTTLENECK ! ! !
	//// -> Truncate framerate to run only every 32 / 64 frames
	//// -> Use boolean that defines if a Synth is used or not
	////    (TrackerOut / TrackerDrumOut set the boolean when they pull from the
	////    corresponding synth
	///// [6] UPDATE SYNTHS (WITH TRUNCATED FRAMERATE)
	for (i = 0; i < this->synth_count; ++i)
		synths[i].process(dt_sec * rate_divider, dt_beat * rate_divider);

	/// [7] CLEAR THREAD FLAG
	g_timeline.thread_flag.clear();
}

void Timeline::stop(void) {
	int		i;

	for (i = 0; i < 32; ++i) {
		//if (this->pattern_source[i] != NULL) {
		//	this->pattern_reader[i].stop();
		//	this->pattern_source[i] = NULL;
		//	this->pattern_source[i] = NULL;
		//	//this->pattern_cell[i] = NULL;
		//	this->pattern_start[i] = 0;
		//}
		this->pattern_it[i] = this->timeline[i].begin();
		this->pattern_it_end[i] = this->timeline[i].end();
		this->pattern_reader[i].stop();
		this->pattern_state[i] = false;
	}
}
