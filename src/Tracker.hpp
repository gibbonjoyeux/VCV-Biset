
#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "plugin.hpp"

typedef struct Clock			Clock;
typedef struct PatternEffect		PatternEffect;
typedef struct PatternCell		PatternCell;
typedef struct PatternSource		PatternSource;
typedef struct SynthVoice		SynthVoice;
typedef struct TimelineRow		TimelineRow;
typedef struct Timeline			Timeline;

struct Clock {
	u32				beat;
	float				phase;

	Clock() {
		this->beat = 0;
		this->phase = 0.0;
	}

	void process(float dt) {
		this->phase += dt;
		while (this->phase >= 1.0) {
			this->beat += 1;
			this->phase -= 1.0;
		}
	}
};

//////////////////////////////////////////////////
/// SYNTH
//////////////////////////////////////////////////

struct SynthVoice {
	bool				active;

	u8				synth;
	u8				synth_channel;
	u8				pitch;
	u8				velocity;

	u8				glide;
	float				glide_cur;
	float				glide_pitch_from;
	float				glide_pitch_to;
	float				glide_velo_from;
	float				glide_velo_to;

	u8				delay;
	float				delay_cur;

	u8				vibrato_amp;
	u8				vibrato_freq;
	float				vibrato_phase;
	u8				tremolo_amp;
	u8				tremolo_freq;
	float				tremolo_phase;

	SynthVoice() {
		this->reset();
	}

	void process() {
	}

	void init(int synth, int channel) {
		this->active = true;
		this->synth = synth;
		this->channel = channel;
	}

	void reset() {
		this->active = false;
		this->synth = 0;
		this->synth_channel = 0;
		this->pitch = 0;
		this->velocity = 255;
		this->glide = 0;
		this->glide_cur = 0.0;
		this->glide_pitch_from = 0.0;
		this->glide_pitch_to = 0.0;
		this->glide_velo_from = 0.0;
		this->glide_velo_to = 0.0;
		this->delay = 0;
		this->delay_cur = 0.0;
		this->vibrato_amp = 0;
		this->vibrato_freq = 0;
		this->vibrato_phase = 0.0;
		this->tremolo_amp = 0;
		this->tremolo_freq = 0;
		this->tremolo_phase = 0.0;
	}

	void close() {
	}
};

struct Synth {
	u8				index;
	u8				channel_cur;
	u8				channel_count;
	SynthVoice			voices[16];

	Synth() {
		this->index = 0;
		this->channel_count = 6;
		this->channel_cur = 0;
	}

	void init(int synth_index) {
		int			i;

		this->index = synth_index;
		for (i = 0; i < 16; ++i) {
			this->voices[i].synth = synth_index;
			this->voices[i].channel = i;
		}
	}

	void process() {
		int			i;

		for (i = 0; i < 16; ++i) {
			this->voices[i].process();
		}
	}

	SynthVoice* add() {
		return NULL;
	}
};

//////////////////////////////////////////////////
/// PATTERN SOURCE
//////////////////////////////////////////////////

struct PatternEffect {
	enum {
					NONE,
					VIBRATO_AMP,
					VIBRATO_FREQ,
					TREMOLO_AMP,
					TREMOLO_FREQ,
					FADE_IN,
					FADE_OUT
	}				type;
	u8				value;

	PatternEffect() {
		this->type = NONE;
		this->value = 0;
	}
};

struct PatternCell {
	bool				active;
	u8				synth;
	i8				pitch;		// Pitch (< 0 = OFF)
	u8				velocity;
	u8				delay;
	u8				chance;
	PatternEffect			effects[8];

	PatternCell() {
		this->active = false;
		this->synth = 0;
		this->pitch = 0;
		this->velocity = 255;
		this->delay = 0;
		this->chance = 255;
	}
};

struct PatternSource {
	u16				beat_count;	// Beat per pattern
	u16				line_count;	// Lines per row
	u8				row_count;	// Row per pattern
	Array2D<PatternCell>		cells;		// Row X Lines
	u8				lpb;		// Lines per beat
	u8				color;
	i16				index;

	PatternSource() {
		this->beat_count = 4;
		this->line_count = this->beat_count * 4;
		this->row_count = 1;
		this->lpb = 4;
		this->cells.allocate(this->row_count, this->line_count);
		this->color = 0;
	}

	bool resize(int line_count, int track_count) {
		// ...
		return true;
	}
};

//////////////////////////////////////////////////
/// TIMELINE
//////////////////////////////////////////////////

struct TimelineRow {
	PatternSource*			pattern;
	u32				pattern_beat;
	u32				pattern_start;
	SynthVoice*			voices[32];		// Synth voices (1 voice / row)
	PatternCell*			cells[32];
	int				row;

	TimelineRow() {
		this->pattern = NULL;
		this->pattern_beat = 0;
		this->row = 0;
	}

	void process(Timeline* timeline, Clock clock) {
		Clock			clock_relative;
		int			pattern_index;
		PatterSource*		pattern_new;
		PatternCell*		cell;
		SynthVoice*		voice;
		int			line, row;

		pattern_index = timeline.timeline[this->row][clock.beat];
		/// PATTERN ON
		if (pattern_index >= 0) {
			/// [1] COMPUTE PATTERN CHANGE / TIMING
			pattern_new = timeline->patterns[pattern_index];
			/// PATTERN KEEP
			if (pattern_new == pattern) {
				clock_relative.beat = (clock.beat - pattern_start) % pattern_new.beat_count;
				clock_relative.phase = clock.phase;
			/// PATTERN CHANGE
			} else {
				this->close();
				clock_relative.beat = 0;
				clock_relative.phase = clock.phase;
				this->pattern_beat = 0;
				this->pattern_start = clock.beat;
				this->pattern = pattern_new;
			}
			/// [2] COMPUTE PATTERN PLAYING
			line = clock_relative.beat * this->pattern->lpb
			/**/ + clock_relative.phase * this->pattern->lpb;
			for (row = 0; row < 32; ++row) {
				cell = &(this->pattern->cells[row][line]);
				/// CELL CHANGE
				if (cell != this->cells[row]) {
					/// NOTE CHANGE
					if (cell->active == true) {
						/// NOTE NEW
						if (cell->pitch >= 0) {
						/// NOTE STOP
						} else {
						}
					/// NOTE KEEP
					} else {
					}
					this->cells[row] = cell;
				}
				/// CELL PROCESS
				this->voices[i].process(pattern, row, line);
			}
		/// PATTERN OFF
		} else {
			/// PATTERN SWITCH OFF
			if (this->pattern != NULL) {
				this->close();
			}
			this->pattern_beat = 0;
		}
	}

	void close() {
		/// [1] CLOSE VOICES GATES
		//for (i = 0; i < 32; ++i) {
		//}
		/// [2] CLEAR VOICES
	}
};

struct Timeline {
	u16				line_count;
	Array2D<i16>			timeline;
	TimelineRow			rows[32];

	//Vector<PatternSource>		patterns;
	//Vector<Synth>			synths;

	Timeline() {
		int			i;

		for (i = 0; i < 32; ++i)
			rows[i].row = i;
	}
};

#endif
