
#include "Pkm.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

PkmWidget::PkmWidget(Pkm* _module) {
	Vec		p;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Pkm.svg")));

	p.x = 55.88 / 2.0;
	p.y = 53.5;

	/// PARAMS
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(p.x - 15.0, 20.0)),
	/**/ module,
	/**/ Pkm::PARAM_PITCH));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(p.x - 15.0, 30.0)),
	/**/ module,
	/**/ Pkm::INPUT_PITCH));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(p.x - 0.0, 20.0)),
	/**/ module,
	/**/ Pkm::PARAM_DETUNE));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(p.x + 15.0, 20.0)),
	/**/ module,
	/**/ Pkm::PARAM_WIDTH));

	addParam(
	/**/ createParamCentered<KnobBig>(mm2px(Vec(p.x - 10.0, 57.0)),
	/**/ module,
	/**/ Pkm::PARAM_FEEDBACK));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(p.x - 10.0, 67.5)),
	/**/ module,
	/**/ Pkm::PARAM_FEEDBACK_SCALE));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(p.x - 10.0, 74.5)),
	/**/ module,
	/**/ Pkm::INPUT_FEEDBACK));

	addParam(
	/**/ createParamCentered<KnobBig>(mm2px(Vec(p.x + 10.0, 57.0)),
	/**/ module,
	/**/ Pkm::PARAM_FEEDBACK_DELAY));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(p.x + 10.0, 67.5)),
	/**/ module,
	/**/ Pkm::PARAM_FEEDBACK_DELAY_SCALE));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(p.x + 10.0, 74.5)),
	/**/ module,
	/**/ Pkm::INPUT_FEEDBACK_DELAY));

	/// OUTPUTS
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(p.x - 5.0, 103.8)),
	/**/ module,
	/**/ Pkm::OUTPUT_LEFT));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(p.x + 5.0, 103.8)),
	/**/ module,
	/**/ Pkm::OUTPUT_RIGHT));
}
