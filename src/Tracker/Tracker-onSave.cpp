
#include <bit>
#include <iostream>
#include <fcntl.h>
#include "Tracker.hpp"

#include <iostream>
#include <fstream>

#define SAVE_MODE_RECORD	0
#define SAVE_MODE_WRITE		1

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
		- Swing	ratio							u8
		- Swing	signature						u8
		- Beat count							u16
		- Note count							u8
		- CV count								u8
		- lpb (lines per beat)					u8
		- Note columns										x N
			- Column muted						u8	NOT HANDLED YET
			- Column general effects
				- Fx velocity					u8	NOT HANDLED YET
				- Fx panning					u8	NOT HANDLED YET
				- Fx delay						u8	NOT HANDLED YET
				- Fx chance						u8	NOT HANDLED YET
				- Fx chance mode				u8	NOT HANDLED YET
				- Fx octave						u8	NOT HANDLED YET
				- Fx octave mode				u8	NOT HANDLED YET
				- Fx pitch						u8	NOT HANDLED YET
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
		- Synth name string						char
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

//////////////////////////////////////////////////
/// FILL BUFFER
//////////////////////////////////////////////////

static void fill_u8(u8 number) {
	/// MODE WRITE
	if (g_timeline->save_mode == SAVE_MODE_WRITE)
		g_timeline->save_buffer[g_timeline->save_cursor] = number;
	/// MODE RECORD & WRITE
	g_timeline->save_cursor += 1;
}

static void fill_u16(u16 number) {
	u8		*bytes;

	/// MODE WRITE
	if (g_timeline->save_mode == SAVE_MODE_WRITE) {
		bytes = (u8*)&number;
		g_timeline->save_buffer[g_timeline->save_cursor] = bytes[0];
		g_timeline->save_buffer[g_timeline->save_cursor + 1] = bytes[1];
	}
	/// MODE RECORD & WRITE
	g_timeline->save_cursor += 2;
}

static void fill_u32(u32 number) {
	u8		*bytes;

	/// MODE WRITE
	if (g_timeline->save_mode == SAVE_MODE_WRITE) {
		bytes = (u8*)&number;
		g_timeline->save_buffer[g_timeline->save_cursor] = bytes[0];
		g_timeline->save_buffer[g_timeline->save_cursor + 1] = bytes[1];
		g_timeline->save_buffer[g_timeline->save_cursor + 2] = bytes[2];
		g_timeline->save_buffer[g_timeline->save_cursor + 3] = bytes[3];
	}
	/// MODE RECORD & WRITE
	g_timeline->save_cursor += 4;
}

static void fill_name(char *name) {
	int		len;

	/// HANDLE LENGTH
	len = strlen(name);
	fill_u8(len);
	/// HANDLE STRING
	//// MODE WRITE
	if (g_timeline->save_mode == SAVE_MODE_WRITE)
		memcpy(&(g_timeline->save_buffer[g_timeline->save_cursor]), name, len);
	//// MODE RECORD & WRITE
	g_timeline->save_cursor += len;
}

static void fill_cursor_save(u8 size) {
	/// MODE WRITE
	if (g_timeline->save_mode == SAVE_MODE_WRITE)
		g_timeline->save_cursor_save = g_timeline->save_cursor;
	/// MODE RECORD & WRITE
	g_timeline->save_cursor += size;
}

static void fill_cursor_count(u8 size, u32 count) {
	u32		cursor;

	/// MODE WRITE
	if (g_timeline->save_mode == SAVE_MODE_WRITE) {
		cursor = g_timeline->save_cursor;
		g_timeline->save_cursor = g_timeline->save_cursor_save;
		if (size == 1)
			fill_u8(count);
		else if (size == 2)
			fill_u16(count);
		else
			fill_u32(count);
		g_timeline->save_cursor = cursor;
	}
}

