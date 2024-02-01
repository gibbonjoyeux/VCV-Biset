
#include "River.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

RiverFile::RiverFile(char *name, int type) {
	int		i;

	this->name = name;
	this->type = type;
	/// HANDLE DIRECTORY
	if (type == RIVER_DIR) {
		this->name += "/";
	/// HANDLE FILE
	} else if (type == RIVER_FILE) {
		/// CHECK IF WAVE FILE
		i = 0;
		while (name[i] != 0)
			i += 1;
		if (i > 4
		&& (name[i - 4] == '.')
		&& (name[i - 3] == 'w' || name[i - 3] == 'W')
		&& (name[i - 2] == 'a' || name[i - 2] == 'A')
		&& (name[i - 1] == 'v' || name[i - 1] == 'V'))
			this->type = RIVER_WAV;
	}
}
