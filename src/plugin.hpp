
#pragma once
#include <rack.hpp>

using namespace		rack;
using				std::vector;
using				std::string;
using				std::unique_ptr;

#include "types.hpp"
#include "Tracker.hpp"

// Declare the Plugin, defined in plugin.cpp
extern Plugin*		pluginInstance;

// Declare each Model, defined in each module source file
extern Model*		modelTracker;
//extern Model*		modelTrackerClockExpander;
//extern Model*		modelTrackerOutputExpander;

