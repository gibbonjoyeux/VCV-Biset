
#include "Acro.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

AcroWidget::AcroWidget(Acro* _module) {
	AcroDisplay	*display;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker.svg")));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(200.0, 10.0)),
	/**/ module,
	/**/ Acro::INPUT_CLOCK));

	//// MAIN LED DISPLAY
	display = createWidget<AcroDisplay>(mm2px(Vec(5.0, 5.0)));
	display->box.size = Vec(ACRO_W * ACRO_CHAR_W, ACRO_H * ACRO_CHAR_H);
	display->module = module;
	display->moduleWidget = this;
	this->display = display;
	addChild(display);
}
