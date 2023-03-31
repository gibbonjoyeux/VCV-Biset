
#include <bit>
#include <iostream>
#include <fcntl.h>
#include "plugin.hpp"

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
				- Note pitch
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

//////////////////////////////////////////////////
/// INIT BUFFER
//////////////////////////////////////////////////

static void init_save_buffer() {
	PatternSource	*pattern;
	PatternNoteRow	*note_col;
	PatternNote		*note;
	PatternCVRow	*cv_col;
	u8				*buffer;
	u32				size;
	int				i, j, k;

	if (g_timeline.save_to_change == false)
		return;
	/// [1] COMPUTE BUFFER SIZE
	//// BASICS
	size = sizeof(u8)		// Saving endian
	/**/ + sizeof(u32)		// File size
	/**/ + sizeof(u8)		// Active synth
	/**/ + sizeof(u16)		// Active pattern
	/**/ + sizeof(u8)		// Used jump
	/**/ + sizeof(u8)		// Used octave
	/**/ + sizeof(u8)		// View velocity
	/**/ + sizeof(u8)		// View panning
	/**/ + sizeof(u8)		// View delay
	/**/ + sizeof(u8)		// View glide
	/**/ + sizeof(u8);		// View effects
	//// TIMELINE
	size += sizeof(u16)		// Beat count
	/**/ + sizeof(u16);		// Set lines count
	for (i = 0; i < 12; ++i) {
		for (j = 0; j < g_timeline.beat_count; ++j) {
			if (g_timeline.timeline[i][j].mode == TIMELINE_CELL_ADD) {
				size += sizeof(u8)		// Line (beat) column
				/**/ + sizeof(u16)		// Line (beat) number
				/**/ + sizeof(u8)		// Cell mode
				/**/ + sizeof(u16)		// Cell pattern
				/**/ + sizeof(u16);		// Cell beat
			} else if (g_timeline.timeline[i][j].mode == TIMELINE_CELL_STOP) {
				size += sizeof(u8)		// Line (beat) column
				/**/ + sizeof(u16)		// Line (beat) number
				/**/ + sizeof(u8);		// Cell mode
			}
		}
	}
	//// PATTERNS
	for (i = 0; i < 256; ++i) {
		pattern = &(g_timeline.patterns[i]);
		size += sizeof(u16)				// Beat count
		/**/ + sizeof(u8)				// Note count
		/**/ + sizeof(u8)				// CV count
		/**/ + sizeof(u8);				// lpb (lines per beat)
		/// PATTERN NOTE COLUMNS
		for (j = 0; j < pattern->note_count; ++j) {
			note_col = pattern->notes[j];
			size += sizeof(u8)			// Column mode (gate / trigger / drum)
			/**/ + sizeof(u8)			// Effect count
			/**/ + sizeof(u16);			// Set line count
			for (k = 0; k < pattern->line_count; ++k) {
				note = &(note_col->lines[k]);
				if (note->mode == PATTERN_NOTE_NEW) {
					size += sizeof(u16)	// Line number
					/**/ + sizeof(u8)	// Note mode
					/**/ + sizeof(u8)	// Note pitch
					/**/ + sizeof(u8)	// Note velocity
					/**/ + sizeof(u8)	// Note panning
					/**/ + sizeof(u8)	// Note synth
					/**/ + sizeof(u8)	// Note delay
					/**/ + sizeof(u8)	// Note glide
					/**/ + (sizeof(u8)	// Effect type
					/**/ + sizeof(u8))	// Effect value
					/**/ * note_col->effect_count;
				} else if (note->mode == PATTERN_NOTE_STOP) {
					size += sizeof(u16)	// Line number
					/**/ + sizeof(u8)	// Note mode
					/**/ + sizeof(u8);	// Note delay
				} else if (note->mode == PATTERN_NOTE_CHANGE) {
					size += sizeof(u16)	// Line number
					/**/ + sizeof(u8)	// Note mode
					/**/ + sizeof(u8)	// Note velocity
					/**/ + sizeof(u8)	// Note panning
					/**/ + sizeof(u8);	// Note delay
				} else if (note->mode == PATTERN_NOTE_GLIDE) {
					size += sizeof(u16)	// Line number
					/**/ + sizeof(u8)	// Note mode
					/**/ + sizeof(u8)	// Note pitch
					/**/ + sizeof(u8);	// Note glide
				}
			}
		}
		/// PATTERN CV COLUMNS
		for (j = 0; j < pattern->cv_count; ++j) {
			cv_col = pattern->cvs[j];
			size += sizeof(u8)			// Column mode (cv / bpm)
			/**/ + sizeof(u8)			// Column synth
			/**/ + sizeof(u8)			// Column channel
			/**/ + sizeof(u16);			// Set lines count
			for (k = 0; k < pattern->line_count; ++k) {
				if (cv_col->lines[k].mode == PATTERN_CV_SET) {
					size += sizeof(u16)	// Line number
					/**/ + sizeof(u8)	// CV mode
					/**/ + sizeof(u16)	// CV value
					/**/ + sizeof(u8)	// CV delay
					/**/ + sizeof(u8);	// CV curve
				}
			}
		}
	}
	//// SYNTHS
	size += sizeof(u8) * 64;			// Synth channel count

	/// [2] ALLOC BUFFER
	//// NEW BUFFER
	if (g_timeline.save_buffer == NULL) {
		buffer = (u8*)malloc(size);
	//// RESIZE BUFFER
	} else if (size != g_timeline.save_length) {
		buffer = (u8*)realloc(g_timeline.save_buffer, size);
		if (buffer == NULL) {
			free(g_timeline.save_buffer);
			g_timeline.save_buffer = NULL;
		}
	//// KEEP BUFFER
	} else {
		buffer = g_timeline.save_buffer;
	}
	g_timeline.save_buffer = buffer;
	g_timeline.save_length = size;
	g_timeline.save_to_change = false;
}

