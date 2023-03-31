
#include <fcntl.h>
#include <unistd.h>
#include "plugin.hpp"

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
		- Line (beat) column					u8
		- Line (beat) number					u16
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

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

//static void init_editition(void) {
//	g_editor.set_synth(this->params[PARAM_SYNTH].getValue(), false);
//	g_editor.set_pattern(this->params[PARAM_PATTERN].getValue(), false);
//	g_editor.set_song_length(g_timeline.beat_count, true);
//}

//static u8 load_u8(fd) {
//	u8		value;
//
//	read(fd, &value, sizeof(u8));
//	return value;
//}
//
//static u16 load_u16(fd) {
//	u16		value;
//
//	read(fd, &value, sizeof(u16));
//	if (g_editor.save_endian_reverse) {
//		return ((value << 8)
//		/**/ | ((value >> 8) & 0x00ff));
//	}
//	return value;
//}
//
//static u32 load_u32(fd) {
//	u32		value;
//
//	read(fd, &value, sizeof(u32));
//	if (g_editor.save_endian_reverse) {
//		return ((value << 24)
//		/**/ | ((value <<  8) & 0x00ff0000)
//		/**/ | ((value >>  8) & 0x0000ff00)
//		/**/ | ((value >> 24) & 0x000000ff));
//	}
//	return value;
//}

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
	g_timeline.save_to_change = false;
	if (buffer == NULL)
		return false;
	/// [5] LOAD FILE INTO BUFFER
	read(fd, g_timeline.save_buffer + 1 + 4, size - 1 - 4);
	close(fd);
	return true;
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void Tracker::onAdd(const AddEvent &e) {
	/// [1] INIT EDITION
	//init_edition();

	/// LOAD FILE
	if (load_save_file()) {
	/// EMPTY FILE
	} else {
	}
}

