
#pragma once
#include <rack.hpp>

using namespace		rack;
using				std::vector;
using				std::list;
using				std::string;
using				std::unique_ptr;

#include "types.hpp"
#include "components.hpp"
#include "Tracker/Tracker.hpp"
#include "TrackerSynth/TrackerSynth.hpp"
//#include "TrackerDrum/TrackerDrum.hpp"

#include "Regex/Regex.hpp"

extern char		table_pitch[12][3];
extern char		table_effect[14];
extern char		table_hex[17];
extern int		table_keyboard[128];
extern float	table_temp_equal[12];
extern float	table_temp_just[12];
extern float	table_temp_pyth[12];
extern float	table_temp_carlos_super_just[12];
extern float	table_temp_carlos_harmonic[12];
extern float	table_temp_kirnberger[12];
extern float	table_temp_vallotti_young[12];
extern float	table_temp_werckmeister_3[12];
extern NVGcolor	colors[16];
extern NVGcolor	colors_user[8];

// Declare the Plugin, defined in plugin.cpp
extern Plugin*		pluginInstance;

// Declare each Model, defined in each module source file
extern Model*		modelTracker;
extern Model*		modelTrackerSynth;
//extern Model*		modelTrackerDrum;
//extern Model*		modelTrackerClock;
//extern Model*		modelTrackerPhase;
//extern Model*		modelTrackerState;
//extern Model*		modelTrackerTemp;

extern Model*		modelRegex;
extern Model*		modelRegexCondensed;
