
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// Timeline stores every PatternSource & Synth.
// Timeline stores the main clock;
// Timeline stores a PatternInstance array (one per track / row)
// Timeline stores the timeline cells array (2D).
//  1D: Track / Row
//  2D: Cell (TimelineCell)
// There are 32 tracks / rows and their length is based on the beat count.
// Each cell / line of the tracks represent one beat.

Timeline::Timeline() {
	int						i;

	this->thread_flag.clear();
	/// [1] INIT ROWS
	for (i = 0; i < 12; ++i) {
		this->pattern_source[i] = NULL;
		this->pattern_cell[i] = NULL;
		this->pattern_start[i] = 0;
		this->pattern_instance[i].reset();
	}
	/// [2] INIT SYNTHS
	for (i = 0; i < 64; ++i) {
		this->synths[i].init(i, 1);
	}
	/// [3] INIT CLOCK
	this->clock.reset();
	/// [4] INIT TIMELINE
	this->resize(16);
	/// [5] INIT SAVE BUFFER
	this->save_buffer = NULL;
	this->save_length = 0;

	debug = 0;
	debug_2 = 0;
	debug_str[0] = 0;
}

void Timeline::process(i64 frame, float dt_sec, float dt_beat) {
	Clock					clock_relative;
	PatternSource*			pattern;
	TimelineCell*			cell;
	int						i;

	/// [1] UPDATE CLOCK
	this->clock.process(dt_beat);
	if (this->clock.beat >= this->beat_count) {
		this->clock.beat = 0;
		/// SWITCH OFF RUNNING PATTERNS
		for (i = 0; i < 12; ++i) {
			if (this->pattern_source[i] != NULL) {
				this->pattern_instance[i].stop();
				this->pattern_source[i] = NULL;
				this->pattern_source[i] = NULL;
				this->pattern_cell[i] = NULL;
				this->pattern_start[i] = 0;
			}
		}
	}

	//// TRUNCATE FRAMERATE
	//if (frame % 64 != 0)
	//	return;

	/// [2] CHECK THREAD FLAG
	if (g_timeline.thread_flag.test_and_set())
		return;

	/// [3] UPDATE TIMELINE ROWS
	for (i = 0; i < 12; ++i) {
		cell = &(this->timeline[i][clock.beat]);
		/// PATTERN CHANGE
		if (cell->mode == TIMELINE_CELL_NEW
		&& this->pattern_cell[i] != cell) {
			if (cell->pattern < 256) {
				/// COMPUTE PATTERN CHANGE / TIMING
				pattern = &(this->patterns[cell->pattern]);
				/// RESET RELATIVE CLOCK
				clock_relative.beat = cell->beat % pattern->beat_count;
				clock_relative.phase = this->clock.phase;
				/// RESET PATTERN
				this->pattern_instance[i].stop();
				this->pattern_start[i] = clock.beat;
				this->pattern_cell[i] = cell;
				this->pattern_source[i] = pattern;
				/// COMPUTE PATTERN PROCESS
				this->pattern_instance[i].process(this->synths,
				/**/ this->pattern_source[i], clock_relative,
				/**/ &debug, &debug_2, debug_str);
			}
		/// PATTERN STOP
		} else if (cell->mode == TIMELINE_CELL_STOP) {
			/// PATTERN SWITCH OFF
			if (this->pattern_source[i] != NULL) {
				this->pattern_instance[i].stop();
				this->pattern_source[i] = NULL;
				this->pattern_source[i] = NULL;
				this->pattern_cell[i] = NULL;
				this->pattern_start[i] = 0;
			}
		/// PATTERN KEEP
		} else {
			if (this->pattern_source[i]) {
				pattern = this->pattern_source[i];
				/// COMPUTE RELATIVE CLOCK
				clock_relative.beat =
				/**/ (this->clock.beat - this->pattern_start[i]
				/**/ + this->pattern_cell[i]->beat) % pattern->beat_count;
				clock_relative.phase = this->clock.phase;
				/// COMPUTE PATTERN PROCESS
				this->pattern_instance[i].process(this->synths,
				/**/ this->pattern_source[i], clock_relative,
				/**/ &debug, &debug_2, debug_str);
			}
		}
	}

	// -> ! ! ! BOTTLENECK ! ! !
	// -> Truncate framerate to run only every 32 / 64 frames
	// -> Use boolean that defines if a Synth is used or not
	//    (does a TrackerOut / TrackerDrumOut uses it, event on change / add /
	//    remove / ...)
	/// [4] UPDATE SYNTHS (WITH TRUNCATED FRAMERATE)
	//for (i = 0; i < 64; ++i)
	//	synths[i].process(dt_sec * 64.0, dt_beat * 64.0);
	for (i = 0; i < 64; ++i)
		synths[i].process(dt_sec, dt_beat);

	/// [5] CLEAR THREAD FLAG
	g_timeline.thread_flag.clear();
}

void Timeline::resize(int beat_count) {
	/// [1] RESIZE TIMELINE
	this->beat_count = beat_count;
	this->timeline.allocate(12, this->beat_count);
}
