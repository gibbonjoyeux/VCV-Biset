
#include "Tree.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TreeWidget::TreeWidget(Tree* _module) {
	TreeDisplay	*display;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-State.svg")));

	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85, 103.75)),
	/**/ module,
	/**/ Tree::OUTPUT_MAIN));

	display = createWidget<TreeDisplay>(mm2px(Vec(5.0, 5.0)));
	display->box.size = mm2px(Vec(46, 88.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);
}