//////////////////////////////////////////////////
/// FILL BUFFER
//////////////////////////////////////////////////

static void fill_u8(u8 number) {
	g_timeline.save_buffer[g_timeline.save_cursor] = number;
	g_timeline.save_cursor += 1;
}

static void fill_u16(u16 number) {
	u8		*bytes;

	bytes = (u8*)&number;
	g_timeline.save_buffer[g_timeline.save_cursor] = bytes[0];
	g_timeline.save_buffer[g_timeline.save_cursor + 1] = bytes[1];
	g_timeline.save_cursor += 2;
}

static void fill_u32(u32 number) {
	u8		*bytes;

	bytes = (u8*)&number;
	g_timeline.save_buffer[g_timeline.save_cursor] = bytes[0];
	g_timeline.save_buffer[g_timeline.save_cursor + 1] = bytes[1];
	g_timeline.save_buffer[g_timeline.save_cursor + 2] = bytes[2];
	g_timeline.save_buffer[g_timeline.save_cursor + 3] = bytes[3];
	g_timeline.save_cursor += 4;
}

static void fill_cursor_save(u8 size) {
	g_timeline.save_cursor_save = g_timeline.save_cursor;
	g_timeline.save_cursor += size;
}

static void fill_cursor_count(u8 size, u32 count) {
	u32		cursor;

	cursor = g_timeline.save_cursor;
	g_timeline.save_cursor = g_timeline.save_cursor_save;
	if (size == 1)
		fill_u8(count);
	else if (size == 2)
		fill_u16(count);
	else
		fill_u32(count);
	g_timeline.save_cursor = cursor;
}

