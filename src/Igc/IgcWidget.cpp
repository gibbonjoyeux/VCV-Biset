
#include "Igc.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

IgcWidget::IgcWidget(Igc* _module) {
	IgcDisplay		*display;
	float			pos_x, pos_y;
	float			speed_x, speed_y;
	float			grain_x, grain_y;
	float			lvl_x, lvl_y;
	float			mode_x, mode_y;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker.svg")));

	display = createWidget<IgcDisplay>(mm2px(Vec(0.0, 60.0)));
	display->box.size = mm2px(Vec(100, 25.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);

	pos_x = 15.0;
	pos_y = 10.0;
	speed_x = 45.0;
	speed_y = 10.0;
	grain_x = 75.0;
	grain_y = 10.0;

	lvl_x = 45.0;
	lvl_y = 40.0;

	mode_x = 5.0;
	mode_y = 35.0;

	///  MODE SWITCH
	addParam(
	/**/ createParamCentered<ButtonTriggerState>(mm2px(Vec(mode_x, mode_y - 1.0)),
	/**/ module,
	/**/ Igc::PARAM_MODE));
	/// ALGO LIGHTS
	addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(mode_x, mode_y + 5.0)),
	/**/ module, Igc::LIGHT_MODE_POS_REL));
	addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(mode_x, mode_y + 10.0)),
	/**/ module, Igc::LIGHT_MODE_POS_ABS));
	addChild(createLightCentered<MediumLight<BlueLight>>(mm2px(Vec(mode_x, mode_y + 15.0)),
	/**/ module, Igc::LIGHT_MODE_SPEED));
	addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(mode_x, mode_y + 20.0)),
	/**/ module, Igc::LIGHT_MODE_GRAIN));

	/// ADD PARAMS
	addParam(
	/**/ createParamCentered<KnobBig>(mm2px(Vec(speed_x, 110)),
	/**/ module,
	/**/ Igc::PARAM_DELAY_TIME));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(pos_x, pos_y)),
	/**/ module,
	/**/ Igc::PARAM_POS));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(pos_x - 5.0, pos_y + 9.0)),
	/**/ module,
	/**/ Igc::INPUT_POS_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(pos_x - 5.0, pos_y + 9.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_POS_MOD_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(pos_x + 5.0, pos_y + 9.0)),
	/**/ module,
	/**/ Igc::INPUT_POS_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(pos_x + 5.0, pos_y + 9.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_POS_MOD_2));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(speed_x, speed_y)),
	/**/ module,
	/**/ Igc::PARAM_SPEED));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(speed_x - 5.0, speed_y + 9.0)),
	/**/ module,
	/**/ Igc::INPUT_SPEED_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(speed_x - 5.0, speed_y + 9.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_SPEED_MOD_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(speed_x + 5.0, speed_y + 9.0)),
	/**/ module,
	/**/ Igc::INPUT_SPEED_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(speed_x + 5.0, speed_y + 9.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_SPEED_MOD_2));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(grain_x, grain_y)),
	/**/ module,
	/**/ Igc::PARAM_GRAIN));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(grain_x - 5.0, grain_y + 9.0)),
	/**/ module,
	/**/ Igc::INPUT_GRAIN_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(grain_x - 5.0, grain_y + 9.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_GRAIN_MOD_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(grain_x + 5.0, grain_y + 9.0)),
	/**/ module,
	/**/ Igc::INPUT_GRAIN_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(grain_x + 5.0, grain_y + 9.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_GRAIN_MOD_2));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(lvl_x, lvl_y)),
	/**/ module,
	/**/ Igc::PARAM_LVL));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(lvl_x - 5.0, lvl_y + 9.0)),
	/**/ module,
	/**/ Igc::INPUT_LVL_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(lvl_x - 5.0, lvl_y + 9.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_LVL_MOD_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(lvl_x + 5.0, lvl_y + 9.0)),
	/**/ module,
	/**/ Igc::INPUT_LVL_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(lvl_x + 5.0, lvl_y + 9.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_LVL_MOD_2));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(5, 90)),
	/**/ module,
	/**/ Igc::INPUT_L));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(5, 100)),
	/**/ module,
	/**/ Igc::INPUT_R));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(90, 90)),
	/**/ module,
	/**/ Igc::OUTPUT_L));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(90, 100)),
	/**/ module,
	/**/ Igc::OUTPUT_R));

}
