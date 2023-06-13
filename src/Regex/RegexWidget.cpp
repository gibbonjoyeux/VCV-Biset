
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

	for (i = 0; i < 8; ++i) {
		addParam(
		/**/ createParamCentered<KnobSmall>(mm2px(Vec(6.5, 8.5 + 13.0 * i)),
		/**/ module,
		/**/ Regex::PARAM_MODE + i));
	}

	/// ADD INPUTS / OUTPUTS
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(104.5 + 10.0, 8.5 + 13.0 * 8)),
	/**/ module,
	/**/ Regex::INPUT_MASTER));
	for (i = 0; i < 8; ++i) {
		addInput(
		/**/ createInputCentered<Outlet>(mm2px(Vec(104.5 + 10.0, 8.5 + 13.0 * i)),
		/**/ module,
		/**/ Regex::INPUT_EXP + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(104.5 + 20.0, 8.5 + 13.0 * i)),
		/**/ module,
		/**/ Regex::OUTPUT_EXP + i));
	}

	/// ADD DISPLAYS
	display_prev = NULL;
	for (i = 0; i < 8; ++i) {
		/// DISPLAY
		display = createWidget<RegexDisplay>(mm2px(Vec(3.0 + 10.0, 3.0 + 13.0 * i)));
		display->box.size = mm2px(Vec(95.0, 10.0));
		display->module = module;
		display->moduleWidget = this;
		if (this->module) {
			display->sequence = &(this->module->sequences[i]);
			this->module->sequences[i].display = display;
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
