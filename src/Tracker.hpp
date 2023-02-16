
#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "plugin.hpp"

typedef struct Clock			Clock;
typedef struct SynthVoice		SynthVoice;
typedef struct Synth			Synth;
typedef struct PatternEffect	PatternEffect;
typedef struct PatternCell		PatternCell;
typedef struct PatternSource	PatternSource;
typedef struct TimelineCell		TimelineCell;
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
/// PATTERN SOURCE
//////////////////////////////////////////////////

struct PatternEffect {
	enum {
								NONE,
								VIBRATO,
								TREMOLO,
								FADE_IN,
								FADE_OUT,
								RACHET,
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

	void init(int row_count, int beat_count, int lpb) {
		this->beat_count = beat_count;
		this->line_count = this->beat_count * lpb;
		this->row_count = row_count;
		this->lpb = lpb;
		this->cells.allocate(this->row_count, this->line_count);
	}
};

//////////////////////////////////////////////////
/// SYNTH
//////////////////////////////////////////////////

struct SynthVoice {
	bool						active;

	u8							synth;
	u8							channel;
	u8							pitch;
	u8							velocity;

	u8							glide;
	float						glide_cur;
	float						glide_pitch_from;
	float						glide_pitch_to;
	float						glide_velo_from;
	float						glide_velo_to;

	float						delay;
	float						delay_gate;

	float						vibrato_amp;
	float						vibrato_freq;
	float						vibrato_phase;
	float						tremolo_amp;
	float						tremolo_freq;
	float						tremolo_phase;

	SynthVoice() {
		this->reset();
	}

	void process(Module *module, float dt_sec, float dt_beat) {
		float					pitch;
		float					velocity;
		float					vibrato, tremolo;

		if (this->active && this->delay <= 0 && this->delay_gate <= 0) {
			/// COMPUTE CV
			pitch = (float)(this->pitch - 69) / 12.0f;
			velocity = (float)this->velocity / 25.5;
			/// COMPUTE EFFECTS
			//// COMPUTE VIBRATO
			if (this->vibrato_amp > 0) {
				this->vibrato_phase += this->vibrato_freq * dt_beat;
				vibrato = sinf(this->vibrato_phase) * this->vibrato_amp;
				pitch += vibrato;
			}
			//// COMPUTE TREMOLO
			if (this->tremolo_amp > 0) {
				this->tremolo_phase += this->tremolo_freq * dt_beat;
				tremolo = 0.5f + sinf(this->tremolo_phase) * 0.5f
				/**/ * this->tremolo_amp;
				velocity *= tremolo;
			}
			/// SET OUTPUT
			module->outputs[1 + this->synth].setVoltage(pitch, this->channel);
			module->outputs[9 + this->synth].setVoltage(10.0f, this->channel);
			module->outputs[17 + this->synth].setVoltage(velocity, this->channel);
		} else {
			/// HANDLE NOTE DELAY
			if (this->delay > 0)
				this->delay -= dt_beat;
			/// HANDLE INTER NOTES GATE DELAY
			if (this->delay_gate > 0)
				this->delay_gate -= dt_sec;
			/// SET OUTPUT
			module->outputs[9 + this->synth].setVoltage(0.0f, this->channel);
		}
	}

	bool start(PatternCell *cell, int lpb) {
		PatternEffect			*effect;
		int						i;

		delay = (1.0f / (float)lpb) * ((float)cell->delay / 256.0f);
		/// START NOTE ON CHANCE
		if (cell->chance == 255 || random::uniform() * 255.0 < cell->chance) {
			/// SET INTER NOTE GATE DELAY
			this->delay_gate = (this->active) ? 0.001f : 0.0f;
			/// SET MAIN DELAY
			this->delay = delay;
			/// SET NOTE PROPS
			this->pitch = cell->pitch;
			this->velocity = cell->velocity;
			/// SET EFFECTS
			this->vibrato_amp = 0;
			this->tremolo_amp = 0;
			for (i = 0; i < 8; ++i) {
				effect = &(cell->effects[i]);
				switch(effect->type) {
					case PatternEffect::NONE:
						break;
					case PatternEffect::VIBRATO:
						this->vibrato_freq =
						/**/ (float)(effect->value / 16) * M_PI * 2.0f;
						this->vibrato_amp = (float)(effect->value % 16) / 64.0;
						break;
					case PatternEffect::TREMOLO:
						this->tremolo_freq =
						/**/ (float)(effect->value / 16) * M_PI * 2.0f;
						this->tremolo_amp = (float)(effect->value % 16) / 16.0;
						break;
					case PatternEffect::FADE_IN:
						break;
					case PatternEffect::FADE_OUT:
						break;
					case PatternEffect::RACHET:
						break;
				}
			}
			/// ACTIVATE NOTE
			this->active = true;
			return true;
		}
		return false;
	}

