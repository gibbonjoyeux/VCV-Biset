
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
	/**/ createInputCentered<Outlet>(mm2px(Vec(5.0, 100.0)),
	/**/ module,
	/**/ Tree::INPUT_TREE_RESET));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(15.0, 100.0)),
	/**/ module,
	/**/ Tree::INPUT_SEQ_RESET));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(25.0, 100.0)),
	/**/ module,
	/**/ Tree::INPUT_SEQ_CLOCK));

	/// ADD OUTPUTS
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(40.0, 105.0)),
	/**/ module,
	/**/ Tree::OUTPUT_X));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(50.0, 105.0)),
	/**/ module,
	/**/ Tree::OUTPUT_Y));

	/// ADD PARAMS
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(5.0, 95.0)),
	/**/ module,
	/**/ Tree::PARAM_BRANCH_ANGLE_VARIATION));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(11.0, 95.0)),
	/**/ module,
	/**/ Tree::PARAM_BRANCH_ANGLE_DIVISION));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(17.0, 95.0)),
	/**/ module,
	/**/ Tree::PARAM_BRANCH_ANGLE_SUN_FORCE));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(23.0, 95.0)),
	/**/ module,
	/**/ Tree::PARAM_BRANCH_DIVISION));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(10.0, 75.0)),
	/**/ module,
	/**/ Tree::PARAM_SEQ_LENGTH));
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(20.0, 75.0)),
	/**/ module,
	/**/ Tree::PARAM_SEQ_OFFSET));
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(30.0, 75.0)),
	/**/ module,
	/**/ Tree::PARAM_SEQ_WIND_INFLUENCE));
}
