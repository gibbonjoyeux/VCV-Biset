
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

// Declare the Plugin, defined in plugin.cpp
extern Plugin*		pluginInstance;

// Declare each Model, defined in each module source file
extern Model*		modelTracker;
extern Model*		modelTrackerSynth;
//extern Model*		modelTrackerDrum;
//extern Model*		modelTrackerClock;

extern Model*		modelRegex;
