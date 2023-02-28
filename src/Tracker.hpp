
#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "plugin.hpp"

#define PATTERN_EFFECT_NONE		0
#define PATTERN_EFFECT_VIBRATO	1
#define PATTERN_EFFECT_TREMOLO	2
#define PATTERN_EFFECT_FADE_IN	3
#define PATTERN_EFFECT_FADE_OUT	4
#define PATTERN_EFFECT_RACHET	5

#define PATTERN_NOTE_KEEP		0
#define PATTERN_NOTE_NEW		1
#define PATTERN_NOTE_STOP		-1
#define PATTERN_NOTE_GLIDE		2
#define PATTERN_CV_KEEP			0
#define PATTERN_CV_SET			1

#define TIMELINE_CELL_KEEP		0
#define TIMELINE_CELL_ADD		1
#define TIMELINE_CELL_STOP		-1

typedef struct Clock			Clock;
typedef struct SynthVoice		SynthVoice;
typedef struct Synth			Synth;
typedef struct PatternEffect	PatternEffect;
typedef struct PatternNote		PatternNote;
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
	u8							type;
	u8							value;

	PatternEffect() {
		this->type = PATTERN_EFFECT_NONE;
		this->value = 0;
	}
};

struct PatternCV {
	i8							mode;		// PATTERN_CV_xxx
	u8							synth;
	u8							channel;
	u8							value;
	u8							glide;

	PatternCV() {
		this->mode = PATTERN_CV_KEEP;
		this->synth = 0;
		this->channel = 0;
		this->value = 0;
		this->glide = 0;
	}
};

struct PatternNote {
	i8							mode;		// PATTERN_NOTE_xxx
	u8							glide;
	u8							synth;
	u8							pitch;
	u8							velocity;
	u8							delay;
	u8							chance;
	PatternEffect				effects[8];

	PatternNote() {
		this->mode = PATTERN_NOTE_KEEP;
		this->glide = 0;
		this->synth = 0;
		this->pitch = 0;
		this->velocity = 255;
		this->delay = 0;
		this->chance = 255;
	}
};

struct PatternCVRow {
	u8							synth;		// CV synth output
	u8							channel;	// CV synth channel output
	PatternCV					cvs[0];		// CVs (memory as struct extension)
};

struct PatternNoteRow {
	u8							effect_count;
	PatternNote					notes[0];	// Notes (memory as struct extension)
};

struct PatternSource {
	u16							beat_count;	// Beat per pattern
	u16							line_count;	// Lines per row
	u16							note_count;	// Note rows
	u16							cv_count;	// CV rows
	//u8							row_count;	// Row per pattern
	Array2D<PatternNote>		notes;		// Row X note nines
	Array2D<PatternCV>			cvs;		// Row X CV lines
	ArrayExt<PatternNote>		test;
	u8							lpb;		// Lines per beat
	u8							color;
	i16							index;

	PatternSource() {
		this->beat_count = 4;
		this->line_count = this->beat_count * 4;
		this->note_count = 4;
		this->cv_count = 0;
		this->lpb = 4;
		this->notes.allocate(this->note_count, this->line_count);
		this->cvs.allocate(this->cv_count, this->line_count);
		this->color = 0;
	}

	void resize(int note_count, int cv_count, int beat_count, int lpb) {
		bool					change_note, change_cv;

		/// [1] CHECK CHANGES
		change_note = false;
		change_cv = false;
		if (this->beat_count != beat_count
		|| this->lpb != lpb) {
			change_note = true;
			change_cv = true;
		}
		if (this->note_count != note_count)
			change_note = true;
		if (this->cv_count != cv_count)
			change_cv = true;
		/// [2] SET VARIABLES
		this->lpb = lpb;
		this->beat_count = beat_count;
		this->line_count = this->beat_count * this->lpb;
		this->note_count = note_count;
		this->cv_count = cv_count;
		/// [3] ALLOCATE ARRAYS
		if (change_note)
			this->notes.allocate(this->note_count, this->line_count);
		if (change_cv)
			this->cvs.allocate(this->cv_count, this->line_count);
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

	float						pitch_glide_len;
	float						pitch_glide_cur;
	float						pitch_from;
	float						pitch_to;

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
		float					pitch, mix;
		float					velocity;
		float					vibrato, tremolo;

		if (this->active && this->delay <= 0 && this->delay_gate <= 0) {
			/// COMPUTE CV
			//pitch = (float)(this->pitch - 69) / 12.0f;
			velocity = (float)this->velocity / 25.5;
			/// COMPUTE EFFECTS
			//// COMPUTE GLIDE
			if (this->pitch_glide_len > 0) {
				this->pitch_glide_cur += dt_beat;
				/// GLIDE ENDED
				if (this->pitch_glide_cur >= this->pitch_glide_len) {
					this->pitch_glide_len = 0;
					this->pitch_glide_cur = 0;
					this->pitch_from = this->pitch_to;
					pitch = (float)(this->pitch_from - 69.0) / 12.0f;
				/// GLIDE RUNNING
				} else {
					mix = this->pitch_glide_cur / this->pitch_glide_len;
					pitch = ((this->pitch_from * (1.0 - mix)
					/**/ + this->pitch_to * mix) - 69.0) / 12.0f;
				}
			} else {
				pitch = (float)(this->pitch_from - 69) / 12.0f;
			}
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

	bool start(PatternNote *note, int lpb) {
		PatternEffect			*effect;
		int						i;

		delay = (1.0f / (float)lpb) * ((float)note->delay / 256.0f);
		/// START NOTE ON CHANCE
		if (note->chance == 255 || random::uniform() * 255.0 < note->chance) {
			/// SET INTER NOTE GATE DELAY
			this->delay_gate = (this->active) ? 0.001f : 0.0f;
			/// SET MAIN DELAY
			this->delay = delay;
			/// SET NOTE PROPS
			this->velocity = note->velocity;
			this->pitch = note->pitch;
			this->pitch_from = note->pitch;
			this->pitch_to = note->pitch;
			this->pitch_glide_len = 0;
			this->pitch_glide_cur = 0;
			/// SET EFFECTS
			this->vibrato_amp = 0;
			this->tremolo_amp = 0;
			for (i = 0; i < 8; ++i) {
				effect = &(note->effects[i]);
				switch(effect->type) {
					case PATTERN_EFFECT_NONE:
						break;
					case PATTERN_EFFECT_VIBRATO:
						this->vibrato_freq =
						/**/ (float)(effect->value / 16) * M_PI * 2.0f;
						this->vibrato_amp = (float)(effect->value % 16) / 64.0;
						break;
					case PATTERN_EFFECT_TREMOLO:
						this->tremolo_freq =
						/**/ (float)(effect->value / 16) * M_PI * 2.0f;
						this->tremolo_amp = (float)(effect->value % 16) / 16.0;
						break;
					case PATTERN_EFFECT_FADE_IN:
						break;
					case PATTERN_EFFECT_FADE_OUT:
						break;
					case PATTERN_EFFECT_RACHET:
						break;
				}
			}
			/// ACTIVATE NOTE
			this->active = true;
			return true;
		}
		return false;
	}

	void glide(PatternNote *note) {
		float						mix;

		if (this->active) {
			/// GLIDE ALREADY RUNNING
			if (this->pitch_glide_len > 0) {
				/// COMPUTE GLIDING PITCH
				mix = this->pitch_glide_cur / this->pitch_glide_len;
				this->pitch_from = this->pitch_from * (1.0 - mix)
				/**/ + this->pitch_to * mix;
			/// GLIDE OFF
			} else {
			}
			this->pitch_to = note->pitch;
			this->pitch_glide_len = (1.0 - ((float)note->glide / 256.0));
			this->pitch_glide_cur = 0;
		}
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
		this->pitch_glide_len = 0;
		this->pitch_glide_cur = 0.0;
		this->pitch_from = 0.0;
		this->pitch_to = 0.0;
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

	SynthVoice* add(PatternNote *note, int lpb) {
		SynthVoice*				voice;

		voice = &(this->voices[this->channel_cur]);
		if (voice->start(note, lpb) == true) {
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
	PatternNote*				notes[32];

	PatternInstance() {
		this->reset();
	}

	void reset() {
		int						i;

		for (i = 0; i < 32; ++i) {
			this->voices[i] = NULL;
			this->notes[i] = NULL;
		}
	}

	void process(Module *module, vector<Synth>* synths, PatternSource* pattern,
	Clock clock, int *debug, int *debug_2, char *debug_str) {
		int						line, row;
		PatternCV				*cv_from, *cv_to;
		PatternNote				*note;
		SynthVoice				*voice;
		int						line_from, line_to;
		float					phase;
		float					cv_phase;
		float					cv_value;

		line = clock.beat * pattern->lpb + clock.phase * pattern->lpb;
		phase = clock.phase * pattern->lpb;
		phase -= (int)phase;

		*debug = line;
		*debug_2 = 0;
		//sprintf(debug_str, "%d x %d (%d:%d) %d/%d", clock.beat, line,
		///**/ note->mode, note->synth,
		///**/ voice->channel, module->outputs[1].getChannels());

		/// [1] COMPUTE PATTERN NOTE ROWS
		for (row = 0; row < pattern->note_count; ++row) {
			note = &(pattern->notes[row][line]);
			voice = this->voices[row];
			/// CELL CHANGE
			// TODO: ! ! ! Dangerous comparision as the PatternSource arrays
			// might be reallocated on resize ! ! !
			if (note != this->notes[row]) {
				/// NOTE CHANGE
				if (note->mode == PATTERN_NOTE_NEW) {
					/// CLOSE ACTIVE NOTE
					if (voice) {
						voice->stop();
						this->voices[row] = NULL;
					}
					/// ADD NEW NOTE
					if (note->synth < synths->size()) {
						voice = synths->at(note->synth).add(note, pattern->lpb);
						this->voices[row] = voice;
					}
				/// NOTE GLIDE
				} else if (note->mode == PATTERN_NOTE_GLIDE) {
					if (voice) {
						voice->glide(note);
					}
				/// NOTE KEEP
				} else if (note->mode == PATTERN_NOTE_GLIDE) {
				/// NOTE STOP
				} else if (note->mode == PATTERN_NOTE_STOP) {
					/// CLOSE ACTIVE NOTE
					if (voice) {
						voice->stop();
						this->voices[row] = NULL;
					}
				}
				this->notes[row] = note;
			}
		}
		/// [2] COMPUTE PATTERN CV ROWS
		for (row = 0; row < pattern->cv_count; ++row) {
			//cv = &(pattern->cvs[row][line]);
			/// FIND LINE FROM
			line_from = line - 1;
			while (line_from >= 0
			&& pattern->cvs[row][line_from].mode != PATTERN_CV_SET)
				line_from -= 1;
			if (line_from < 0)
				line_from = line;
			cv_from = &(pattern->cvs[row][line_from]);
			/// FIND LINE TO
			line_to = line + 1;
			while (line_to < pattern->line_count
			&& pattern->cvs[row][line_to].mode != PATTERN_CV_SET)
				line_to += 1;
			if (line_to >= pattern->line_count)
				line_to = line;
			cv_to = &(pattern->cvs[row][line_to]);
			/// COMPUTE CV PHASE
			if (line_from == line_to) {
				cv_phase = 0;
			} else {
				// TODO: add inter line phase
				cv_phase = (((float)line + phase) - (float)line_from)
				/**/ / (float)(line_to - line_from + 1);
			}
			/// COMPUTE CV VALUE
			cv_value = (float)cv_from->value +
			/**/ ((float)cv_to->value - (float)cv_from->value)
			/**/ * cv_phase;
			/// REMAP CV FROM [0:255] TO [0:10]
			cv_value /= 25.5;
			/// OUTPUT CV
			// TODO: Use ArrayExt and get SYNTH & CHANNEL from row
			module->outputs[1 + pattern->cvs[row][0].synth]
			/**/ .setVoltage(cv_value);
		}
	}

	void stop() {
		int						row;

		for (row = 0; row < 32; ++row) {
			if (this->voices[row])
				this->voices[row]->stop();
			this->voices[row] = NULL;
			this->notes[row] = NULL;
		}
	}
};

struct TimelineCell {
	i8							mode;		// TIMELINE_CELL_xxx
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
