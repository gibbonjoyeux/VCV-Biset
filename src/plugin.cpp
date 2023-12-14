#include "plugin.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;

	//p->addModel(modelTracker);
	//p->addModel(modelTrackerSynth);
	//p->addModel(modelTrackerDrum);
	//p->addModel(modelTrackerClock);
	//p->addModel(modelTrackerPhase);
	//p->addModel(modelTrackerQuant);
	//p->addModel(modelTrackerState);

	//p->addModel(modelRegex);
	//p->addModel(modelRegexCondensed);

	//p->addModel(modelSegfault);

	//p->addModel(modelGbu);
	//p->addModel(modelPkm);

	//p->addModel(modelTree);

	p->addModel(modelAcro);
}
