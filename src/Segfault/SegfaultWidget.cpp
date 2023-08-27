
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
	//if (this->module)
	//	this->module->widget = this;
	setModule(this->module);
	/// COMPUTE WIDGET TYPE
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Segfault.svg")));

	/// ADD PARAMS
	/// ADD SWITCHES
	step = 8.265;
	for (i = 0; i < 12; ++i) {
		addParam(
		/**/ createParamCentered<ButtonSwitch>(mm2px(Vec(41.5, 7.57 + step * i)),
		/**/ module,
		/**/ Segfault::PARAM_BYPASS + (11 - i)));
	}
	/// ADD INPUTS / OUTPUTS
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(6.25, 7.57 + step * 10.0)),
	/**/ module,
	/**/ Segfault::INPUT_PITCH));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(15.75, 7.57 + step * 10.0)),
	/**/ module,
	/**/ Segfault::INPUT_GATE));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(6.25, 7.57 + step * 11.0)),
	/**/ module,
	/**/ Segfault::OUTPUT_PITCH));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(15.75, 7.57 + step * 11.0)),
	/**/ module,
	/**/ Segfault::OUTPUT_GATE));
	for (i = 0; i < 12; ++i) {
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(50.18, 7.57 + step * i)),
		/**/ module,
		/**/ Segfault::OUTPUT_GATE_KEY + (11 - i)));
	}

}

//void SegfaultWidget::appendContextMenu(Menu *menu) {
//}
