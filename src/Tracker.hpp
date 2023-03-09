
#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// CONSTANTS
////////////////////////////////////////////////////////////////////////////////

#define PATTERN_EFFECT_NONE			0
#define PATTERN_EFFECT_RAND_AMP		1	// Axx
#define PATTERN_EFFECT_RAND_PAN		2	// Pxx
#define PATTERN_EFFECT_RAND_OCT		3	// Oxy
#define PATTERN_EFFECT_RAND_PITCH	4	// Mxy
#define PATTERN_EFFECT_RAND_SCALE	5	// Sxy
#define PATTERN_EFFECT_VIBRATO		6	// Vxy
#define PATTERN_EFFECT_TREMOLO		7	// Txy
#define PATTERN_EFFECT_FADE_IN		8	// Fxx
#define PATTERN_EFFECT_FADE_OUT		9	// fxx
#define PATTERN_EFFECT_CHANCE		10	// Cxx
#define PATTERN_EFFECT_CHANCE_STOP	11	// cxx
#define PATTERN_EFFECT_RACHET		12	// Rxy

#define PATTERN_NOTE_KEEP			0
#define PATTERN_NOTE_NEW			1
#define PATTERN_NOTE_STOP			-1
#define PATTERN_NOTE_GLIDE			2
#define PATTERN_CV_KEEP				0
#define PATTERN_CV_SET				1

#define TIMELINE_CELL_KEEP			0
#define TIMELINE_CELL_ADD			1
#define TIMELINE_CELL_STOP			-1

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
/// CLOCK
//////////////////////////////////////////////////

struct Clock {
	u32							beat;
	float						phase;

	Clock();

	void process(float dt);
	void reset();
};

//////////////////////////////////////////////////
/// PATTERN SOURCE
//////////////////////////////////////////////////
/// PatternSource is the object that stores a pattern content (notes, cv...).
///  It does not process anything, it only stores origin pattern data while
///  PatternInstance does the process.
/// PatternSource stores its length (beat, line, note, cv).
/// PatternSource stores its notes and cvs as extended 1D arrays. An array being
///  a row containing the lines (and thus notes or cv).
//////////////////////////////////////////////////

struct PatternEffect {
	u8							type;
	u8							value;

	PatternEffect();
};

struct PatternCV {
	i8							mode;		// PATTERN_CV_xxx
	u8							value;
	u8							delay;
	u8							glide;

	PatternCV();
};

struct PatternNote {
	i8							mode;		// PATTERN_NOTE_xxx
	u8							glide;
	u8							synth;
	u8							pitch;
	u8							velocity;
	u8							panning;
	u8							delay;
	u8							chance;
	PatternEffect				effects[8];

	PatternNote();
};

struct PatternCVRow {
	u8							synth;		// CV synth output
	u8							channel;	// CV synth channel output
	PatternCV					lines[0];	// CVs (memory as struct extension)
};

struct PatternNoteRow {
	u8							effect_count;
	PatternNote					lines[0];	// Notes (memory as struct extension)
};

struct PatternSource {
	u16							beat_count;	// Beat per pattern
	u16							line_count;	// Lines per row
	u16							note_count;	// Note rows
	u16							cv_count;	// CV rows
	ArrayExt<PatternCVRow>		cvs;		// Row X CV lines
	ArrayExt<PatternNoteRow>	notes;		// Row X Note lines
	u8							lpb;		// Lines per beat
	u8							color;
	i16							index;

	PatternSource();

	void resize(int note_count, int cv_count, int beat_count, int lpb);
};

//////////////////////////////////////////////////
/// SYNTH
//////////////////////////////////////////////////
/// SynthVoice is the object that compute the synth voice output.
/// SynthVoice store basic playing information (synth, channel, pitch...)
/// SynthVoice can be active (playing) or not.
/// SynthVoice computes the final pitch, gate and velocity.
//////////////////////////////////////////////////
/// Synth stores the basic synth output informations (synth index, channel
///  count, etc.).
/// There are 64 synths stored in the global timeline structure. They can be
///  accessed from anywere.
/// Synth stores 32 SynthVoice to process (play) a voice.
/// Synth stores its outputs (notes and cv) that can be accessed by the
///  TrackerOut module.
//////////////////////////////////////////////////

