
#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "plugin.hpp"

typedef struct Clock			Clock;
typedef struct SynthVoice		SynthVoice;
typedef struct Synth			Synth;
typedef struct PatternEffect	PatternEffect;
typedef struct PatternCell		PatternCell;
typedef struct PatternSource	PatternSource;
typedef struct TimelineRow		TimelineRow;
typedef struct Timeline			Timeline;

struct Clock {
	u32							beat;
	float						phase;

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

	void reset() {
		this->beat = 0;
		this->phase = 0.0;
	}
};

//////////////////////////////////////////////////
/// SYNTH
//////////////////////////////////////////////////

struct SynthVoice {
	bool						active;

	u8							synth;
	u8							synth_channel;
	u8							pitch;
	u8							velocity;

	u8							glide;
	float						glide_cur;
	float						glide_pitch_from;
	float						glide_pitch_to;
	float						glide_velo_from;
	float						glide_velo_to;

	u8							delay;
	float						delay_cur;

	u8							vibrato_amp;
	u8							vibrato_freq;
	float						vibrato_phase;
	u8							tremolo_amp;
	u8							tremolo_freq;
	float						tremolo_phase;

	SynthVoice() {
		this->reset();
	}

	void process() {
	}

	void start(int pitch) {
		this->pitch = pitch;
		this->active = true;
	}

	void init(int synth, int channel) {
		this->reset();
		this->synth = synth;
		this->synth_channel = channel;
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
	u8							index;
	u8							channel_cur;
	u8							channel_count;
	SynthVoice					voices[16];

	Synth() {
		this->index = 0;
		this->channel_count = 6;
		this->channel_cur = 0;
	}

	void init(int synth_index) {
		int			i;

		this->index = synth_index;
		for (i = 0; i < 16; ++i)
			this->voices[i].init(synth_index, i);
	}

	void process() {
		int			i;

		for (i = 0; i < 16; ++i)
			this->voices[i].process();
	}

	SynthVoice* add(int pitch) {
		SynthVoice*				voice;

		voice = &(this->voices[this->channel_cur]);
		voice->start(pitch);
		this->channel_cur = (this->channel_cur + 1) % channel_count;
		return voice;
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
	}							type;
	u8							value;

	PatternEffect() {
		this->type = NONE;
		this->value = 0;
	}
};

struct PatternCell {
	i8							mode;		// 0: keep 1: note -1: stop
	u8							synth;
	u8							pitch;
	u8							velocity;
	u8							delay;
	u8							chance;
	PatternEffect				effects[8];

	PatternCell() {
		this->mode = 0;
		this->synth = 0;
		this->pitch = 0;
		this->velocity = 255;
		this->delay = 0;
		this->chance = 255;
	}
};

struct PatternSource {
	u16							beat_count;	// Beat per pattern
	u16							line_count;	// Lines per row
	u8							row_count;	// Row per pattern
	Array2D<PatternCell>		cells;		// Row X Lines
	u8							lpb;		// Lines per beat
	u8							color;
	i16							index;

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

struct PatternInstance {
	SynthVoice*					voices[32];	// Synth voices (1 voice : 1 row)
	PatternCell*				cells[32];

	PatternInstance() {
		int						i;

		for (i = 0; i < 32; ++i) {
			this->voices[i] = NULL;
			this->cells[i] = NULL;
		}
	}

	void process(Timeline* timeline, PatternSource* pattern, Clock clock) {
		int						line, row;
		PatternCell*			cell;
		SynthVoice*				voice;

		line = clock.beat * pattern->lpb + clock.phase * pattern->lpb;
		for (row = 0; row < 32; ++row) {
			cell = &(pattern->cells[row][line]);
			voice = this->voices[row];
			/// CELL CHANGE
			if (cell != this->cells[row]) {
				/// NOTE CHANGE
				if (cell->mode == 1) {
					/// CLOSE NOTE
					if (this->cells[row]) {
						if (voice) {
							voice->close();
							voice = NULL;
							this->voices[row] = NULL;
						}
					}
					/// LOAD NEW NOTE

					// ! ! ! ! 
					if (cell->synth < timeline->synths.size()) {
						voice = timeline->synths[cell->synth].add(cell->pitch);
					}
					// ! ! ! !

				/// NOTE KEEP
				} else if (cell->mode == 0) {
				/// NOTE STOP
				} else {
					/// CLOSE NOTE
					if (voice) {
						voice->close();
						voice = NULL;
						this->voices[row] = NULL;
					}
				}
				this->cells[row] = cell;
			}
		}
	}

	void close() {
		int						row;

		for (row = 0; row < 32; ++row) {
			if (this->voices[row])
				this->voices[row]->close();
			this->voices[row] = NULL;
			this->cells[row] = NULL;
		}
	}
};

//////////////////////////////////////////////////
/// TIMELINE
//////////////////////////////////////////////////

struct TimelineRow {
	PatternSource*				pattern;
	u32							pattern_beat;
	u32							pattern_start;
	PatternInstance				pattern_instance;
	int							row;

	TimelineRow() {
		this->pattern = NULL;
		this->pattern_beat = 0;
		this->row = 0;
	}

	void init(int row) {
		this->row = row;
	}

	void process(Timeline* timeline, Clock clock) {
		Clock					clock_relative;
		int						pattern_index;
		PatternSource*			pattern_new;

		pattern_index = timeline->timeline[this->row][clock.beat];
		/// PATTERN ON
		if (pattern_index >= 0
		&& pattern_index < timeline->patterns.size()) {
			/// [1] COMPUTE PATTERN CHANGE / TIMING
			pattern_new = &(timeline->patterns[pattern_index]);
			/// PATTERN KEEP
			if (pattern_new == pattern) {
				clock_relative.beat = (clock.beat - pattern_start)
				/**/ % pattern_new->beat_count;
				clock_relative.phase = clock.phase;
			/// PATTERN CHANGE
			} else {
				this->pattern_instance.close();
				this->pattern_beat = 0;
				this->pattern_start = clock.beat;
				this->pattern = pattern_new;
				clock_relative.beat = 0;
				clock_relative.phase = clock.phase;
			}
			/// [2] COMPUTE PATTERN PROCESS
			this->pattern_instance.process(timeline, this->pattern,
			/**/ clock_relative);
		/// PATTERN OFF
		} else {
			/// PATTERN SWITCH OFF
			if (this->pattern != NULL)
				this->pattern_instance.close();
			this->pattern_beat = 0;
		}
	}
};

struct Timeline {
	Clock						clock;
	u16							line_count;
	Array2D<i16>				timeline;
	TimelineRow					rows[32];

	vector<PatternSource>		patterns;
	vector<Synth>				synths;

	Timeline() {
		int						i;

		/// INIT ROWS
		for (i = 0; i < 32; ++i)
			rows[i].init(i);
		/// INIT CLOCK
		clock.reset();
	}

	void process(float dt) {
		int						len;
		int						i;

		/// UPDATE CLOCK
		clock.process(dt);
		/// UPDATE TRACK
		for (i = 0; i < 32; ++i) {
			this->rows[i].process(this, clock);
		}
		/// UPDATE SYNTHS
		len = synths.size();
		for (i = i; i < len; ++i)
			synths[i].process();
	}
};

#endif
