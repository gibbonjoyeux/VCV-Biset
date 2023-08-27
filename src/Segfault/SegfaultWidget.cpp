
#include "Segfault.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

SegfaultWidget::SegfaultWidget(Segfault* _module) {
	float				step;
	int					i;

	this->module = _module;
	setModule(this->module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Segfault.svg")));
	/// ADD PARAMS
	step = 8.265;
	for (i = 0; i < 12; ++i) {
		addParam(
		/**/ createParamCentered<ButtonSwitch>(mm2px(Vec(36.07 - 10.1, 7.57 + step * i)),
		/**/ module,
		/**/ Segfault::PARAM_BYPASS + (11 - i)));
	}
	/// ADD INPUTS / OUTPUTS
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(6.25 + 1.5, 7.57 + step * 9.0)),
	/**/ module,
	/**/ Segfault::INPUT_PITCH));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(6.25 + 1.5, 7.57 + step * 11.0)),
	/**/ module,
	/**/ Segfault::INPUT_GATE));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(44.8 - 10.3, 7.57 + step * 12.53)),
	/**/ module,
	/**/ Segfault::OUTPUT_GATE));
	for (i = 0; i < 12; ++i) {
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(44.8 - 10.3, 7.57 + step * i)),
		/**/ module,
		/**/ Segfault::OUTPUT_GATE_KEY + (11 - i)));
	}

}

//void SegfaultWidget::appendContextMenu(Menu *menu) {
//}
