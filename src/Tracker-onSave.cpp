
#include "plugin.hpp"

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
	u32				size;
	int				i, j, k;

	if (g_timeline.save_to_change == false)
		return;
	/// COMPUTE BUFFER SIZE
	//// BASICS
	size = sizeof(u8)		// Saving endian
	/**/ + sizeof(u8)		// Active synth
	/**/ + sizeof(u8)		// Active pattern
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
				size += sizeof(u16)		// Line number
				/**/ + sizeof(u8)		// Line column
				/**/ + sizeof(u8)		// Cell mode
				/**/ + sizeof(u8)		// Cell pattern
				/**/ + sizeof(u16);		// Cell beat
			} else if (g_timeline.timeline[i][j].mode == TIMELINE_CELL_STOP) {
				size += sizeof(u16)		// Line number
				/**/ + sizeof(u8)		// Line column
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
		}
	}

	/// FREE BUFFER
	//if (g_timeline.save_buffer != NULL)
	//	free(g_timeline.save_buffer);
	///// CREATE BUFFER
	//g_timeline.save_buffer = (u8*)malloc(size);
	//g_timeline.save_length = size;
	//g_timeline.save_to_change = false;
}

//////////////////////////////////////////////////
/// FILL BUFFER
//////////////////////////////////////////////////

static void fill_save_buffer() {
	if (g_timeline.save_buffer == NULL)
		return;

	// std::endian::big
	// std::endian::little
	// std::endian::native
}

//////////////////////////////////////////////////
/// WRITE BUFFER
//////////////////////////////////////////////////

static void write_save_buffer() {
	if (g_timeline.save_buffer == NULL)
		return;
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/*
- Saving endian									u8
- Editor
	- Active synth								u8
	- Active pattern							u8 / u16
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
		- Line number							u16
		- Line column							u8
		- Cell mode								u8
		- ? Mode NEW
			- Cell pattern						u8 / u16
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
				- Note synth
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
			- CV value							u8 / u16
			- CV delay							u8
			- CV curve							u8
- Synths													x 64
	- Synth channel count						u8
*/

void Tracker::onSave(const SaveEvent &e) {
	/// [1] INIT SAVE BUFFER
	init_save_buffer();
	/// [2] FILL SAVE BUFFER
	fill_save_buffer();
	/// [3] WRITE SAVE BUFFER
	write_save_buffer();
}
