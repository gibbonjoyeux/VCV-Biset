
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

	display = createWidget<TreeDisplay>(mm2px(Vec(5.0, 5.0)));
	display->box.size = mm2px(Vec(46, 60.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);

	/// ADD INPUTS
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10.0, 105.0)),
	/**/ module,
	/**/ Tree::INPUT_RESET));

	/// ADD PARAMS
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(10.0, 95.0)),
	/**/ module,
	/**/ Tree::PARAM_BRANCH_ANGLE_VARIATION));
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(20.0, 95.0)),
	/**/ module,
	/**/ Tree::PARAM_BRANCH_ANGLE_DIVISION));
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(30.0, 95.0)),
	/**/ module,
	/**/ Tree::PARAM_BRANCH_ANGLE_SUN_FORCE));
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(40.0, 95.0)),
	/**/ module,
	/**/ Tree::PARAM_BRANCH_DIVISION));
}
