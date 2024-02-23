
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

RegexWidget::RegexWidget(Regex* _module, bool _condensed) {
	RegexDisplay		*display;
	RegexDisplay		*display_prev;
	float				step;
	float				height;
	float				offset;
	int					i;

	this->module = _module;
	if (this->module)
		this->module->widget = this;
	this->condensed = _condensed;
	setModule(this->module);
	/// COMPUTE WIDGET TYPE
	if (this->condensed) {
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Regex-Condensed.svg")));
		step = 8.265;
		height = 6.0;
		offset = 2.5;
		this->exp_count = 12;
	} else {
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Regex.svg")));
		step = 13.0;
		height = 10.0;
		offset = 0;
		this->exp_count = 8;
	}

	/// ADD PARAMS
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(114.5 - 8.75 - 3.75, 8.5 + 13.0 * 8 + 10.75)),
	/**/ module,
	/**/ Regex::PARAM_BIAS));
	/// ADD SWITCHES
	for (i = 0; i < this->exp_count; ++i) {
		addParam(
		/**/ createParamCentered<ButtonSwitch>(mm2px(Vec(6.5 - 1.75, 8.5 + step * i)),
		/**/ module,
		/**/ Regex::PARAM_MODE + i));
	}
	/// ADD INPUTS / OUTPUTS
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(114.5 - 8.75 - 3.75, 8.5 + 13.0 * 8)),
	/**/ module,
	/**/ Regex::INPUT_BIAS));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(114.5 + 8.75 * 0 - 3.75, 8.5 + 13.0 * 8)),
	/**/ module,
	/**/ Regex::INPUT_RESET));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(114.5 + 8.75 * 1 - 3.75, 8.5 + 13.0 * 8)),
	/**/ module,
	/**/ Regex::INPUT_MASTER));
	for (i = 0; i < this->exp_count; ++i) {
		addInput(
		/**/ createInputCentered<Outlet>(mm2px(Vec(114.5 - 3.75, 8.5 + step * i)),
		/**/ module,
		/**/ Regex::INPUT_EXP_RESET + i));
		addInput(
		/**/ createInputCentered<Outlet>(mm2px(Vec(114.5 + 8.75 * 1 - 3.75, 8.5 + step * i)),
		/**/ module,
		/**/ Regex::INPUT_EXP_1 + i));
		addInput(
		/**/ createInputCentered<Outlet>(mm2px(Vec(114.5 + 8.75 * 2 - 3.75, 8.5 + step * i)),
		/**/ module,
		/**/ Regex::INPUT_EXP_2 + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(114.5 + 8.75 * 3 - 3.75, 8.5 + step * i)),
		/**/ module,
		/**/ Regex::OUTPUT_EXP_EOC + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(114.5 + 8.75 * 4 - 3.75, 8.5 + step * i)),
		/**/ module,
		/**/ Regex::OUTPUT_EXP + i));
	}

	/// ADD DISPLAYS
	display_prev = NULL;
	for (i = 0; i < this->exp_count; ++i) {
		/// DISPLAY
		display = createWidget<RegexDisplay>(mm2px(Vec(3.0 + 10.0 - 3.0, 3.0 + step * i + offset)));
		display->box.size = mm2px(Vec(95.0, height));
		display->module = module;
		display->moduleWidget = this;
		display->condensed = this->condensed;
		if (this->module) {
			display->text = this->module->sequences[i].string_edit;
			display->sequence = &(this->module->sequences[i]);
		} else {
			display->sequence = NULL;
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
	Param		*param_run;
	MenuLabel	*label;

	/// [1] MENU OPTIONS
	menu->addChild(new MenuSeparator);
	param_run = &(this->module->params[Regex::PARAM_RUN_START]);
	menu->addChild(new MenuCheckItem("Run on start up", "",
		[=]() { return param_run->getValue() == 1; },
		[=]() { param_run->setValue(!(int)param_run->getValue()); }
	));

	/// [2] MENU HELP
	menu->addChild(new MenuSeparator);
	label = new MenuLabel();
	label->text = "Help";
	menu->addChild(label);

	/// SEQUENCE EXAMPLES
	menu->addChild(rack::createSubmenuItem("Examples", "",
		[=](Menu *menu) {
			menu->addChild(new MenuItemStay("1,2,3", "", [=](){}));
			menu->addChild(new MenuItemStay(">(1,2,3)%16", "", [=](){}));
			menu->addChild(new MenuItemStay(">(1,2,3,?(4,5,6))", "", [=](){}));

			menu->addChild(new MenuItemStay("c,d#,e", "", [=](){}));
			menu->addChild(new MenuItemStay("^(c,d#,e,f)%16", "", [=](){}));
		}
	));

	/// SEQUENCE TYPES
	menu->addChild(rack::createSubmenuItem("Sequence types", "",
		[=](Menu *menu) {
			menu->addChild(new MenuItemStay(">", "Sequence forward", [=](){}));
			menu->addChild(new MenuItemStay("<", "Sequence backward", [=](){}));
			menu->addChild(new MenuItemStay("^", "Sequence ping-pong", [=](){}));
			menu->addChild(new MenuItemStay("@", "Shuffle sequence", [=](){}));
			menu->addChild(new MenuItemStay("?", "Random", [=](){}));
			menu->addChild(new MenuItemStay("!", "Random exclude", [=](){}));
			menu->addChild(new MenuItemStay("$", "Walk random", [=](){}));
		}
	));

	/// SEQUENCE MODULATORS
	menu->addChild(rack::createSubmenuItem("Modulators", "",
		[=](Menu *menu) {
			MenuLabel	*label;

			label = new MenuLabel();
			label->text = "Clock sequence";
			menu->addChild(label);

			menu->addChild(new MenuItemStay("xN", "All items times N", [=](){}));
			menu->addChild(new MenuItemStay("%N", "Until N clock", [=](){}));
			menu->addChild(new MenuItemStay("*N", "Until N items", [=](){}));

			menu->addChild(new MenuSeparator);
			label = new MenuLabel();
			label->text = "CV / Pitch sequence";
			menu->addChild(label);

			menu->addChild(new MenuItemStay("xN", "All items times N", [=](){}));
			menu->addChild(new MenuItemStay("%N", "Until N items", [=](){}));
			menu->addChild(new MenuItemStay("*N", "Until N items", [=](){}));
		}
	));

	/// SEQUENCE VALUES
	menu->addChild(rack::createSubmenuItem("Values", "",
		[=](Menu *menu) {
			menu->addChild(new MenuItemStay("4 / 12 / -7", "Number", [=](){}));
			menu->addChild(new MenuItemStay("c / c# / cb / c4", "Pitch", [=](){}));
		}
	));

	/// MODULE SHORTCUTS
	menu->addChild(rack::createSubmenuItem("Shortcuts", "",
		[=](Menu *menu) {
			menu->addChild(new MenuItemStay("Enter", "Compile exp", [=](){}));
			menu->addChild(new MenuItemStay("Ctrl + Enter", "Compile all exp", [=](){}));
			menu->addChild(new MenuItemStay("Escape", "Stop exp", [=](){}));
			menu->addChild(new MenuItemStay("Ctrl + Arrow", "Jump exp", [=](){}));
		}
	));
}
