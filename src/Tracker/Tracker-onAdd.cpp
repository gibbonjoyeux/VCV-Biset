
#include <fcntl.h>
#include <unistd.h>
#include "Tracker.hpp"

#define ENDIAN_16(x)	if (g_timeline->save_endian_reverse) {	\
							x = ((x << 8)						\
							| ((x >> 8) & 0x00ff));				\
						}

#define ENDIAN_32(x)	if (g_timeline->save_endian_reverse) {	\
							x = ((x << 24)						\
							| ((x <<  8) & 0x00ff0000)			\
							| ((x >>  8) & 0x0000ff00)			\
							| ((x >> 24) & 0x000000ff));		\
						}

/*

TRACKER BINARY SAVE FORMAT:
- File version									u8
- File endian									u8
- File size										u32
- Patterns
	- Pattern count								u16
	- Patterns
		- Name length							u8
		- Name string							chars
		- Color									u8
		- Swing ratio							u8
		- Swing signature						u8
		- Beat count							u16
		- Note count							u8
		- CV count								u8
		- lpb (lines per beat)					u8
		- Note columns										x N
			- Column muted						u8	NOT HANDLE YET
			- Column general effects
				- Fx velocity					u8	NOT HANDLE YET
				- Fx panning					u8	NOT HANDLE YET
				- Fx delay						u8	NOT HANDLE YET
				- Fx chance						u8	NOT HANDLE YET
				- Fx chance mode				u8	NOT HANDLE YET
				- Fx octave						u8	NOT HANDLE YET
				- Fx octave mode				u8	NOT HANDLE YET
				- Fx pitch						u8	NOT HANDLE YET
			- Column effect count				u8
			- Column set lines count			u16
			- Lines (only set lines)
				- Line number					u16
				- Note mode						i8
				- ? Mode NEW
					- Note pitch				u8
					- Note velocity				u8
					- Note panning				u8
					- Note synth				u8
					- Note delay				u8
					- Note glide				u8
					- Note effects							x N
						- Effect type			u8
						- Effect value			u8
				- ? Mode STOP
					- Note delay				u8
				- ? Mode GLIDE
					- Note pitch				u8
					- Note velocity				u8
					- Note panning				u8
					- Note glide				u8
					- Note effects							x N
						- Effect type			u8
						- Effect value			u8
		- CV columns										x N
			- Column mode						u8
			- Column synth						u8
			- Column channel					u8
			- Column set lines count			u16
			- Lines (only set lines)
				- Line number					u16
				- CV mode						u8
				- CV value						u16
				- CV delay						u8
				- CV curve						u8
- Synths
	- Synth count								u8
	- Synths
		- Synth name length						u8
		- Synth name string						chars
		- Synth color							u8
		- Synth mode							u8
		- Synth channel count					u8
- Timeline
	- Instance count							u16
	- Instances
		- Instance row							u8
		- Instance beat							u16
		- Pattern id							u16
		- Instance beat start					u16
		- Instance beat length					u16
		- Muted									u8
- Matrix
	- Cell count								u16
	- Cells
		- Cell column							u8
		- Cell line								u8
		- Cell pattern id						u16
		- Cell mode								u8
		- ? Mode LOOP
		- ? Mode NEXT
		- ? Mode NEXT CIRCULAR
		- ? Mode PREV
		- ? Mode PREV CIRCULAR
		- ? Mode RAND
		- ? Mode XRAND
		- ? Mode GOTO
			- Cell line							u8
		- ? Mode RAND AFTER
			- Cell line							u8
		- ? Mode RAND BEFORE
			- Cell line							u8

*/

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static u8 read_u8(void) {
	u8		value;

	value = g_timeline->save_buffer[g_timeline->save_cursor];
	g_timeline->save_cursor += 1;
	return value;
}

static u16 read_u16(void) {
	u16		value;

	value = *((u16*)&(g_timeline->save_buffer[g_timeline->save_cursor]));
	g_timeline->save_cursor += 2;
	ENDIAN_16(value);
	return value;
}

static void read_name(char *buffer) {
	u8		len;

	/// HANDLE LENGTH
	len = read_u8();
	/// HANDLE STRING
	memcpy(buffer, &(g_timeline->save_buffer[g_timeline->save_cursor]), len);
	buffer[len] = 0;
	g_timeline->save_cursor += len;
}

