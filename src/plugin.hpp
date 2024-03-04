
#pragma once
#include <rack.hpp>

using namespace		rack;
using				simd::float_4;
using				simd::int32_4;
using				std::vector;
using				std::list;
using				std::string;
using				std::unique_ptr;

#include "types.hpp"
#include "components.hpp"
#include "Tracker/Tracker.hpp"
#include "TrackerControl/TrackerControl.hpp"
#include "TrackerSynth/TrackerSynth.hpp"
#include "TrackerDrum/TrackerDrum.hpp"
#include "TrackerClock/TrackerClock.hpp"
#include "TrackerPhase/TrackerPhase.hpp"
#include "TrackerQuant/TrackerQuant.hpp"
#include "TrackerState/TrackerState.hpp"

#include "Regex/Regex.hpp"
#include "RegexExp/RegexExp.hpp"

#include "Tree/Tree.hpp"
#include "TreeSeed/TreeSeed.hpp"

#include "Gbu/Gbu.hpp"
#include "Pkm/Pkm.hpp"

#include "Segfault/Segfault.hpp"
#include "Blank/Blank.hpp"

extern char		table_pitch[12][3];
extern char		table_effect[12];
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
// MAJOR MODES
extern float	table_scale_ionian[12];
extern float	table_scale_dorian[12];
extern float	table_scale_phrygian[12];
extern float	table_scale_lydian[12];
extern float	table_scale_mixolydian[12];
extern float	table_scale_aeolian[12];
extern float	table_scale_locrian[12];
// MELODIC MINOR MODES
extern float	table_scale_ionian_s1[12];
extern float	table_scale_dorian_s7[12];
extern float	table_scale_phrygian_s6[12];
extern float	table_scale_lydian_s5[12];
extern float	table_scale_mixolydian_s4[12];
extern float	table_scale_aeolian_s3[12];
extern float	table_scale_locrian_s2[12];
// HARMONIC MINOR MODES
extern float	table_scale_ionian_s5[12];
extern float	table_scale_dorian_s4[12];
extern float	table_scale_phrygian_s3[12];
extern float	table_scale_lydian_s2[12];
extern float	table_scale_mixolydian_s1[12];
extern float	table_scale_aeolian_s7[12];
extern float	table_scale_locrian_s6[12];
// HARMONIC MAJOR MODES
extern float	table_scale_ionian_b6[12];
extern float	table_scale_dorian_b5[12];
extern float	table_scale_phrygian_b4[12];
extern float	table_scale_lydian_b3[12];
extern float	table_scale_mixolydian_b2[12];
extern float	table_scale_aeolian_b1[12];
extern float	table_scale_locrian_b7[12];
// DIMINISHED MODES
extern float	table_scale_diminished[12];
extern float	table_scale_diminished_inverted[12];
// AUGMENTED MODES
extern float	table_scale_augmented[12];
extern float	table_scale_augmented_inverted[12];
// WHOLE TONE
extern float	table_scale_whole[12];
// EDO
extern float	table_scale_2edo[12];
extern float	table_scale_3edo[12];
extern float	table_scale_4edo[12];
extern float	table_scale_5edo[12];
extern float	table_scale_6edo[12];
extern float	table_scale_7edo[12];
extern float	table_scale_8edo[12];
extern float	table_scale_9edo[12];
extern float	table_scale_10edo[12];
extern float	table_scale_11edo[12];

extern NVGcolor	colors[16];
extern NVGcolor	colors_user[8];

// Declare the Plugin, defined in plugin.cpp
extern Plugin*		pluginInstance;

// Declare each Model, defined in each module source file
extern Model*		modelTracker;
extern Model*		modelTrackerSynth;
extern Model*		modelTrackerDrum;
extern Model*		modelTrackerClock;
extern Model*		modelTrackerPhase;
extern Model*		modelTrackerQuant;
extern Model*		modelTrackerState;
extern Model*		modelTrackerControl;

extern Model*		modelRegex;
extern Model*		modelRegexCondensed;
extern Model*		modelRegexExp;

extern Model*		modelTree;
extern Model*		modelTreeSeed;

extern Model*		modelGbu;
extern Model*		modelPkm;

extern Model*		modelSegfault;
extern Model*		modelBlank;
