
#include <fcntl.h>
#include <unistd.h>
#include "Tracker.hpp"

#define ENDIAN_16(x)	if (g_timeline.save_endian_reverse) {	\
							x = ((x << 8)						\
							| ((x >> 8) & 0x00ff));				\
						}

#define ENDIAN_32(x)	if (g_timeline.save_endian_reverse) {	\
							x = ((x << 24)						\
							| ((x <<  8) & 0x00ff0000)			\
							| ((x >>  8) & 0x0000ff00)			\
							| ((x >> 24) & 0x000000ff));		\
						}

/*

TRACKER BINARY SAVE FORMAT:
- Saving endian									u8
- File size										u32
- Editor
	- Active synth								u8
	- Active pattern							u16
	- Used jump									u8
	- Used octave								u8
	- View modes
		- View velocity							u8
		- View panning							u8
		- View delay							u8
		- View glide							u8
		- View effects							u8
- Timeline
	- Beat count								u16
	- Timeline set lines count					u16
	- Lines (only set lines)
		- Line (beat) column					u8
		- Line (beat) number					u16
		- Cell mode								u8
		- ? Mode NEW
			- Cell pattern						u16
			- Cell beat							u16
		- ? Mode STOP
- Patterns													x 256
	- Beat count								u16
	- Note count								u8
	- CV count									u8
	- lpb (lines per beat)						u8
	- Note columns											x N
		- Column mode (Gate / Trigger / Drum)	u8
		- Column effect count					u8
		- Column set lines count				u16
		- Lines (only set lines)
			- Line number						u16
			- Note mode							i8
			- ? Mode NEW
				- Note pitch					u8
				- Note velocity					u8
				- Note panning					u8
				- Note synth					u8
				- Note delay					u8
				- Note glide					u8
				- Note effects								x N
					- Effect type				u8
					- Effect value				u8
			- ? Mode STOP
				- Note delay					u8
			- ? Mode CHANGE
				- Note velocity					u8
				- Note panning					u8
				- Note delay					u8
			- ? Mode GLIDE
				- Note pitch					u8
				- Note glide					u8
	- CV columns											x N
		- Column mode							u8
		- Column synth							u8
		- Column channel						u8
		- Column set lines count				u16
		- Lines (only set lines)
			- Line number						u16
			- CV mode							u8
			- CV value							u16
			- CV delay							u8
			- CV curve							u8
- Synths													x 64
	- Synth channel count						u8

*/

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static u8 read_u8(void) {
	u8		value;

	value = g_timeline.save_buffer[g_timeline.save_cursor];
	g_timeline.save_cursor += 1;
	return value;
}

static u16 read_u16(void) {
	u16		value;

	value = *((u16*)&(g_timeline.save_buffer[g_timeline.save_cursor]));
	g_timeline.save_cursor += 2;
	ENDIAN_16(value);
	return value;
}

static bool load_save_file(void) {
	u8			*buffer;
	std::string	path;
	u8			endian;
	u32			size;
	int			fd;

	/// [1] GET PATH
	path = system::join(
	/**/ g_editor.module->createPatchStorageDirectory(),
	/**/ "save.btr");
	/// [2] OPEN FILE
	fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
		return false;
	/// [3] LOAD FILE ENDIAN & SIZE
	read(fd, &endian, sizeof(u8));
	g_timeline.save_endian_reverse = (endian != endian_native());
	read(fd, &size, sizeof(u32));
	ENDIAN_32(size);
	/// [4] ALLOC BUFFER
	//// NEW BUFFER
	if (g_timeline.save_buffer == NULL) {
		buffer = (u8*)malloc(size);
	//// RESIZE BUFFER
	} else if (size != g_timeline.save_length) {
		buffer = (u8*)realloc(g_timeline.save_buffer, size);
		if (buffer == NULL)
			free(g_timeline.save_buffer);
	//// KEEP BUFFER
	} else {
		buffer = g_timeline.save_buffer;
	}
	g_timeline.save_buffer = buffer;
	g_timeline.save_length = size;
	if (buffer == NULL)
		return false;
	/// [5] LOAD FILE INTO BUFFER
	read(fd, g_timeline.save_buffer + 1 + 4, size - 1 - 4);
	close(fd);
	return true;
}