static bool load_save_file(void) {
	u8			*buffer;
	std::string	path;
	u8			endian;
	u8			version;
	u32			size;
	int			fd;

	/// [1] GET PATH
	path = system::join(
	/**/ g_module->createPatchStorageDirectory(),
	/**/ "save.btr");
	/// [2] OPEN FILE
	fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
		return false;
	/// [3] LOAD FILE VERSION & ENDIAN & SIZE
	read(fd, &version, sizeof(u8));
	read(fd, &endian, sizeof(u8));
	g_timeline->save_endian_reverse = (endian != endian_native());
	read(fd, &size, sizeof(u32));
	ENDIAN_32(size);
	/// [4] ALLOC BUFFER
	//// NEW BUFFER
	if (g_timeline->save_buffer == NULL) {
		buffer = (u8*)malloc(size);
	//// RESIZE BUFFER
	} else if (size != g_timeline->save_length) {
		buffer = (u8*)realloc(g_timeline->save_buffer, size);
		if (buffer == NULL)
			free(g_timeline->save_buffer);
	//// KEEP BUFFER
	} else {
		buffer = g_timeline->save_buffer;
	}
	g_timeline->save_buffer = buffer;
	g_timeline->save_length = size;
	if (buffer == NULL)
		return false;
	/// [5] LOAD FILE INTO BUFFER
	read(fd, g_timeline->save_buffer + 1 + 1 + 4, size - 1 - 1 - 4);
	close(fd);
	return true;
}

static bool compute_save_file(void) {
	char			name[256];
	PatternSource	*pattern;
	PatternNoteCol	*note_col;
	PatternNote		*note;
	PatternCVCol	*cv_col;
	PatternCV		*cv;
	PatternInstance	*instance;
	Synth			*synth;
	vector<int>		ids;
	int				pattern_count;
	int				synth_count;
	int				instance_count;
	int				beat_count;
	int				note_count, cv_count;
	int				lpb;
	int				count;
	int				row, beat;
	int				color;
	int				i, j, k, l;

	g_timeline->save_cursor = 1 + 1 + 4;				// Version + Endian + Size

	/// [1] GET PATTERNS
	pattern_count = read_u16();
	for (i = 0; i < pattern_count; ++i) {
		/// PATTERN BASICS
		read_name(name);								// Name
		color = read_u8();								// Color
		read_u8();										// Swing ratio
		read_u8();										// Swing signature
		/// PATTERN SIZE
		beat_count = read_u16();						// Beat count
		note_count = read_u8();							// Note count
		cv_count = read_u8();							// CV count
		lpb = read_u8();								// lpb
		pattern = g_timeline->pattern_new(note_count, cv_count, beat_count, lpb);
		pattern->color = color;
		pattern->rename(name);
		/// PATTERN NOTES
		for (j = 0; j < note_count; ++j) {
			note_col = pattern->notes[j];
			read_u8();									// Column muted
			read_u8();									// Column fx velocity
			read_u8();									// Column fx panning
			read_u8();									// Column fx delay
			read_u8();									// Column fx chance
			read_u8();									// Column fx chance mode
			read_u8();									// Column fx octave
			read_u8();									// Column fx octave mode
			read_u8();									// Column fx pitch
			note_col->fx_count = read_u8();				// Column effect count
			count = read_u16();							// Set lines count
			for (k = 0; k < count; ++k) {
				note = &(note_col->lines[read_u16()]);	// Line number
				note->mode = read_u8();					// Node mode
				if (note->mode == PATTERN_NOTE_NEW) {
					note->pitch = read_u8();			// Note pitch
					note->velocity = read_u8();			// Note velocity
					note->panning = read_u8();			// Note panning
					note->synth = read_u8();			// Note synth
					note->delay = read_u8();			// Note delay
					note->glide = read_u8();			// Note glide
					for (l = 0; l < note_col->fx_count; ++l) {
						note->effects[l].type = read_u8();	// Note fx type
						note->effects[l].value = read_u8();	// Note fx value
					}
				} else if (note->mode == PATTERN_NOTE_STOP) {
					note->delay = read_u8();			// Note delay
				} else if (note->mode == PATTERN_NOTE_GLIDE) {
					note->pitch = read_u8();			// Note pitch
					note->velocity = read_u8();			// Note velocity
					note->panning = read_u8();			// Note panning
					note->glide = read_u8();			// Note glide
					for (l = 0; l < note_col->fx_count; ++l) {
						note->effects[l].type = read_u8();	// Note fx type
						note->effects[l].value = read_u8();	// Note fx value
					}
				}
			}
		}
		/// PATTERN CVS
		for (j = 0; j < cv_count; ++j) {
			cv_col = pattern->cvs[j];
			cv_col->mode = read_u8();					// Column mode
			cv_col->synth = read_u8();					// Column synth
			cv_col->channel = read_u8();				// Column channel
			count = read_u16();							// Set lines count
			for (k = 0; k < count; ++k) {
				cv = &(cv_col->lines[read_u16()]);		// Line number
				cv->mode = read_u8();					// CV mode
				cv->value = read_u16();					// CV value
				cv->delay = read_u8();					// CV delay
				cv->curve = read_u8();					// CV curve
			}
		}
	}

	/// [2] GET SYNTHS
	synth_count = read_u8();
	for (i = 0; i < synth_count; ++i) {
		synth = g_timeline->synth_new();
		read_name(name);								// Name
		synth->color = read_u8();						// Color
		synth->rename(name);
		synth->mode = read_u8();						// Synth mode
		synth->channel_count = read_u8();				// Synth channel count
	}

	/// [3] GET TIMELINE
	instance_count = read_u16();
	for (i = 0; i < instance_count; ++i) {
		row = read_u8();								// Instance row
		beat = read_u16();								// Instance beat
		pattern = &(g_timeline->patterns[read_u16()]);	// Instance source pattern
		instance = g_timeline->instance_new(pattern, row, beat);
		instance->beat_start = read_u16();				// Instance beat start
		instance->beat_length = read_u16();				// Instance beat length
		instance->muted = read_u8();					// Instance muted
	}

	/// [4] GET MATRIX
	count = read_u16();

	return true;
}

