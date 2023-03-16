
#include "plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// GLOBAL CONSTANTS
////////////////////////////////////////////////////////////////////////////////

char	table_pitch[12][3] = {
	"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"
};
char	table_effect[] = "APOMSVTFfCcR";
char	table_hex[] = "0123456789ABCDEF";
int		table_row_note_width[] = {
	2,		// Pitch	(2)
	1,		// Octave	(1)
	2,		// Velocity	(2)
	2,		// Panning	(2)
	2,		// Synth	(2)
	2,		// Delay	(2)
	2,		// Glide	(2)
	1, 2,	// Effect 1	(3)
	1, 2,	// Effect 2	(3)
	1, 2,	// Effect 3	(3)
	1, 2,	// Effect 4	(3)
	1, 2,	// Effect 5	(3)
	1, 2,	// Effect 6	(3)
	1, 2,	// Effect 7	(3)
	1, 2	// Effect 8	(3)
};
int		table_row_note_pos[] = {
	0,		// Pitch	(2)
	2,		// Octave	(1)
	3,		// Velocity	(2)
	5,		// Panning	(2)
	7,		// Synth	(2)
	9,		// Delay	(2)
	11,		// Glide	(2)
	13, 14,	// Effect 1	(3)
	16,	17,	// Effect 2	(3)
	19,	20,	// Effect 3	(3)
	22,	23,	// Effect 4	(3)
	25,	26,	// Effect 5	(3)
	28,	29,	// Effect 6	(3)
	30,	31,	// Effect 7	(3)
	33,	34	// Effect 8	(3)
};
int		table_row_cv_width[] = {
	2,	// Value	(2)
	2,	// Curve	(2)
	2	// Delay	(2)
};
int		table_row_cv_pos[] = {
	0,	// Value	(2)
	2,	// Curve	(2)
	4	// Delay	(2)
};
int		table_keyboard[128];

////////////////////////////////////////////////////////////////////////////////
/// GLOBAL STRUCTURES
////////////////////////////////////////////////////////////////////////////////

Timeline		g_timeline;
Editor			g_editor;
