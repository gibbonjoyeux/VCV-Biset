
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

RegexWidget::RegexWidget(Regex* _module) {
	RegexDisplay		*display;
	RegexDisplay		*display_prev;
	int						i;

	this->module = _module;
	setModule(this->module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Regex.svg")));
	/// ADD SWITCHES
	for (i = 0; i < 8; ++i) {
		addParam(
		/**/ createParamCentered<ButtonSwitch>(mm2px(Vec(6.5 - 1.75, 8.5 + 13.0 * i)),
		/**/ module,
		/**/ Regex::PARAM_MODE + i));
	}
	/// ADD INPUTS / OUTPUTS
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(104.5 + -3.35 - 3.75, 8.5 + 13.0 * 8)),
	/**/ module,
	/**/ Regex::INPUT_RESET));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(104.5 + 10.0 - 3.75, 8.5 + 13.0 * 8)),
	/**/ module,
	/**/ Regex::INPUT_MASTER));
	for (i = 0; i < 8; ++i) {
		addInput(
		/**/ createInputCentered<Outlet>(mm2px(Vec(104.5 + 10.0 - 3.75, 8.5 + 13.0 * i)),
		/**/ module,
		/**/ Regex::INPUT_EXP_1 + i));
		addInput(
		/**/ createInputCentered<Outlet>(mm2px(Vec(104.5 + 18.0 - 3.75, 8.5 + 13.0 * i)),
		/**/ module,
		/**/ Regex::INPUT_EXP_2 + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(104.5 + 26.0 - 3.75, 8.5 + 13.0 * i)),
		/**/ module,
		/**/ Regex::OUTPUT_EXP + i));
	}

	/// ADD DISPLAYS
	display_prev = NULL;
	for (i = 0; i < 8; ++i) {
		/// DISPLAY
		display = createWidget<RegexDisplay>(mm2px(Vec(3.0 + 10.0 - 3.0, 3.0 + 13.0 * i)));
		display->box.size = mm2px(Vec(95.0, 10.0));
		display->module = module;
		display->moduleWidget = this;
		if (this->module) {
			display->sequence = &(this->module->sequences[i]);
			this->module->sequences[i].display = display;
		} else {
			display->text = 
			/**/ "-----------R---G---X------------"
			/**/ "-------------E---E--------------";
		}
		this->display[i] = display;
		addChild(display);
		/// SET PREV / NEXT
		display->display_prev = display_prev;
		display->display_next = NULL;
		if (display_prev)
			display_prev->display_next = display;
		display_prev = display;
	}
}
