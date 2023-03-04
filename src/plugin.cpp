#include "plugin.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;

	p->addModel(modelTracker);
	p->addModel(modelTrackerOut);
	//p->addModel(modelTrackerClock);
}
