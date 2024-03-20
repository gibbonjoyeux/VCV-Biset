
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
	float			shape_x, shape_y;
	float			mode_x, mode_y;
	float			delay_x, delay_y;
	float			mod_step_x, mod_step_y;
	float			mix_x, mix_y;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Igc.svg")));

	display = createWidget<IgcDisplay>(mm2px(Vec(0.0, 65.0)));
	display->box.size = mm2px(Vec(101.6, 25.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);

	pos_x = 101.6 * 0.2;
	pos_y = 11.5;
	speed_x = 101.6 * 0.5;
	speed_y = 14.5;
	grain_x = 101.6 * 0.8;
	grain_y = 11.5;

	lvl_x = pos_x;
	lvl_y = 45.0;
	shape_x = grain_x;
	shape_y = lvl_y;

	mode_x = 101.6 * 0.5;
	mode_y = 52.0;

	delay_x = speed_x + 20.0;
	delay_y = 101.5;
	mix_x = 33.0;
	mix_y = 99.5;

	mod_step_x = 6.5;
	mod_step_y = 8.0;

	///  MODE SWITCH
	addParam(
	/**/ createParamCentered<ButtonTriggerState>(mm2px(Vec(mode_x, mode_y - 1.0)),
	/**/ module,
	/**/ Igc::PARAM_MODE));
	/// ALGO LIGHTS
	addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(mode_x - 7.5, mode_y + 4.0)),
	/**/ module, Igc::LIGHT_MODE_POS_REL));
	addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(mode_x - 2.5, mode_y + 4.0)),
	/**/ module, Igc::LIGHT_MODE_POS_ABS));
	addChild(createLightCentered<MediumLight<BlueLight>>(mm2px(Vec(mode_x + 2.5, mode_y + 4.0)),
	/**/ module, Igc::LIGHT_MODE_SPEED));
	addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(mode_x + 7.5, mode_y + 4.0)),
	/**/ module, Igc::LIGHT_MODE_GRAIN));

	/// ADD PARAMS
	addParam(
	/**/ createParamCentered<KnobBig>(mm2px(Vec(delay_x, delay_y)),
	/**/ module,
	/**/ Igc::PARAM_DELAY_TIME));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(delay_x - mod_step_x, delay_y + 10.0)),
	/**/ module,
	/**/ Igc::INPUT_DELAY_CLOCK));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(delay_x + mod_step_x, delay_y + 10.0)),
	/**/ module,
	/**/ Igc::INPUT_DELAY_TIME));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(delay_x + mod_step_x, delay_y + 16.5)),
	/**/ module,
	/**/ Igc::PARAM_DELAY_TIME_MOD));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(pos_x, pos_y)),
	/**/ module,
	/**/ Igc::PARAM_POS));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(pos_x - mod_step_x, pos_y + mod_step_y)),
	/**/ module,
	/**/ Igc::INPUT_POS_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(pos_x - mod_step_x, pos_y + mod_step_y + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_POS_MOD_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(pos_x + mod_step_x, pos_y + mod_step_y)),
	/**/ module,
	/**/ Igc::INPUT_POS_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(pos_x + mod_step_x, pos_y + mod_step_y + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_POS_MOD_2));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(speed_x, speed_y)),
	/**/ module,
	/**/ Igc::PARAM_SPEED));
	addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(Vec(speed_x, speed_y - 7.2)),
	/**/ module, Igc::LIGHT_ROUND_KNOB));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(speed_x - mod_step_x, speed_y + mod_step_y)),
	/**/ module,
	/**/ Igc::INPUT_SPEED_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(speed_x - mod_step_x, speed_y + mod_step_y + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_SPEED_MOD_1));
	addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(Vec(speed_x - mod_step_x - 5.0, speed_y + mod_step_y)),
	/**/ module, Igc::LIGHT_ROUND_INPUT_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(speed_x + mod_step_x, speed_y + mod_step_y)),
	/**/ module,
	/**/ Igc::INPUT_SPEED_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(speed_x + mod_step_x, speed_y + mod_step_y + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_SPEED_MOD_2));
	addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(speed_x + mod_step_x + 5.0, speed_y + mod_step_y)),
	/**/ module, Igc::LIGHT_ROUND_INPUT_2));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(speed_x, speed_y + 22.0)),
	/**/ module,
	/**/ Igc::INPUT_SPEED_REV));
	addParam(
	/**/ createParamCentered<ButtonSwitch>(mm2px(Vec(speed_x + 6.5, speed_y + 22.0)),
	/**/ module,
	/**/ Igc::PARAM_SPEED_REV));
	addParam(
	/**/ createParamCentered<ButtonTriggerState>(mm2px(Vec(speed_x - 6.5, speed_y + 22.0)),
	/**/ module,
	/**/ Igc::PARAM_SPEED_ROUND));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(speed_x, speed_y + 22.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_SPEED_SLEW));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(grain_x, grain_y)),
	/**/ module,
	/**/ Igc::PARAM_GRAIN));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(grain_x - mod_step_x, grain_y + mod_step_y)),
	/**/ module,
	/**/ Igc::INPUT_GRAIN_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(grain_x - mod_step_x, grain_y + mod_step_y + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_GRAIN_MOD_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(grain_x + mod_step_x, grain_y + mod_step_y)),
	/**/ module,
	/**/ Igc::INPUT_GRAIN_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(grain_x + mod_step_x, grain_y + mod_step_y + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_GRAIN_MOD_2));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(lvl_x, lvl_y)),
	/**/ module,
	/**/ Igc::PARAM_LVL));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(lvl_x - mod_step_x, lvl_y + mod_step_y)),
	/**/ module,
	/**/ Igc::INPUT_LVL_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(lvl_x - mod_step_x, lvl_y + mod_step_y + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_LVL_MOD_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(lvl_x + mod_step_x, lvl_y + mod_step_y)),
	/**/ module,
	/**/ Igc::INPUT_LVL_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(lvl_x + mod_step_x, lvl_y + mod_step_y + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_LVL_MOD_2));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(shape_x - 5.0, shape_y)),
	/**/ module,
	/**/ Igc::PARAM_LVL_SHAPE_FORCE));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(shape_x - 5.0, shape_y + 9.0)),
	/**/ module,
	/**/ Igc::INPUT_LVL_SHAPE_FORCE));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(shape_x - 5.0, shape_y + 9.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_LVL_SHAPE_FORCE_MOD));
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(shape_x + 5.0, shape_y)),
	/**/ module,
	/**/ Igc::PARAM_LVL_SHAPE_WAVE));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(shape_x + 5.0, shape_y + 9.0)),
	/**/ module,
	/**/ Igc::INPUT_LVL_SHAPE_WAVE));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(shape_x + 5.0, shape_y + 9.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_LVL_SHAPE_WAVE_MOD));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(5, 98)),
	/**/ module,
	/**/ Igc::INPUT_L));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(5, 108)),
	/**/ module,
	/**/ Igc::INPUT_R));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(101.6 - 5.0, 98)),
	/**/ module,
	/**/ Igc::OUTPUT_L));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(101.6 - 5.0, 108)),
	/**/ module,
	/**/ Igc::OUTPUT_R));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(mix_x, mix_y)),
	/**/ module,
	/**/ Igc::PARAM_MIX_LVL_IN));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(mix_x, mix_y + 9.0)),
	/**/ module,
	/**/ Igc::INPUT_MIX_LVL_IN));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(mix_x, mix_y + 9.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_MIX_LVL_IN_MOD));
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(mix_x + 10.0, mix_y)),
	/**/ module,
	/**/ Igc::PARAM_MIX_LVL_OUT));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(mix_x + 10.0, mix_y + 9.0)),
	/**/ module,
	/**/ Igc::INPUT_MIX_LVL_OUT));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(mix_x + 10.0, mix_y + 9.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_MIX_LVL_OUT_MOD));
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(mix_x + 20.0, mix_y)),
	/**/ module,
	/**/ Igc::PARAM_MIX));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(mix_x + 20.0, mix_y + 9.0)),
	/**/ module,
	/**/ Igc::INPUT_MIX));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(mix_x + 20.0, mix_y + 9.0 + 6.5)),
	/**/ module,
	/**/ Igc::PARAM_MIX_MOD));
}

