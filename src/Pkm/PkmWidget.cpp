
#include "Pkm.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

PkmWidget::PkmWidget(Pkm* _module) {
	Vec		p;
	int		i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-Phase.svg")));

	p.x = 25.7;
	p.y = 53.5;

	/// PARAMS
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(10.0, 10.0)),
	/**/ module,
	/**/ Pkm::PARAM_PITCH));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10.0, 20.0)),
	/**/ module,
	/**/ Pkm::INPUT_PITCH));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(20.0, 10.0)),
	/**/ module,
	/**/ Pkm::PARAM_FEEDBACK));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(20.0, 20.0)),
	/**/ module,
	/**/ Pkm::INPUT_FEEDBACK));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(30.0, 10.0)),
	/**/ module,
	/**/ Pkm::PARAM_FEEDBACK_DELAY));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(30.0, 20.0)),
	/**/ module,
	/**/ Pkm::INPUT_FEEDBACK_DELAY));

	/// OUTPUTS
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(30.0, 50.0)),
	/**/ module,
	/**/ Pkm::OUTPUT_LEFT));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(40.0, 50.0)),
	/**/ module,
	/**/ Pkm::OUTPUT_RIGHT));
}