static void fill_save_buffer() {
	list<PatternInstance>::iterator	it, it_end;
	PatternSource					*pattern;
	PatternNoteCol					*note_col;
	PatternNote						*note;
	PatternCVCol					*cv_col;
	PatternCV						*cv;
	int								i, j, k, l;
	u32								count;

	if (g_timeline->save_mode == SAVE_MODE_WRITE
	&& g_timeline->save_buffer == NULL)
		return;

	/// [1] ADD BASICS
	g_timeline->save_cursor = 0;
	fill_u8(0);								// File version
	fill_u8(endian_native());				// File endian
	fill_u32(g_timeline->save_length);		// File size

	/// [2] ADD PATTERNS
	fill_u16(g_timeline->pattern_count);
	for (i = 0; i < g_timeline->pattern_count; ++i) {
		pattern = &(g_timeline->patterns[i]);
		fill_name(pattern->name);			// Name
		fill_u8(pattern->color);			// Color
		fill_u8(0);							// Swing ratio
		fill_u8(0);							// Swing signature
		fill_u16(pattern->beat_count);		// Beat count
		fill_u8(pattern->note_count);		// Note count
		fill_u8(pattern->cv_count);			// CV count
		fill_u8(pattern->lpb);				// lpb (lines per beat)
		/// PATTERN NOTE COLUMNS
		for (j = 0; j < pattern->note_count; ++j) {
			note_col = pattern->notes[j];
			fill_u8(0);							// Muted
			fill_u8(0);							// Fx velocity
			fill_u8(0);							// Fx panning
			fill_u8(0);							// Fx delay
			fill_u8(0);							// Fx chance
			fill_u8(0);							// Fx chance mode
			fill_u8(0);							// Fx octave
			fill_u8(0);							// Fx octave mode
			fill_u8(0);							// Fx pitch
			fill_u8(note_col->fx_count);		// Effect count
			fill_cursor_save(sizeof(u16));			// -> Prepare set lines count
			count = 0;
			for (k = 0; k < pattern->line_count; ++k) {
				note = &(note_col->lines[k]);
				if (note->mode == PATTERN_NOTE_NEW) {
					fill_u16(k);						// Line number
					fill_u8(note->mode);				// Note mode
					fill_u8(note->pitch);				// Note pitch
					fill_u8(note->velocity);			// Note velocity
					fill_u8(note->panning);				// Note panning
					fill_u8(note->synth);				// Note synth
					fill_u8(note->delay);				// Note delay
					fill_u8(note->glide);				// Note glide
					for (l = 0; l < note_col->fx_count; ++l) {
						fill_u8(note->effects[l].type);	// Effect type
						fill_u8(note->effects[l].value);// Effect value
					}
					count += 1;
				} else if (note->mode == PATTERN_NOTE_STOP) {
					fill_u16(k);						// Line number
					fill_u8(note->mode);				// Note mode
					fill_u8(note->delay);				// Note delay
					count += 1;
				} else if (note->mode == PATTERN_NOTE_GLIDE) {
					fill_u16(k);						// Line number
					fill_u8(note->mode);				// Note mode
					fill_u8(note->pitch);				// Note pitch
					fill_u8(note->velocity);			// Note velocity
					fill_u8(note->panning);				// Note panning
					fill_u8(note->glide);				// Note glide
					for (l = 0; l < note_col->fx_count; ++l) {
						fill_u8(note->effects[l].type);	// Effect type
						fill_u8(note->effects[l].value);// Effect value
					}
					count += 1;
				}
			}
			fill_cursor_count(sizeof(u16), count);	// -> Fill set lines count
		}
		/// PATTERN CV COLUMNS
		for (j = 0; j < pattern->cv_count; ++j) {
			cv_col = pattern->cvs[j];
			fill_u8(cv_col->mode);						// Column mode (cv / bpm)
			fill_u8(cv_col->synth);						// Column synth
			fill_u8(cv_col->channel);					// Column channel
			fill_cursor_save(sizeof(u16));				// -> Prepare set lines count
			count = 0;
			for (k = 0; k < pattern->line_count; ++k) {
				cv = &(cv_col->lines[k]);
				if (cv->mode == PATTERN_CV_SET) {
					fill_u16(k);						// Line number
					fill_u8(cv->mode);					// CV mode
					fill_u16(cv->value);				// CV value
					fill_u8(cv->delay);					// CV delay
					fill_u8(cv->curve);					// CV curve
					count += 1;
				}
			}
			fill_cursor_count(sizeof(u16), count);		// -> Fill set lines count
		}
	}

	/// [3] ADD SYNTHS
	fill_u8(g_timeline->synth_count);
	for (i = 0; i < g_timeline->synth_count; ++i) {
		fill_name(g_timeline->synths[i].name + 5);		// Name
		fill_u8(g_timeline->synths[i].color);			// Color
		fill_u8(g_timeline->synths[i].mode);			// Mode (gate / trigger / drum)
		fill_u8(g_timeline->synths[i].channel_count);	// Channel count
	}

	/// [4] ADD TIMELINE
	fill_cursor_save(sizeof(u16));			// -> Prepare set instances count
	count = 0;
	for (i = 0; i < 32; ++i) {
		it = g_timeline->timeline[i].begin();
		it_end = g_timeline->timeline[i].end();
		while (it != it_end) {
			j = ((intptr_t)it->source - (intptr_t)g_timeline->patterns)
			/**/ / sizeof(PatternSource);
			fill_u8(it->row);				// Instance row
			fill_u16(it->beat);				// Instance beat
			fill_u16(j);					// Instance pattern source index
			fill_u16(it->beat_start);		// Instance beat start
			fill_u16(it->beat_length);		// Instance beat length
			fill_u8(it->muted);				// Instance muted
			it = std::next(it);
			count += 1;
		}
	}
	fill_cursor_count(sizeof(u16), count);	// -> Fill set instances count

	/// [5] ADD MATRIX
	fill_u16(0);
}

