
#include "Igc.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

IgcWidget::IgcWidget(Igc* _module) {
	IgcDisplay		*display;
	float			y_pos;
	float			y_lvl;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tree.svg")));

	display = createWidget<IgcDisplay>(mm2px(Vec(10.0, 10.0)));
	display->box.size = mm2px(Vec(30, 100.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);

	y_pos = 10.0;
	y_lvl = 55.0;

	/// ADD PARAMS
	addParam(
	/**/ createParamCentered<KnobBig>(mm2px(Vec(45, 90)),
	/**/ module,
	/**/ Igc::PARAM_DELAY_TIME));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(5, y_pos)),
	/**/ module,
	/**/ Igc::PARAM_POS));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(5, y_pos + 9.0 + 7.0 * 0.0)),
	/**/ module,
	/**/ Igc::INPUT_POS_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(5, y_pos + 9.0 + 7.0 * 0.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_POS_MOD_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(5, y_pos + 9.0 + 7.0 * 2.0)),
	/**/ module,
	/**/ Igc::INPUT_POS_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(5, y_pos + 9.0 + 7.0 * 2.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_POS_MOD_2));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(5, y_lvl)),
	/**/ module,
	/**/ Igc::PARAM_LVL));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(5, y_lvl + 9.0 + 7.0 * 0)),
	/**/ module,
	/**/ Igc::INPUT_LVL_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(5, y_lvl + 9.0 + 7.0 * 0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_LVL_MOD_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(5, y_lvl + 9.0 + 7.0 * 2)),
	/**/ module,
	/**/ Igc::INPUT_LVL_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(5, y_lvl + 9.0 + 7.0 * 2 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_LVL_MOD_2));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(5, 100)),
	/**/ module,
	/**/ Igc::INPUT_L));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(5, 110)),
	/**/ module,
	/**/ Igc::INPUT_R));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(45, 100)),
	/**/ module,
	/**/ Igc::OUTPUT_L));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(45, 110)),
	/**/ module,
	/**/ Igc::OUTPUT_R));

}