	void stop() {
		this->active = false;
	}

	void init(int synth, int channel) {
		this->reset();
		this->synth = synth;
		this->channel = channel;
	}

	void reset() {
		this->active = false;
		this->pitch = 0;
		this->velocity = 255;
		this->glide = 0;
		this->glide_cur = 0.0;
		this->glide_pitch_from = 0.0;
		this->glide_pitch_to = 0.0;
		this->glide_velo_from = 0.0;
		this->glide_velo_to = 0.0;
		this->delay = 0;
		this->vibrato_amp = 0;
		this->vibrato_freq = 0;
		this->vibrato_phase = 0.0;
		this->tremolo_amp = 0;
		this->tremolo_freq = 0;
		this->tremolo_phase = 0.0;
	}
};

struct Synth {
	u8							index;
	u8							channel_cur;
	u8							channel_count;
	SynthVoice					voices[16];

	Synth() {
		this->index = 0;
		this->channel_cur = 0;
		this->channel_count = 6;
	}

	void init(int synth_index, int channel_count) {
		int			i;

		/// INIT VOICES
		this->index = synth_index;
		for (i = 0; i < 16; ++i)
			this->voices[i].init(this->index, i);
		/// SET CHANNEL COUNT
		this->channel_count = channel_count;
	}

	void process(Module *module, float dt_sec, float dt_beat) {
		int			i;

		/// SET OUTPUT CHANNELS
		module->outputs[1 + this->index].setChannels(this->channel_count);
		module->outputs[1 + 8 + this->index].setChannels(this->channel_count);
		module->outputs[1 + 16 + this->index].setChannels(this->channel_count);
		/// COMPUTE VOICES
		for (i = 0; i < 16; ++i)
			this->voices[i].process(module, dt_sec, dt_beat);
	}

	SynthVoice* add(PatternCell *cell, int lpb) {
		SynthVoice*				voice;

		voice = &(this->voices[this->channel_cur]);
		if (voice->start(cell, lpb) == true) {
			this->channel_cur = (this->channel_cur + 1) % channel_count;
			return voice;
		}
		return NULL;
	}
};

//////////////////////////////////////////////////
/// TIMELINE
//////////////////////////////////////////////////

struct PatternInstance {
	SynthVoice*					voices[32];	// Synth voices (1 voice : 1 row)
	PatternCell*				cells[32];

	PatternInstance() {
		this->reset();
	}

	void reset() {
		int						i;

		for (i = 0; i < 32; ++i) {
			this->voices[i] = NULL;
			this->cells[i] = NULL;
		}
	}

	void process(Module *module, vector<Synth>* synths, PatternSource* pattern,
	Clock clock, int *debug, int *debug_2, char *debug_str) {
		int						line, row;
		PatternCell*			cell;
		SynthVoice*				voice;

		line = clock.beat * pattern->lpb + clock.phase * pattern->lpb;
		*debug = line;
		*debug_2 = 0;
		//sprintf(debug_str, "%d x %d (%d:%d) %d/%d", clock.beat, line,
		///**/ cell->mode, cell->synth,
		///**/ voice->channel, module->outputs[1].getChannels());
		for (row = 0; row < pattern->row_count; ++row) {
			cell = &(pattern->cells[row][line]);
			voice = this->voices[row];
			/// CELL CHANGE
			if (cell != this->cells[row]) {
				/// NOTE CHANGE
				if (cell->mode == 1) {
					/// CLOSE ACTIVE NOTE
					if (voice) {
						voice->stop();
						this->voices[row] = NULL;
					}
					/// ADD NEW NOTE
					if (cell->synth < synths->size()) {
						voice = synths->at(cell->synth).add(cell, pattern->lpb);
						this->voices[row] = voice;
					}
				/// NOTE KEEP
				} else if (cell->mode == 0) {
				/// NOTE STOP
				} else {
					/// CLOSE ACTIVE NOTE
					if (voice) {
						voice->stop();
						this->voices[row] = NULL;
					}
				}
				this->cells[row] = cell;
			}
		}
	}