//////////////////////////////////////////////////
/// INIT BUFFER
//////////////////////////////////////////////////

static void init_save_buffer() {
	u8				*buffer;
	u32				size;

	/// [1] COMPUTE BUFFER SIZE
	g_timeline->save_mode = SAVE_MODE_RECORD;
	fill_save_buffer();
	size = g_timeline->save_cursor;
	g_timeline->save_mode = SAVE_MODE_WRITE;

	/// [2] ALLOC BUFFER
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
}

//////////////////////////////////////////////////
/// WRITE BUFFER
//////////////////////////////////////////////////

static void write_save_buffer() {
	std::string		path;
	std::ofstream	file;

	/// COMPUTE PATH
	path = system::join(
	/**/ g_module->createPatchStorageDirectory(),
	/**/ "save.btr");
	/// OPEN FILE
	file.open(path, std::ios::out | std::ios::binary | std::ios::trunc);
	/// WRITE FILE
	if (file.is_open()) {
		file.write((const char*)g_timeline->save_buffer, g_timeline->save_length);
		file.close();
	}
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void Tracker::onSave(const SaveEvent &e) {

	if (g_module != this)
		return;

	/// [1] WAIT FOR THREAD FLAG
	while (g_timeline->thread_flag.test_and_set()) {}

	/// [2] INIT SAVE BUFFER
	init_save_buffer();
	/// [3] FILL SAVE BUFFER
	fill_save_buffer();
	/// [4] WRITE SAVE BUFFER
	write_save_buffer();

	/// [5] CLEAR THREAD FLAG
	g_timeline->thread_flag.clear();
}

json_t *Tracker::dataToJson(void) {
	json_t		*root;

	if (g_module != this)
		return NULL;

	/// SAVE MIDI (DRIVER + DEVICE + CHANNEL)
	root = json_object();
	json_object_set_new(root, "midi", this->midi_input.toJson());
	/// SAVE EDITOR (JUMP + OCTAVE)
	json_object_set_new(root,
	/**/ "editor_jump", json_integer(g_editor->pattern_jump));
	json_object_set_new(root,
	/**/ "editor_octave", json_integer(g_editor->pattern_octave));
	return root;
}
