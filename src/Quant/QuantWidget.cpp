
#include "Quant.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

QuantWidget::QuantWidget(Quant* _module) {
	QuantDisplayList	*display_list;

	this->module = _module;
	setModule(this->module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Quant.svg")));
	/// ADD PARAMS
	/// ADD INPUTS / OUTPUTS
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10, 10)),
	/**/ module,
	/**/ Quant::INPUT_PITCH));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10, 20)),
	/**/ module,
	/**/ Quant::INPUT_WHEEL));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(10, 30)),
	/**/ module,
	/**/ Quant::OUTPUT_PITCH));

	//// SIDE LED DISPLAY
	display_list = createWidget<QuantDisplayList>(mm2px(Vec(10.0, 5.0)));
	display_list->box.size = Vec(CHAR_W * 20, CHAR_H * 39 + 5.5);
	display_list->module = module;
	display_list->moduleWidget = this;
	//this->display_list = display_list;
	addChild(display_list);

	this->note_id = 0;
	this->note = NULL;
}

//void QuantWidget::appendContextMenu(Menu *menu) {
//}
