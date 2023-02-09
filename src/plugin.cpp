#include "plugin.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;

	p->addModel(modelComposer);
	//p->addModel(modelComposerClockExpander);
	//p->addModel(modelComposeriOutputExpander);
}
