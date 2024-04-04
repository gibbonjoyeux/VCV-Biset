
#include "Omega3.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Omega3Widget::Omega3Widget(Omega3* _module) {
	float		center;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Omega3.svg")));

	center = 20.32 / 2.0;

	/// ADD PARAMS
	addParam(
	/**/ createParamCentered<KnobBig>(mm2px(Vec(center, 12)),
	/**/ module,
	/**/ Omega3::PARAM_POLYPHONY));
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(center, 12 + 16.5)),
	/**/ module,
	/**/ Omega3::PARAM_SHAPE));
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(center, 12 + 30)),
	/**/ module,
	/**/ Omega3::PARAM_CURVE));

	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(center - 6.0, 12 + 40)),
	/**/ module,
	/**/ Omega3::PARAM_OFFSET));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(center + 6.0, 12 + 40)),
	/**/ module,
	/**/ Omega3::PARAM_SCALE));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(center - 6.0, 12 + 47)),
	/**/ module,
	/**/ Omega3::INPUT_OFFSET));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(center + 6.0, 12 + 47)),
	/**/ module,
	/**/ Omega3::INPUT_SCALE));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(center, 128.5 - 22 - 22)),
	/**/ module,
	/**/ Omega3::PARAM_PHASE));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(center, 128.5 - 22 - 13)),
	/**/ module,
	/**/ Omega3::INPUT_PHASE));

	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(center, 128.5 - 22)),
	/**/ module,
	/**/ Omega3::OUTPUT_CV));

}

void Omega3Widget::appendContextMenu(Menu *menu) {
	Param		*param;

	menu->addChild(new MenuSeparator());

	param = &(this->module->params[Omega3::PARAM_CURVE_ORDER]);
	menu->addChild(rack::createSubmenuItem("Curve order", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Pre (curve, phase, shape)", "",
				[=]() { return param->getValue() == 0; },
				[=]() { param->setValue(0); }
			));
			menu->addChild(new MenuCheckItem("Post (phase, shape, curve)", "",
				[=]() { return param->getValue() == 1; },
				[=]() { param->setValue(1); }
			));
		}
	));
}
