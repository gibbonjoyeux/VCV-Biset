
#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "plugin.hpp"

//////////////////////////////////////////////////
/// PATTERN SOURCE
//////////////////////////////////////////////////

struct PatternEffect {
	u8			type;
	u8			value;
};

struct PatternLine {
	u8			synth;
	u8			pitch;
	u8			octave;
	u8			velocity;
	u8			delay;
	u8			chance;
	PatternEffect		effects[8];
};

struct PatternSource {
	u16			length;
	u8			columns;
	vector<PatternLine>	lines[64];
	u8			lpb;		// Lines per beat
};

//////////////////////////////////////////////////
/// PATTERN INSTANCE
//////////////////////////////////////////////////

struct PatternVoice {
	u8			synth;
	u8			synth_channel;
	u8			pitch;
	u8			velocity;

	float			glide;
	float			glide_cur;
	float			glide_pitch_from;
	float			glide_pitch_to;
	float			glide_velo_from;
	float			glide_velo_to;

	float			delay;
	float			delay_cur;
	float			time;
	float			time_cur;

	float			vibrato_amp;
	float			vibrato_freq;
	float			vibrato_phase;
	float			tremolo_amp;
	float			tremolo_freq;
	float			tremolo_phase;
};

struct PatternInstance {
	PatternSource*		source;
	PatternVoice		lines[64];
	int			line;		// Timeline line
	int			beat;		// Timeline beat
};

#endif
