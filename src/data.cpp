
#include "plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// GLOBAL CONSTANTS
////////////////////////////////////////////////////////////////////////////////

char	table_pitch[12][3] = {
	"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"
};
char	table_effect[] = "APDOMSVTFfCcR";
char	table_hex[] = "0123456789ABCDEF";
int		table_keyboard[128];

float	table_temp_equal[] = {
	0.0,
	100.0,
	200.0,
	300.0,
	400.0,
	500.0,
	600.0,
	700.0,
	800.0,
	900.0,
	1000.0,
	1100.0
};
float	table_temp_just[] = {
	// http://www.microtonal-synthesis.com/scale_carlos_super_just.html
	0.0,
	111.7313,
	203.9100,
	315.6414,
	386.3139,
	498.0452,
	582.5125,
	701.9553,
	813.6866,
	884.3591,
	1017.596,
	1088.269
};
float	table_temp_pyth[] = {
	// http://www.microtonal-synthesis.com/scale_pythagorean.html
	0.0,
	113.6850,
	203.9100,
	294.1351,
	407.8201,
	498.0452,
	611.7302,
	701.9553,
	815.6403,
	905.8654,
	996.0905,
	1109.775
};
float	table_temp_carlos_super_just[] = {
	// http://www.microtonal-synthesis.com/scale_carlos_super_just.html
	0.0,
	104.9554,
	203.9100,
	315.6414,
	386.3139,
	498.0452,
	551.3181,
	701.9553,
	840.5280,
	884.3591,
	968.8264,
	1088.269
};
float	table_temp_carlos_harmonic[] = {
	// http://www.microtonal-synthesis.com/scale_carlos_harmonic.html,
	0.0,
	104.9554,
	203.9100,
	297.5131,
	386.3139,
	470.7811,
	551.3181,
	701.9553,
	840.5280,
	905.8654,
	968.8264,
	1088.269
};
float	table_temp_kirnberger[] = {
	// http://www.microtonal-synthesis.com/scale_kirnberger.html
	0.0,
	90.225,
	193.849,
	294.135,
	386.314,
	498.045,
	590.224,
	696.663,
	792.180,
	889.650,
	996.091,
	1088.269
};
float	table_temp_vallotti_young[] = {
	// http://www.microtonal-synthesis.com/scale_vallotti_young.html
	0.0,
	94.135,
	196.090,
	298.045,
	392.180,
	501.955,
	592.180,
	698.045,
	796.090,
	894.135,
	1000.000,
	1090.225
};
float	table_temp_werckmeister[] = {
	// http://www.microtonal-synthesis.com/scale_werckmeisterIII.html
	0.0,
	90.225,
	192.180,
	294.135,
	390.225,
	498.045,
	588.270,
	696.090,
	792.180,
	888.270,
	996.091,
	1092.180
};

NVGcolor	colors[16] = {
	{0x1A / 255.0, 0x1C / 255.0, 0x2C / 255.0, 0xFF / 255.0},
	{0x5D / 255.0, 0x27 / 255.0, 0x5D / 255.0, 0xFF / 255.0},
	{0xB1 / 255.0, 0x3E / 255.0, 0x53 / 255.0, 0xFF / 255.0},
	{0xEF / 255.0, 0x7D / 255.0, 0x57 / 255.0, 0xFF / 255.0},
	{0xFF / 255.0, 0xCD / 255.0, 0x75 / 255.0, 0xFF / 255.0},
	{0xAF / 255.0, 0xF0 / 255.0, 0x70 / 255.0, 0xFF / 255.0},
	{0x38 / 255.0, 0xB7 / 255.0, 0x64 / 255.0, 0xFF / 255.0},
	{0x25 / 255.0, 0x71 / 255.0, 0x79 / 255.0, 0xFF / 255.0},
	{0x29 / 255.0, 0x36 / 255.0, 0x6F / 255.0, 0xFF / 255.0},
	{0x3B / 255.0, 0x5D / 255.0, 0xC9 / 255.0, 0xFF / 255.0},
	{0x41 / 255.0, 0xA6 / 255.0, 0xF6 / 255.0, 0xFF / 255.0},
	{0x73 / 255.0, 0xEF / 255.0, 0xF7 / 255.0, 0xFF / 255.0},
	{0xF4 / 255.0, 0xF4 / 255.0, 0xF4 / 255.0, 0xFF / 255.0},
	{0x94 / 255.0, 0xB0 / 255.0, 0xC2 / 255.0, 0xFF / 255.0},
	{0x56 / 255.0, 0x6C / 255.0, 0x86 / 255.0, 0xFF / 255.0},
	{0x33 / 255.0, 0x3C / 255.0, 0x57 / 255.0, 0xFF / 255.0}
};

////////////////////////////////////////////////////////////////////////////////
/// GLOBAL STRUCTURES
////////////////////////////////////////////////////////////////////////////////

Timeline		g_timeline;
Editor			g_editor;