struct SynthVoice {
	bool						active;

	u8							synth;
	u8							channel;
	u8							pitch;
	u8							velocity;
	u8							panning;

	float						pitch_glide_len;
	float						pitch_glide_cur;
	float						pitch_from;
	float						pitch_to;

	float						delay;		// Delay before start
	float						delay_gate;	// Delay between 2 notes using same voice
	float						delay_stop;	// Delay before stop

	float						vibrato_amp;
	float						vibrato_freq;
	float						vibrato_phase;
	float						tremolo_amp;
	float						tremolo_freq;
	float						tremolo_phase;

	SynthVoice();

	void process(float dt_sec, float dt_beat, float *output);
	bool start(PatternNote *note, int lpb);
	void stop(PatternNote *note, int lpb);
	void init(int synth, int channel);
	void reset();
	void glide(PatternNote *note);
};

struct Synth {
	bool						active;
	u8							index;
	u8							channel_cur;
	u8							channel_count;
	SynthVoice					voices[16];
	float						out_synth[16 * 4];	// Out synth (pitch, gate...)
	float						out_cv[8];			// Out CV

	Synth();

	void process(float dt_sec, float dt_beat);
	void init(int synth_index, int channel_count);
	SynthVoice* add(PatternNote *note, int lpb);
};

//////////////////////////////////////////////////
/// TIMELINE
//////////////////////////////////////////////////
/// TODO: Rename to TimelineInstance
/// PatternInstance represent the playing part of a PatternSource. It does not
///  store the origin pattern data, it only process.
/// PatternInstance process (play) a given pattern from the timeline.
/// There are 32 PatternInstance stored in the timeline. One PatternInstance
///  per timeline row. This allows to have multiple patterns playing at the same
///  time (up to 32), even if they are from the same PatternSource.
//////////////////////////////////////////////////
/// TimelineCell is the basic cell of the timeline.
/// It consists of a pattern index and a pattern starting beat.
/// They are stored as 2D array (1st dimension: timeline row - 2nd dimension:
///  line / beat).
//////////////////////////////////////////////////
/// Timeline is the struct that stores the whole track (timeline, patterns...).
/// It stores the main clock and a PatternInstance per timeline row (32).
/// There are 32 tracks / rows and their length is based on the beat count.
/// Each cell / line of the tracks represent one beat.
//////////////////////////////////////////////////

struct PatternInstance {
	SynthVoice*					voices[32];	// Synth voices (1 voice : 1 row)
	PatternNote*				notes[32];

	PatternInstance();

	void process(Synth *synths, PatternSource* pattern, Clock clock,
		int *debug, int *debug_2, char *debug_str);
	void reset(void);
	void stop(void);
};

struct TimelineCell {
	i8							mode;		// TIMELINE_CELL_xxx
	u16							pattern;	// Pattern index
	u16							beat;		// Pattern starting beat

	TimelineCell();
};

struct Timeline {
	char						debug_str[1024];
	int							debug;
	int							debug_2;

	Clock						clock;
	u16							beat_count;
	Array2D<TimelineCell>		timeline;
	PatternSource*				pattern_source[32];
	TimelineCell*				pattern_cell[32];
	u32							pattern_start[32];
	PatternInstance				pattern_instance[32];

	vector<PatternSource>		patterns;
	Synth						synths[64];

	Timeline();

	void process(float dt_sec, float dt_beat);
	void resize(int beat_count);
};

////////////////////////////////////////////////////////////////////////////////
/// GLOBAL DATA
////////////////////////////////////////////////////////////////////////////////

extern Timeline	g_timeline;

#endif
