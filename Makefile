# If RACK_DIR is not defined when calling the Makefile, default to two directories above
RACK_DIR ?= ../..

# FLAGS will be passed to both the C and C++ compiler
FLAGS +=
CFLAGS +=
CXXFLAGS +=

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS +=

# Add .cpp files to the build
SOURCES += $(wildcard src/*.cpp)

#SOURCES += $(wildcard src/Tracker/*.cpp)
#SOURCES += $(wildcard src/TrackerSynth/*.cpp)
#SOURCES += $(wildcard src/TrackerDrum/*.cpp)
#SOURCES += $(wildcard src/TrackerClock/*.cpp)
#SOURCES += $(wildcard src/TrackerPhase/*.cpp)
#SOURCES += $(wildcard src/TrackerQuant/*.cpp)
#SOURCES += $(wildcard src/TrackerState/*.cpp)
#
#SOURCES += $(wildcard src/Regex/*.cpp)
#SOURCES += $(wildcard src/Segfault/*.cpp)
#
#SOURCES += $(wildcard src/Gbu/*.cpp)
#SOURCES += $(wildcard src/Pkm/*.cpp)
#
#SOURCES += $(wildcard src/Tree/*.cpp)

SOURCES += $(wildcard src/Acro/*.cpp)

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)
DISTRIBUTABLES += $(wildcard presets)

# Include the Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk
