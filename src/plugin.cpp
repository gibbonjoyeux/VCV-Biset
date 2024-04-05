#include "plugin.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;

	p->addModel(modelTracker);
	p->addModel(modelTrackerSynth);
	p->addModel(modelTrackerDrum);
	p->addModel(modelTrackerClock);
	p->addModel(modelTrackerPhase);
	p->addModel(modelTrackerQuant);
	p->addModel(modelTrackerState);
	p->addModel(modelTrackerControl);

	p->addModel(modelRegex);
	p->addModel(modelRegexCondensed);
	p->addModel(modelRegexExp);

	p->addModel(modelTree);
	p->addModel(modelTreeSeed);

	p->addModel(modelGbu);
	p->addModel(modelPkm);

	p->addModel(modelIgc);
	p->addModel(modelOmega3);
	p->addModel(modelOmega6);

	p->addModel(modelSegfault);
	p->addModel(modelBlank);
}