static bool compute_save_file(void) {
	TimelineCell	*cell;
	PatternSource	*pattern;
	PatternNoteRow	*note_col;
	PatternNote		*note;
	PatternCVRow	*cv_col;
	PatternCV		*cv;
	int				synth_id, pattern_id;
	int				beat_count;
	int				note_count, cv_count;
	int				lpb;
	int				count;
	int				line, column;
	int				i, j, k, l;

	g_timeline.save_cursor = 1 + 4;
	/// [1] GET ACTIVE SYNTH & PATTERN
	synth_id = read_u8();								// Active synth
	pattern_id = read_u16();							// Active pattern
	/// [2] GET EDITOR BASICS
	g_editor.pattern_jump = read_u8();					// Used jump
	g_editor.pattern_octave = read_u8();				// Used octave
	g_editor.switch_view[0].state = read_u8();			// View velocity
	g_editor.switch_view[1].state = read_u8();			// View panning
	g_editor.switch_view[2].state = read_u8();			// View delay
	g_editor.switch_view[3].state = read_u8();			// View glide
	g_editor.switch_view[4].state = read_u8();			// View effects
	/// [3] GET TIMELINE
	count = read_u16();									// Beat count
	g_timeline.resize(count);
	count = read_u16();									// Set lines count
	for (i = 0; i < count; ++i) {
		column = read_u8();								// Column number
		line = read_u16();								// Line number
		cell = &(g_timeline.timeline[column][line]);
		cell->mode = read_u8();							// Cell mode
		if (cell->mode == TIMELINE_CELL_NEW) {
			cell->pattern = read_u16();					// Cell pattern
			cell->beat = read_u16();					// Cell beat
		}
	}
	/// [4] GET PATTERNS
	for (i = 0; i < 256; ++i) {
		pattern = &(g_timeline.patterns[i]);
		/// PATTERN SIZE
		beat_count = read_u16();						// Beat count
		note_count = read_u8();							// Note count
		cv_count = read_u8();							// CV count
		lpb = read_u8();								// lpb
		pattern->resize(note_count, cv_count, beat_count, lpb);
		/// PATTERN NOTES
		for (j = 0; j < note_count; ++j) {
			note_col = pattern->notes[j];
			note_col->mode = read_u8();					// Column mode
			note_col->effect_count = read_u8();			// Column effect count
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
					for (l = 0; l < note_col->effect_count; ++l) {
						note->effects[l].type = read_u8();
						note->effects[l].value = read_u8();
					}
				} else if (note->mode == PATTERN_NOTE_STOP) {
					note->delay = read_u8();			// Note delay
				} else if (note->mode == PATTERN_NOTE_CHANGE) {
					note->velocity = read_u8();			// Note velocity
					note->panning = read_u8();			// Note panning
					note->delay = read_u8();			// Note delay
				} else if (note->mode == PATTERN_NOTE_GLIDE) {
					note->pitch = read_u8();			// Note pitch
					note->glide = read_u8();			// Note glide
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
				cv->glide = read_u8();					// CV curve
			}
		}
	}
	/// [5] GET SYNTHS
	for (i = 0; i < 64; ++i)
		g_timeline.synths[i].channel_count = read_u8();	// Synth channel count
	/// [6] SET ACTIVE SYNTH & PATTERN
	g_editor.set_synth(synth_id, true);
	g_editor.set_pattern(pattern_id, true);
	g_editor.set_song_length(g_timeline.beat_count, true);
	return true;
}

static void load_template(void) {
	PatternSource	*pattern;

	g_timeline.resize(8);
	g_timeline.timeline[0][0].mode = TIMELINE_CELL_NEW;
	g_timeline.timeline[0][0].pattern = 0;
	g_timeline.timeline[0][0].beat = 0;

	pattern = &(g_timeline.patterns[0]);
	pattern->resize(2, 0, 8, 4);

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

	g_editor.set_synth(0, true);
	g_editor.set_pattern(0, true);
	g_editor.set_song_length(g_timeline.beat_count, true);
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void Tracker::onAdd(const AddEvent &e) {
	/// [1] WAIT FOR THREAD FLAG
	while (g_timeline.thread_flag.test_and_set()) {}

	/// [2] LOAD FILE
	if (load_save_file()) {
		if (compute_save_file() == false) {
			load_template();
		}
	/// [3] LOAD TEMPLATE
	} else {
		load_template();
	}

	/// [4] CLEAR THREAD FLAG
	g_timeline.thread_flag.clear();
}