static void fill_save_buffer() {
	PatternSource	*pattern;
	PatternNoteRow	*note_col;
	PatternNote		*note;
	PatternCVRow	*cv_col;
	PatternCV		*cv;
	TimelineCell	*cell;
	int				i, j, k, l;
	u16				count;

	if (g_timeline.save_buffer == NULL)
		return;
	/// [1] ADD BASICS
	g_timeline.save_cursor = 0;
	fill_u8(endian_native());				// Saving endian
	fill_u32(g_timeline.save_length);		// File size
	fill_u8(g_editor.synth_id);				// Active synth
	fill_u16(g_editor.pattern_id);			// Active pattern
	fill_u8(g_editor.pattern_jump);			// Used jump
	fill_u8(g_editor.pattern_octave);		// Used octave
	fill_u8(g_editor.switch_view[0].state);	// View velocity
	fill_u8(g_editor.switch_view[1].state);	// View panning
	fill_u8(g_editor.switch_view[2].state);	// View delay
	fill_u8(g_editor.switch_view[3].state);	// View glide
	fill_u8(g_editor.switch_view[4].state);	// View effects
	/// [2] ADD TIMELINE
	fill_u16(g_timeline.beat_count);		// Beat count
	fill_cursor_save(sizeof(u32));			// -> Prepare set lines count
	count = 0;
	for (i = 0; i < 12; ++i) {
		for (j = 0; j < g_timeline.beat_count; ++j) {
			cell = &(g_timeline.timeline[i][j]);
			if (cell->mode == TIMELINE_CELL_ADD) {
				fill_u8(i);					// Line (beat) column
				fill_u16(j);				// Line (beat) number
				fill_u8(cell->mode);		// Cell mode
				fill_u16(cell->pattern);	// Cell pattern
				fill_u16(cell->beat);		// Cell beat
				count += 1;
			} else if (cell->mode == TIMELINE_CELL_STOP) {
				fill_u8(i);					// Line (beat) column
				fill_u16(j);				// Line (beat) number
				fill_u8(cell->mode);		// Cell mode
				count += 1;
			}
		}
	}
	fill_cursor_count(sizeof(u32), count);	// -> Fill set lines count
	/// [3] ADD PATTERNS
	for (i = 0; i < 256; ++i) {
		pattern = &(g_timeline.patterns[i]);
		fill_u16(pattern->beat_count);		// Beat count
		fill_u8(pattern->note_count);		// Note count
		fill_u8(pattern->cv_count);			// CV count
		fill_u8(pattern->lpb);				// lpb (lines per beat)
		/// PATTERN NOTE COLUMNS
		for (j = 0; j < pattern->note_count; ++j) {
			note_col = pattern->notes[j];
			fill_u8(note_col->mode);		// Column mode (gate / trigger / drum)
			fill_u8(note_col->effect_count);// Effect count
			fill_cursor_save(sizeof(u16));	// -> Prepare set lines count
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
					for (l = 0; l < note_col->effect_count; ++l) {
						fill_u8(note->effects[l].type);	// Effect type
						fill_u8(note->effects[l].value);// Effect value
					}
				} else if (note->mode == PATTERN_NOTE_STOP) {
					fill_u16(k);						// Line number
					fill_u8(note->mode);				// Note mode
					fill_u8(note->delay);				// Note delay
				} else if (note->mode == PATTERN_NOTE_CHANGE) {
					fill_u16(k);						// Line number
					fill_u8(note->mode);				// Note mode
					fill_u8(note->velocity);			// Note velocity
					fill_u8(note->panning);				// Note panning
					fill_u8(note->delay);				// Note delay
				} else if (note->mode == PATTERN_NOTE_GLIDE) {
					fill_u16(k);						// Line number
					fill_u8(note->mode);				// Note mode
					fill_u8(note->pitch);				// Note pitch
					fill_u8(note->glide);				// Note glide
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
					fill_u8(cv->glide);					// CV curve
				}
			}
			fill_cursor_count(sizeof(u16), count);		// -> Fill set lines count
		}
	}
	/// [4] SYNTHS
	for (i = 0; i < 64; ++i)
		fill_u8(g_timeline.synths[i].channel_count);	// Channel count
}

//////////////////////////////////////////////////
/// WRITE BUFFER
//////////////////////////////////////////////////

static void write_save_buffer() {
	std::string	path;
	int			fd;

	if (g_timeline.save_buffer == NULL)
		return;
	/// [1] GET PATH
	path = system::join(
	/**/ g_editor.module->createPatchStorageDirectory(),
	/**/ "save.btr");
	/// [2] OPEN FILE
	fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
	/// [3] WRITE FILE
	if (fd > 0) {
		write(fd, g_timeline.save_buffer, g_timeline.save_length);
		close(fd);
	}
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void Tracker::onSave(const SaveEvent &e) {
	/// [1] INIT SAVE BUFFER
	init_save_buffer();
	/// [2] FILL SAVE BUFFER
	fill_save_buffer();
	/// [3] WRITE SAVE BUFFER
	write_save_buffer();
}
