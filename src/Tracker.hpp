

#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// CONSTANTS
////////////////////////////////////////////////////////////////////////////////

#define EDITOR_MODE_PATTERN			0
#define EDITOR_MODE_TIMELINE		1
#define EDITOR_MODE_PARAMETERS		2

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
#define PATTERN_NOTE_CHANGE			3
#define PATTERN_CV_KEEP				0
#define PATTERN_CV_SET				1

#define TIMELINE_CELL_KEEP			0
#define TIMELINE_CELL_ADD			1
#define TIMELINE_CELL_STOP			-1


#define CHAR_W						6.302522
#define CHAR_H						8.5
#define CHAR_COUNT_X				84
#define CHAR_COUNT_Y				39


extern char		table_pitch[12][3];
extern char		table_effect[13];					// 12
extern char		table_hex[17];					// 16
extern int		table_row_note_width[23];			// 23
extern int		table_row_note_pos[23];			// 23
extern int		table_row_cv_width[3];			// 3
extern int		table_row_cv_pos[3];				// 3
extern int		table_keyboard[128];
extern NVGcolor	colors[16];

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
	//u8							chance;
	PatternEffect				effects[8];

	PatternNote();
};

struct PatternCVRow {
	u8							mode;		// CV | BPM
	u8							synth;		// CV synth output
	u8							channel;	// CV synth channel output
	PatternCV					lines[0];	// CVs (memory as struct extension)
};

struct PatternNoteRow {
	u8							mode;		// Synth | Drum
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
	//u8							color;
	//i16							index;

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
	bool start(PatternNoteRow *row, PatternNote *note, int lpb);
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
	SynthVoice* add(PatternNoteRow *row, PatternNote *note, int lpb);
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

	//vector<PatternSource>		patterns;
	PatternSource				patterns[256];
	Synth						synths[64];

	u8							*save_buffer;
	u32							save_length;
	u32							save_cursor;
	u32							save_cursor_save;
	bool						save_to_change;
	bool						save_endian_reverse;

	Timeline();

	void process(float dt_sec, float dt_beat);
	void resize(int beat_count);
};

//////////////////////////////////////////////////
/// EDITOR
//////////////////////////////////////////////////
/// Editor stores the tracker editor information
///  Such as active row, line, cell, etc.
//////////////////////////////////////////////////

struct EditorSwitch {
	bool						state;
	float						previous_input;

	EditorSwitch();

	bool process(float in);
};

struct EditorTrigger : dsp::BooleanTrigger {
};

struct Editor {
	Module						*module;

	int							mode;			// Pattern / Timeline / Param
	bool						selected;
	int							pattern_id;
	PatternSource				*pattern;
	int							pattern_track;
	int							pattern_row;
	int							pattern_row_prev;
	int							pattern_line;
	int							pattern_cell;
	int							pattern_char;
	int							pattern_cam_x;
	int							pattern_cam_y;
	char						pattern_debug[4];
	bool						pattern_view_velo;
	bool						pattern_view_pan;
	bool						pattern_view_glide;
	bool						pattern_view_delay;
	bool						pattern_view_fx;
	int							pattern_octave;
	int							pattern_jump;

	int							timeline_cam_x;
	int							timeline_cam_y;

	int							synth_id;

	EditorSwitch				switch_view[5];
	EditorTrigger				button_mode[3];
	EditorTrigger				button_octave[2];
	EditorTrigger				button_jump[2];
	EditorTrigger				button_save;
	EditorTrigger				button_play[4];

	Editor();

	void process(void);
	void save_edition(void);
	void set_row(int index);
	void set_song_length(int length, bool mode);
	void set_synth(int index, bool mode);
	void set_pattern(int index, bool mode);
	void pattern_clamp_cursor(void);
	void pattern_move_cursor_x(int x);
	void pattern_move_cursor_y(int y);
};

////////////////////////////////////////////////////////////////////////////////
/// VCV RACK DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct Tracker : Module {
	enum ParamIds {
								PARAM_PLAY_SONG,
								PARAM_PLAY_PATTERN,
								PARAM_PLAY,
								PARAM_STOP,
								PARAM_BPM,
								PARAM_SYNTH,
								PARAM_PATTERN,
								PARAM_JUMP_UP,
								PARAM_JUMP_DOWN,
								PARAM_OCTAVE_UP,
								PARAM_OCTAVE_DOWN,
								PARAM_EDIT_SAVE,
								ENUMS(PARAM_EDIT, 9),
								ENUMS(PARAM_MODE, 3),
								ENUMS(PARAM_VIEW, 5),
								PARAM_COUNT
	};
	enum InputIds {
								INPUT_COUNT
	};
	enum OutputIds {
								OUTPUT_CLOCK,
								ENUMS(OUTPUT_CV, 8),
								ENUMS(OUTPUT_GATE, 8),
								ENUMS(OUTPUT_VELO, 8),
								OUTPUT_COUNT
	};
	enum LightIds {
								LIGHT_FOCUS,
								LIGHT_PLAY,
								ENUMS(LIGHT_MODE, 3),
								ENUMS(LIGHT_VIEW, 5),
								LIGHT_COUNT
	};

	dsp::TTimer<float>			clock_timer;
	float						clock_time;
	float						clock_time_p;

	Tracker();

	void	onAdd(const AddEvent &e) override;
	void	onSave(const SaveEvent &e) override;
	void	process(const ProcessArgs& args) override;
};

struct TrackerDisplay : LedDisplay {
	Tracker*					module;
	ModuleWidget*				moduleWidget;
	std::string					font_path;

	TrackerDisplay();

	void drawLayer(const DrawArgs& args, int layer) override;
	void draw_pattern(const DrawArgs& args, Rect rect);
	void draw_timeline(const DrawArgs& args, Rect rect);
};

struct TrackerInfoDisplay : LedDisplay {
	Tracker*					module;
	ModuleWidget*				moduleWidget;
	std::string					font_path;
	char						str_bpm[4];

	TrackerInfoDisplay();

	void drawLayer(const DrawArgs& args, int layer) override;
};

struct TrackerEditDisplay : LedDisplay {
	Tracker*					module;
	ModuleWidget*				moduleWidget;
	std::string					font_path;
	char						string[16];

	TrackerEditDisplay();

	void drawLayer(const DrawArgs& args, int layer) override;
};


struct TrackerWidget : ModuleWidget {
	Tracker						*module;

	TrackerWidget(Tracker* _module);

	void onSelectKey(const SelectKeyEvent &e) override;
	void onHoverScroll(const HoverScrollEvent &e) override;
	void onSelect(const SelectEvent &e) override;
	void onDeselect(const DeselectEvent &e) override;

	//void onDragStart(const DragStartEvent& e) override;
	//void onDragMove(const DragMoveEvent& e) override;
	//void onDragEnd(const DragEndEvent& e) override;
};

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void itoaw(char *str, int number, int width);
bool endian_native(void);

////////////////////////////////////////////////////////////////////////////////
/// GLOBAL STRUCTURES
////////////////////////////////////////////////////////////////////////////////

extern Timeline	g_timeline;
extern Editor	g_editor;

#endif