	void stop() {
		int						row;

		for (row = 0; row < 32; ++row) {
			if (this->voices[row])
				this->voices[row]->stop();
			this->voices[row] = NULL;
			this->cells[row] = NULL;
		}
	}
};

struct TimelineCell {
	i8							mode;		// 0: keep 1: note -1: stop
	u16							pattern;	// Pattern index
	u16							beat;		// Pattern starting beat

	TimelineCell() {
		this->mode = 0;
		this->pattern = 0;
		this->beat = 0;
	}
};

struct Timeline {
	char						debug_str[1024];
	int							debug;
	int							debug_2;

	Clock						clock;
	u16							beat_count;
	Array2D<TimelineCell>		timeline;
	PatternSource*				pattern[32];
	TimelineCell*				pattern_cell[32];
	u32							pattern_start[32];
	PatternInstance				pattern_instance[32];

	vector<PatternSource>		patterns;
	vector<Synth>				synths;

	Timeline() {
		int						i;

		/// [1] INIT ROWS
		for (i = 0; i < 32; ++i) {
			this->pattern[i] = NULL;
			this->pattern_cell[i] = NULL;
			this->pattern_start[i] = 0;
			this->pattern_instance[i].reset();
		}
		/// [2] INIT CLOCK
		clock.reset();

		debug = 0;
		debug_2 = 0;
		debug_str[0] = 0;
	}

	void process(Module* module, float dt_sec, float dt_beat) {
		Clock					clock_relative;
		PatternSource*			pattern;
		TimelineCell*			cell;
		int						len;
		int						i;

		/// [1] UPDATE CLOCK
		this->clock.process(dt_beat);
		if (this->clock.beat >= this->beat_count)
			this->clock.beat = 0;

		/// [2] UPDATE TIMELINE ROWS
		for (i = 0; i < 32; ++i) {
			cell = &(this->timeline[i][clock.beat]);
			/// PATTERN CHANGE
			if (cell->mode == 1
			&& this->pattern_cell[i] != cell) {
				if (cell->pattern < (int)this->patterns.size()) {
					/// COMPUTE PATTERN CHANGE / TIMING
					pattern = &(this->patterns[cell->pattern]);
					/// RESET RELATIVE CLOCK
					clock_relative.beat = cell->beat % pattern->beat_count;
					clock_relative.phase = this->clock.phase;
					/// RESET PATTERN
					this->pattern_instance[i].stop();
					this->pattern_start[i] = clock.beat;
					this->pattern_cell[i] = cell;
					this->pattern[i] = pattern;
					/// COMPUTE PATTERN PROCESS
					this->pattern_instance[i].process(module, &this->synths,
					/**/ this->pattern[i], clock_relative,
					/**/ &debug, &debug_2, debug_str);
				}
			/// PATTERN STOP
			} else if (cell->mode == -1) {
				/// PATTERN SWITCH OFF
				if (this->pattern[i] != NULL)
					this->pattern_instance[i].stop();
			/// PATTERN KEEP
			} else {
				if (this->pattern[i]) {
					pattern = this->pattern[i];
					/// COMPUTE RELATIVE CLOCK
					clock_relative.beat =
					/**/ (this->clock.beat - this->pattern_start[i]
					/**/ + this->pattern_cell[i]->beat) % pattern->beat_count;
					clock_relative.phase = this->clock.phase;
					/// COMPUTE PATTERN PROCESS
					this->pattern_instance[i].process(module, &this->synths,
					/**/ this->pattern[i], clock_relative,
					/**/ &debug, &debug_2, debug_str);
				}
			}
		}

		/// [3] UPDATE SYNTHS
		len = synths.size();
		for (i = 0; i < len; ++i)
			synths[i].process(module, dt_sec, dt_beat);
	}
};

#endif