static void load_template(void) {
	PatternInstance	*instance;
	PatternSource	*pattern;

	pattern = g_timeline->pattern_new(2, 0, 8, 4);

	/// FILL PATTERN SOURCE NOTES
	pattern->notes[0]->lines[0].mode = PATTERN_NOTE_NEW;
	pattern->notes[0]->lines[0].synth = 0;
	pattern->notes[0]->lines[0].pitch = 63;
	pattern->notes[0]->lines[0].velocity = 99;
	pattern->notes[0]->lines[0].panning = 50;
	pattern->notes[0]->lines[8].mode = PATTERN_NOTE_NEW;
	pattern->notes[0]->lines[8].synth = 0;
	pattern->notes[0]->lines[8].pitch = 61;
	pattern->notes[0]->lines[8].velocity = 99;
	pattern->notes[0]->lines[8].panning = 50;
	pattern->notes[0]->lines[16].mode = PATTERN_NOTE_NEW;
	pattern->notes[0]->lines[16].synth = 0;
	pattern->notes[0]->lines[16].pitch = 66;
	pattern->notes[0]->lines[16].velocity = 99;
	pattern->notes[0]->lines[16].panning = 50;
	pattern->notes[0]->lines[24].mode = PATTERN_NOTE_NEW;
	pattern->notes[0]->lines[24].synth = 0;
	pattern->notes[0]->lines[24].pitch = 70;
	pattern->notes[0]->lines[24].velocity = 99;
	pattern->notes[0]->lines[24].panning = 50;

	instance = g_timeline->instance_new(pattern, 0, 0);
	instance->beat_start = 0;
	instance->beat_length = pattern->beat_count;
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void Tracker::onAdd(const AddEvent &e) {

	if (g_module != this)
		return;

	/// [1] WAIT FOR THREAD FLAG
	while (g_timeline->thread_flag.test_and_set()) {}

	/// [2] LOAD FILE
	g_timeline->clear();
	if (load_save_file()) {
		if (compute_save_file() == false) {
			load_template();
		}
	/// [3] LOAD TEMPLATE
	} else {
		load_template();
	}

	/// [4] CLEAR THREAD FLAG
	g_timeline->thread_flag.clear();
}

void Tracker::dataFromJson(json_t *root) {
	json_t		*obj;

	if (g_module != this)
		return;

	/// RECOVER MIDI (DRIVER + DEVICE + CHANNEL)
	obj = json_object_get(root, "midi");
	if (obj)
		this->midi_input.fromJson(obj);
	/// RECOVER EDITOR (JUMP + OCTAVE)
	obj = json_object_get(root, "editor_jump");
	if (obj)
		g_editor->pattern_jump = json_integer_value(obj);
	obj = json_object_get(root, "editor_octave");
	if (obj)
		g_editor->pattern_octave = json_integer_value(obj);
}
