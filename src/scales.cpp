
#include "plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// GLOBAL CONSTANTS
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
/// SCALES
/// CF: https://newjazz.dk/Compendiums/scales_of_harmonies.pdf
//////////////////////////////////////////////////

float	table_scale_chromatic[] = {
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

//////////////////////////////
/// MAJOR MODES
//////////////////////////////

float	table_scale_ionian[] = {
	0.0,		// T
	0.0,
	200.0,		// T
	200.0,
	400.0,		// ST
	500.0,		// T
	500.0,
	700.0,		// T
	700.0,
	900.0,		// T
	900.0,
	1100.0		// ST
};
float	table_scale_dorian[] = {
	0.0,		// T
	0.0,
	200.0,		// ST
	300.0,		// T
	300.0,
	500.0,		// T
	500.0,
	700.0,		// T
	700.0,
	900.0,		// ST
	1000.0,		// T
	1000.0
};
float	table_scale_phrygian[] = {
	0.0,		// ST
	100.0,		// T
	100.0,
	300.0,		// T
	300.0,
	500.0,		// T
	500.0,
	700.0,		// ST
	800.0,		// T
	800.0,
	900.0,		// T
	900.0
};
float	table_scale_lydian[] = {
	0.0,		// T
	0.0,
	200.0,		// T
	200.0,
	400.0,		// T
	400.0,
	600.0,		// ST
	700.0,		// T
	700.0,
	900.0,		// T
	900.0,
	1100.0		// ST
};
float	table_scale_mixolydian[] = {
	0.0,		// T
	0.0,
	200.0,		// T
	200.0,
	400.0,		// ST
	500.0,		// T
	500.0,
	700.0,		// T
	700.0,
	900.0,		// ST
	1000.0,		// T
	1000.0
};
float	table_scale_aeolian[] = {
	0.0,		// T
	0.0,
	200.0,		// ST
	300.0,		// T
	300.0,
	500.0,		// T
	500.0,
	700.0,		// ST
	800.0,		// T
	800.0,
	1000.0,		// T
	1000.0
};
float	table_scale_locrian[] = {
	0.0,		// ST
	100.0,		// T
	100.0,
	300.0,		// T
	300.0,
	500.0,		// ST
	600.0,		// T
	600.0,
	800.0,		// T
	800.0,
	1000.0,		// T
	1000.0
};

//////////////////////////////
/// MELODIC MINOR MODES
//////////////////////////////

float	table_scale_ionian_s1[] = {
	0.0,		// ST
	100.0,		// T
	100.0,
	300.0,		// ST
	400.0,		// T
	400.0,
	600.0,		// T
	600.0,
	800.0,		// T
	800.0,
	1000.0,		// T
	1000.0
};
float	table_scale_dorian_s7[] = {
	0.0,		// T
	0.0,
	200.0,		// ST
	300.0,		// T
	400.0,
	500.0,		// T
	500.0,
	700.0,		// T
	700.0,
	900.0,		// T
	900.0,
	1100.0
};
float	table_scale_phrygian_s6[] = {
	0.0,		// ST
	100.0,		// T
	100.0,
	300.0,		// T
	300.0,
	500.0,		// T
	500.0,
	700.0,		// T
	700.0,
	900.0,		// ST
	1000.0,		// T
	1000.0
};
float	table_scale_lydian_s5[] = {
	0.0,		// T
	0.0,
	200.0,		// T
	200.0,
	400.0,		// T
	400.0,
	600.0,		// T
	600.0,
	800.0,		// ST
	900.0,		// T
	900.0,
	1000.0		// ST
};
float	table_scale_mixolydian_s4[] = {
	0.0,		// T
	0.0,
	200.0,		// T
	200.0,
	400.0,		// T
	400.0,
	600.0,		// ST
	700.0,		// T
	700.0,
	900.0,		// ST
	1000.0,		// T
	1000.0
};
float	table_scale_aeolian_s3[] = {
	0.0,		// T
	0.0,
	200.0,		// T
	200.0,
	400.0,		// ST
	500.0,		// T
	500.0,
	700.0,		// ST
	800.0,		// T
	800.0,
	1000.0,		// T
	1000.0
};
float	table_scale_locrian_s2[] = {
	0.0,		// T
	0.0,
	200.0,		// ST
	300.0,		// T
	300.0,
	500.0,		// ST
	600.0,		// T
	600.0,
	800.0,		// T
	800.0,
	1000.0,		// T
	1000.0
};

//////////////////////////////
/// HARMONIC MINOR MODES
//////////////////////////////

float	table_scale_ionian_s5[] = {
	0.0,		// T
	0.0,
	200.0,		// T
	200.0,
	400.0,		// ST
	500.0,		// T ST
	500.0,
	800.0,
	800.0,		// ST
	900.0,		// T
	900.0,
	1100.0		// ST
};
float	table_scale_dorian_s4[] = {
	0.0,		// T
	0.0,
	200.0,		// ST
	300.0,		// T ST
	300.0,
	600.0,
	600.0,		// ST
	700.0,		// T
	700.0,
	900.0,		// ST
	1000.0,		// T
	1000.0
};
float	table_scale_phrygian_s3[] = {
	0.0,		// ST
	100.0,		// T ST
	100.0,
	400.0,
	400.0,		// ST
	500.0,		// T
	500.0,
	700.0,		// ST
	800.0,		// T
	800.0,
	1000.0,		// T
	1000.0
};
float	table_scale_lydian_s2[] = {
	0.0,		// T ST
	0.0,
	300.0,
	300.0,		// ST
	400.0,		// T
	400.0,
	600.0,		// ST
	700.0,		// T
	700.0,
	900.0,		// T
	900.0,
	1100.0		// ST
};
float	table_scale_mixolydian_s1[] = {
	0.0,		// ST
	100.0,		// T
	100.0,
	300.0,		// ST
	400.0,		// T
	400.0,
	600.0,		// T
	600.0,
	800.0,		// ST
	900.0,		// T ST
	900.0,
	900.0
};
float	table_scale_aeolian_s7[] = {
	0.0,		// T
	0.0,
	200.0,		// ST
	300.0,		// T
	300.0,
	500.0,		// T
	500.0,
	700.0,		// ST
	800.0,		// T ST
	800.0,
	1100.0,
	1100.0		// ST
};
float	table_scale_locrian_s6[] = {
	0.0,		// ST
	100.0,		// T
	100.0,
	300.0,		// T
	300.0,
	500.0,		// ST
	600.0,		// T ST
	600.0,
	900.0,
	900.0,		// ST
	1000.0,		// T
	1000.0
};

//////////////////////////////
/// HARMONIC MAJOR MODES
//////////////////////////////

float	table_scale_ionian_b6[] = {
	0.0,		// T
	0.0,
	200.0,		// T
	200.0,
	400.0,		// ST		
	500.0,		// T
	500.0,
	700.0,		// ST
	800.0,		// T ST
	800.0,
	1100.0,
	1100.0		// ST
};
float	table_scale_dorian_b5[] = {
	0.0,		// T
	0.0,
	200.0,		// ST
	300.0,		// T
	300.0,
	500.0,		// ST
	600.0,		// T ST
	600.0,
	900.0,
	900.0,		// ST
	1000.0,		// T
	1000.0
};
float	table_scale_phrygian_b4[] = {
	0.0,		// ST
	100.0,		// T
	100.0,
	300.0,		// ST
	400.0,		// T ST
	400.0,
	700.0,
	700.0,		// ST
	800.0,		// T
	800.0,
	1000.0,		// T
	1000.0
};
float	table_scale_lydian_b3[] = {
	0.0,		// T
	0.0,
	200.0,		// ST
	300.0,		// T ST
	300.0,
	600.0,
	600.0,		// ST
	700.0,		// T
	700.0,
	900.0,		// T
	900.0,
	1100.0		// ST
};
float	table_scale_mixolydian_b2[] = {
	0.0,		// ST
	100.0,		// T ST
	100.0,
	400.0,
	400.0,		// ST
	500.0,		// T
	500.0,
	700.0,		// T
	700.0,
	900.0,		// ST
	1000.0,		// T
	1000.0
};
float	table_scale_aeolian_b1[] = {
	0.0,		// T ST
	0.0,
	300.0,
	300.0,		// ST
	400.0,		// T
	400.0,
	600.0,		// T
	600.0,
	800.0,		// ST
	900.0,		// T
	900.0,
	1100.0		// ST
};
float	table_scale_locrian_b7[] = {
	0.0,		// ST
	100.0,		// T
	100.0,
	300.0,		// T
	300.0,
	500.0,		// ST
	600.0,		// T
	600.0,
	800.0,		// ST
	900.0,		// T ST
	900.0,
	0.0
};

//////////////////////////////
/// DIMINISHED MODES
//////////////////////////////

float	table_scale_diminished[] = {
	0.0,		// T
	0.0,
	200.0,		// ST
	300.0,		// T
	300.0,
	500.0,		// ST
	600.0,		// T
	600.0,
	800.0,		// ST
	900.0,		// T
	900.0,
	1100.0		// ST
};
float	table_scale_diminished_inverted[] = {
	0.0,		// ST
	100.0,		// T
	100.0,
	300.0,		// ST
	400.0,		// T
	400.0,
	600.0,		// ST
	700.0,		// T
	700.0,
	900.0,		// ST
	1000.0,		// T
	1000.0
};

//////////////////////////////
/// AUGMENTED MODES
//////////////////////////////

float	table_scale_augmented[] = {
	0.0,		// T ST
	0.0,
	300.0,
	300.0,		// ST
	400.0,		// T ST
	400.0,
	700.0,
	700.0,		// ST
	800.0,		// T ST
	800.0,
	1100.0,
	1100.0		// ST
};
float	table_scale_augmented_inverted[] = {
	0.0,		// ST
	100.0,		// T ST
	100.0,
	400.0,
	400.0,		// ST
	500.0,		// T ST
	500.0,
	800.0,
	800.0,		// ST
	900.0,		// T ST
	900.0,
	0.0
};

//////////////////////////////
/// WHOLE TONE
//////////////////////////////

float	table_scale_whole[] = {
	0.0,		// T
	0.0,
	200.0,		// T
	200.0,
	400.0,		// T
	400.0,
	600.0,		// T
	600.0,
	800.0,		// T
	800.0,
	1000.0,		// T
	1000.0
};

//////////////////////////////////////////////////
/// EDO SCALES
//////////////////////////////////////////////////

float	table_scale_2edo[] = {
	(1200.0 / 3) * 0.0,
	(1200.0 / 3) * 0.0,
	(1200.0 / 3) * 0.0,
	(1200.0 / 3) * 0.0,
	(1200.0 / 3) * 0.0,
	(1200.0 / 3) * 0.0,
	(1200.0 / 3) * 1.0,
	(1200.0 / 3) * 1.0,
	(1200.0 / 3) * 1.0,
	(1200.0 / 3) * 1.0,
	(1200.0 / 3) * 1.0,
	(1200.0 / 3) * 1.0
};

float	table_scale_3edo[] = {
	(1200.0 / 3) * 0.0,
	(1200.0 / 3) * 0.0,
	(1200.0 / 3) * 0.0,
	(1200.0 / 3) * 0.0,
	(1200.0 / 3) * 1.0,
	(1200.0 / 3) * 1.0,
	(1200.0 / 3) * 1.0,
	(1200.0 / 3) * 1.0,
	(1200.0 / 3) * 2.0,
	(1200.0 / 3) * 2.0,
	(1200.0 / 3) * 2.0,
	(1200.0 / 3) * 2.0
};

float	table_scale_4edo[] = {
	(1200.0 / 4) * 0.0,
	(1200.0 / 4) * 0.0,
	(1200.0 / 4) * 0.0,
	(1200.0 / 4) * 1.0,
	(1200.0 / 4) * 1.0,
	(1200.0 / 4) * 1.0,
	(1200.0 / 4) * 2.0,
	(1200.0 / 4) * 2.0,
	(1200.0 / 4) * 2.0,
	(1200.0 / 4) * 3.0,
	(1200.0 / 4) * 3.0,
	(1200.0 / 4) * 3.0
};

float	table_scale_5edo[] = {
	(1200.0 / 5) * 0.0,
	(1200.0 / 5) * 0.0,
	(1200.0 / 5) * 1.0,
	(1200.0 / 5) * 1.0,
	(1200.0 / 5) * 2.0,
	(1200.0 / 5) * 2.0,
	(1200.0 / 5) * 3.0,
	(1200.0 / 5) * 3.0,
	(1200.0 / 5) * 3.0,
	(1200.0 / 5) * 4.0,
	(1200.0 / 5) * 4.0,
	(1200.0 / 5) * 4.0
};

float	table_scale_6edo[] = {
	(1200.0 / 6) * 0.0,
	(1200.0 / 6) * 0.0,
	(1200.0 / 6) * 1.0,
	(1200.0 / 6) * 1.0,
	(1200.0 / 6) * 2.0,
	(1200.0 / 6) * 2.0,
	(1200.0 / 6) * 3.0,
	(1200.0 / 6) * 3.0,
	(1200.0 / 6) * 4.0,
	(1200.0 / 6) * 4.0,
	(1200.0 / 6) * 5.0,
	(1200.0 / 6) * 5.0
};

float	table_scale_7edo[] = {
	(1200.0 / 7) * 0.0,
	(1200.0 / 7) * 0.0,
	(1200.0 / 7) * 1.0,
	(1200.0 / 7) * 1.0,
	(1200.0 / 7) * 2.0,
	(1200.0 / 7) * 3.0,
	(1200.0 / 7) * 3.0,
	(1200.0 / 7) * 4.0,
	(1200.0 / 7) * 4.0,
	(1200.0 / 7) * 5.0,
	(1200.0 / 7) * 5.0,
	(1200.0 / 7) * 6.0
};

float	table_scale_8edo[] = {
	(1200.0 / 8) * 0.0,
	(1200.0 / 8) * 1.0,
	(1200.0 / 8) * 2.0,
	(1200.0 / 8) * 2.0,
	(1200.0 / 8) * 3.0,
	(1200.0 / 8) * 4.0,
	(1200.0 / 8) * 4.0,
	(1200.0 / 8) * 5.0,
	(1200.0 / 8) * 6.0,
	(1200.0 / 8) * 6.0,
	(1200.0 / 8) * 7.0,
	(1200.0 / 8) * 7.0,
};

float	table_scale_9edo[] = {
	(1200.0 / 9) * 0.0,
	(1200.0 / 9) * 1.0,
	(1200.0 / 9) * 2.0,
	(1200.0 / 9) * 2.0,
	(1200.0 / 9) * 3.0,
	(1200.0 / 9) * 4.0,
	(1200.0 / 9) * 4.0,
	(1200.0 / 9) * 5.0,
	(1200.0 / 9) * 5.0,
	(1200.0 / 9) * 6.0,
	(1200.0 / 9) * 7.0,
	(1200.0 / 9) * 8.0
};

float	table_scale_10edo[] = {
	(1200.0 / 10) * 0.0,
	(1200.0 / 10) * 0.0,
	(1200.0 / 10) * 1.0,
	(1200.0 / 10) * 2.0,
	(1200.0 / 10) * 3.0,
	(1200.0 / 10) * 4.0,
	(1200.0 / 10) * 5.0,
	(1200.0 / 10) * 5.0,
	(1200.0 / 10) * 6.0,
	(1200.0 / 10) * 7.0,
	(1200.0 / 10) * 8.0,
	(1200.0 / 10) * 9.0
};

float	table_scale_11edo[] = {
	(1200.0 / 11) * 0.0,
	(1200.0 / 11) * 0.0,
	(1200.0 / 11) * 1.0,
	(1200.0 / 11) * 2.0,
	(1200.0 / 11) * 3.0,
	(1200.0 / 11) * 4.0,
	(1200.0 / 11) * 5.0,
	(1200.0 / 11) * 6.0,
	(1200.0 / 11) * 7.0,
	(1200.0 / 11) * 8.0,
	(1200.0 / 11) * 9.0,
	(1200.0 / 11) * 10.0
};

//////////////////////////////////////////////////
/// TEMPERAMENTS
//////////////////////////////////////////////////

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
float	table_temp_werckmeister_3[] = {
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
