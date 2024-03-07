
#include "Igc.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

IgcWidget::IgcWidget(Igc* _module) {
	IgcDisplay		*display;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tree.svg")));

	display = createWidget<IgcDisplay>(mm2px(Vec(10.0, 10.0)));
	display->box.size = mm2px(Vec(30, 100.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);

	/// ADD PARAMS
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(5, 90)),
	/**/ module,
	/**/ Igc::PARAM_DELAY_TIME));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(5, 10)),
	/**/ module,
	/**/ Igc::INPUT_L));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(5, 20)),
	/**/ module,
	/**/ Igc::INPUT_R));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(5, 40)),
	/**/ module,
	/**/ Igc::INPUT_PLAYHEAD));

	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(45, 10)),
	/**/ module,
	/**/ Igc::OUTPUT_L));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(45, 20)),
	/**/ module,
	/**/ Igc::OUTPUT_R));
}
