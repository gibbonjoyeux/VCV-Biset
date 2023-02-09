
#pragma once
#include <rack.hpp>

using namespace		rack;

typedef uint8_t		u8;
typedef int8_t		i8;
typedef uint16_t	u16;
typedef int16_t		i16;
typedef uint32_t	u32;
typedef int32_t		i32;

// Declare the Plugin, defined in plugin.cpp
extern Plugin*		pluginInstance;

// Declare each Model, defined in each module source file
extern Model*		modelComposer;
//extern Model*		modelComposerClockExpander;
//extern Model*		modelComposerOutputExpander;

