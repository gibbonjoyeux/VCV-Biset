
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
	int					i;

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
	/**/ createInputCentered<Outlet>(mm2px(Vec(104.5 + 10.0 - 3.75, 8.5 + 13.0 * 8)),
	/**/ module,
	/**/ Regex::INPUT_RESET));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(104.5 + 10.0 + 8.75 - 3.75, 8.5 + 13.0 * 8)),
	/**/ module,
	/**/ Regex::INPUT_MASTER));
	for (i = 0; i < 8; ++i) {
		addInput(
		/**/ createInputCentered<Outlet>(mm2px(Vec(104.5 + 10.0 - 3.75, 8.5 + 13.0 * i)),
		/**/ module,
		/**/ Regex::INPUT_EXP_RESET + i));
		addInput(
		/**/ createInputCentered<Outlet>(mm2px(Vec(104.5 + 10.0 + 8.75 * 1 - 3.75, 8.5 + 13.0 * i)),
		/**/ module,
		/**/ Regex::INPUT_EXP_1 + i));
		addInput(
		/**/ createInputCentered<Outlet>(mm2px(Vec(104.5 + 10.0 + 8.75 * 2 - 3.75, 8.5 + 13.0 * i)),
		/**/ module,
		/**/ Regex::INPUT_EXP_2 + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(104.5 + 10.0 + 8.75 * 3 - 3.75, 8.5 + 13.0 * i)),
		/**/ module,
		/**/ Regex::OUTPUT_EXP_EOC + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(104.5 + 10.0 + 8.75 * 4 - 3.75, 8.5 + 13.0 * i)),
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
			if (this->module->expressions[i].empty() == false)
				display->text = std::move(this->module->expressions[i]);
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

void RegexWidget::appendContextMenu(Menu *menu) {
	MenuLabel	*label;

	/// SEQUENCE TYPE
	menu->addChild(new MenuSeparator);
	label = new MenuLabel();
	label->text = "Expression exemples";
	menu->addChild(label);
	menu->addChild(new MenuItemStay("#1,2,3", "", [=](){}));
	menu->addChild(new MenuItemStay("#(1,2,3)%16", "", [=](){}));
	menu->addChild(new MenuItemStay("#(1,2,3,?(4,5,6))", "", [=](){}));

	/// SEQUENCE TYPE
	menu->addChild(new MenuSeparator);
	label = new MenuLabel();
	label->text = "Expression types";
	menu->addChild(label);
	menu->addChild(new MenuItemStay("#", "Sequence", [=](){}));
	menu->addChild(new MenuItemStay("@", "Shuffle sequence", [=](){}));
	menu->addChild(new MenuItemStay("?", "Random", [=](){}));
	menu->addChild(new MenuItemStay("!", "Random exclude", [=](){}));
	menu->addChild(new MenuItemStay("$", "Walk random", [=](){}));

	/// SEQUENCE VALUES
	menu->addChild(new MenuSeparator);
	label = new MenuLabel();
	label->text = "Expression values";
	menu->addChild(label);
	menu->addChild(new MenuItemStay("4 / 12", "Number", [=](){}));
	menu->addChild(new MenuItemStay("c / c# / cb / c4", "Pitch", [=](){}));

	/// SEQUENCE MODULATOR
	menu->addChild(new MenuSeparator);
	label = new MenuLabel();
	label->text = "Expression modulators";
	menu->addChild(label);
	menu->addChild(new MenuItemStay("xN", "Multiply by N", [=](){}));
	menu->addChild(new MenuItemStay("%N", "Until N", [=](){}));

	/// SEQUENCE MODULATOR
	menu->addChild(new MenuSeparator);
	label = new MenuLabel();
	label->text = "Shortcuts";
	menu->addChild(label);
	menu->addChild(new MenuItemStay("Enter", "Compile exp", [=](){}));
	menu->addChild(new MenuItemStay("Ctrl + Enter", "Compile all exp", [=](){}));
	menu->addChild(new MenuItemStay("Maj + Enter", "Compile sync exp", [=](){}));
	menu->addChild(new MenuItemStay("Escape", "Stop exp", [=](){}));
	menu->addChild(new MenuItemStay("Ctrl + Arrow", "Jump exp", [=](){}));
}
