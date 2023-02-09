#include "plugin.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;

	p->addModel(modelTracker);
	//p->addModel(modelTrackerClockExpander);
	//p->addModel(modelTrackeriOutputExpander);
}
