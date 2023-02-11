
#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "plugin.hpp"

typedef struct Clock			Clock;
typedef struct PatternEffect		PatternEffect;
typedef struct PatternLine		PatternLine;
typedef struct PatternSource		PatternSource;
typedef struct PatternVoice		PatternVoice;
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

struct PatternLine {
	u8				synth;
	i8				pitch;
	u8				velocity;
	u8				delay;
	u8				chance;
	PatternEffect			effects[8];

	PatternLine() {
		this->synth = 0;
		this->pitch = -1;
		this->velocity = 255;
		this->delay = 0;
		this->chance = 255;
	}
};

struct PatternSource {
	char				name[4];
	u16				line_count;
	u8				track_count;
	Array2D<PatternLine>		lines;
	u8				lpb;		// Lines per beat

	PatternSource() {
		this->line_count = 16;
		this->track_count = 1;
		this->lpb = 4;
		this->lines.allocate(this->track_count, this->line_count);
	}

	bool resize(int line_count, int track_count) {
		// ...
		return true;
	}
};

//////////////////////////////////////////////////
/// TIMELINE
//////////////////////////////////////////////////

struct PatternVoice {
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

	PatternVoice() {
		this->reset();
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
};

struct TimelineRow {
	PatternSource*			pattern;
	PatternVoice			lines[32];
	int				row;

	TimelineRow() {
		this->pattern = NULL;
		this->row = 0;
	}

	void process(Timeline* timeline, Clock clock) {
		//if (timeline.timeline[this->row][clock.beat])
	}
};

struct Timeline {
	u16				line_count;
	Array2D<char[4]>		timeline;
	TimelineRow			rows[32];

	//Vector<PatternSource>		patterns;

	Timeline() {
	}
};

#endif
