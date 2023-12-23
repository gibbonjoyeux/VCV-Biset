
#include "Fft.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

FftWidget::FftWidget(Fft* _module) {
	FftDisplay	*display;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Pkm.svg")));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10.0, 10.0)),
	/**/ module,
	/**/ Fft::INPUT));

	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(30.0, 10.0)),
	/**/ module,
	/**/ Fft::OUTPUT_PITCH));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(30.0, 20.0)),
	/**/ module,
	/**/ Fft::OUTPUT_VELOCITY));

	display = createWidget<FftDisplay>(mm2px(Vec(2.0, 30.0)));
	display->box.size = mm2px(Vec(40.640 - 4.0, 40.640 - 4.0));
	display->module = module;
	display->moduleWidget = this;
	//this->display = display;
	addChild(display);
}