void IgcWidget::appendContextMenu(Menu *menu) {
	Param		*param;

	menu->addChild(new MenuSeparator());

	param = &(this->module->params[Igc::PARAM_MODE_HD]);
	menu->addChild(new MenuCheckItem("Mode HD", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Igc::PARAM_MODE_ANTICLICK]);
	menu->addChild(new MenuCheckItem("Mode anti-click", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Igc::PARAM_DELAY_PPQN]);
	menu->addChild(rack::createSubmenuItem("Clock PPQN", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("1", "",
				[=]() { return param->getValue() == 1; },
				[=]() { param->setValue(1); }
			));
			menu->addChild(new MenuCheckItem("4", "",
				[=]() { return param->getValue() == 4; },
				[=]() { param->setValue(4); }
			));
			menu->addChild(new MenuCheckItem("8", "",
				[=]() { return param->getValue() == 8; },
				[=]() { param->setValue(8); }
			));
			menu->addChild(new MenuCheckItem("12", "",
				[=]() { return param->getValue() == 12; },
				[=]() { param->setValue(12); }
			));
			menu->addChild(new MenuCheckItem("24", "",
				[=]() { return param->getValue() == 24; },
				[=]() { param->setValue(24); }
			));
			menu->addChild(new MenuCheckItem("48", "",
				[=]() { return param->getValue() == 48; },
				[=]() { param->setValue(48); }
			));
			menu->addChild(new MenuCheckItem("96", "",
				[=]() { return param->getValue() == 96; },
				[=]() { param->setValue(96); }
			));
		}
	));
}
