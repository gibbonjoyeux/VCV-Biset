
#include "Blank.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

BlankWidget::BlankWidget(Blank* _module) {
	BlankScope	*scope;
	BlankCables	*display;

	this->module = _module;
	setModule(this->module);
	if (this->module)
		this->set_panel(this->module->params[Blank::PARAM_PANEL].getValue());
	else
		this->set_panel(0);

	/// ADD CABLE DISPLAY
	if (this->module) {
		display = createWidget<BlankCables>(Vec(0, 0));
		display->box.size = math::Vec(INFINITY, INFINITY);
		display->module = module;
		this->module->display = display;
		APP->scene->rack->addChild(display);
	}

	/// ADD SCOPE DISPLAY
	if (this->module) {
		scope = createWidget<BlankScope>(Vec(0, 0));
		scope->box.size = math::Vec(INFINITY, INFINITY);
		scope->module = module;
		this->module->scope = scope;
		APP->scene->addChild(scope);
	}
}

void BlankWidget::appendContextMenu(Menu *menu) {
	Param		*param;
	MenuSlider	*slider;
	MenuLabel	*label;

	/// [1] PANEL

	menu->addChild(new MenuSeparator);

	param = &(this->module->params[Blank::PARAM_PANEL]);
	menu->addChild(rack::createSubmenuItem("Panel", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("City pigeon", "",
				[=]() { return param->getValue() == 0 ; },
				[=]() { this->set_panel(0); }
			));
			menu->addChild(new MenuCheckItem("Wild pigeon", "",
				[=]() { return param->getValue() == 1 ; },
				[=]() { this->set_panel(1); }
			));
			menu->addChild(new MenuCheckItem("Pigeon gang", "",
				[=]() { return param->getValue() == 2 ; },
				[=]() { this->set_panel(2); }
			));
			menu->addChild(new MenuCheckItem("Pigeon Army (loops)", "",
				[=]() { return param->getValue() == 3 ; },
				[=]() { this->set_panel(3); }
			));
		}
	));

	/// [2] CABLE

	menu->addChild(new MenuSeparator);
	label = new MenuLabel();
	label->text = "Cables";
	menu->addChild(label);

	param = &(this->module->params[Blank::PARAM_CABLE_ENABLED]);
	menu->addChild(new MenuCheckItem("Cable animation enabled", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Blank::PARAM_CABLE_LIGHT]);
	menu->addChild(new MenuCheckItem("Cable led", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Blank::PARAM_CABLE_BRIGHTNESS]);
	menu->addChild(new MenuCheckItem("Cable brightness", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Blank::PARAM_CABLE_POLY_THICK]);
	menu->addChild(new MenuCheckItem("Cable polyphonic thickness", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Blank::PARAM_CABLE_POLY_MODE]);
	menu->addChild(rack::createSubmenuItem("Cable polyphonic", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("1st channel", "",
				[=]() { return param->getValue() == BLANK_CABLE_POLY_FIRST; },
				[=]() { param->setValue(BLANK_CABLE_POLY_FIRST); }
			));
			menu->addChild(new MenuCheckItem("Sum", "",
				[=]() { return param->getValue() == BLANK_CABLE_POLY_SUM; },
				[=]() { param->setValue(BLANK_CABLE_POLY_SUM); }
			));
			menu->addChild(new MenuCheckItem("Sum / channel count", "",
				[=]() { return param->getValue() == BLANK_CABLE_POLY_SUM_DIVIDED; },
				[=]() { param->setValue(BLANK_CABLE_POLY_SUM_DIVIDED); }
			));
		}
	));

	param = &(this->module->params[Blank::PARAM_CABLE_FAST]);
	menu->addChild(new MenuCheckItem("Cable CPU fast", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	slider = new MenuSlider(
	/**/ this->module->paramQuantities[Blank::PARAM_CABLE_SLEW]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);

	slider = new MenuSlider(
	/**/ this->module->paramQuantities[Blank::PARAM_CABLE_SCALE]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);

	/// [3] SCOPE

	menu->addChild(new MenuSeparator);
	label = new MenuLabel();
	label->text = "Scope";
	menu->addChild(label);

	param = &(this->module->params[Blank::PARAM_SCOPE_ENABLED]);
	menu->addChild(new MenuCheckItem("Scope enabled", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Blank::PARAM_SCOPE_MAJ]);
	menu->addChild(new MenuCheckItem("Scope on MAJ", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Blank::PARAM_SCOPE_MODE]);
	menu->addChild(new MenuCheckItem("Scope display circular", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Blank::PARAM_SCOPE_POSITION]);
	menu->addChild(rack::createSubmenuItem("Scope position", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Top left", "",
				[=]() { return param->getValue() == BLANK_SCOPE_TOP_LEFT; },
				[=]() { param->setValue(BLANK_SCOPE_TOP_LEFT); }
			));
			menu->addChild(new MenuCheckItem("Top right", "",
				[=]() { return param->getValue() == BLANK_SCOPE_TOP_RIGHT; },
				[=]() { param->setValue(BLANK_SCOPE_TOP_RIGHT); }
			));
			menu->addChild(new MenuCheckItem("Bottom left", "",
				[=]() { return param->getValue() == BLANK_SCOPE_BOTTOM_LEFT; },
				[=]() { param->setValue(BLANK_SCOPE_BOTTOM_LEFT); }
			));
			menu->addChild(new MenuCheckItem("Bottom right", "",
				[=]() { return param->getValue() == BLANK_SCOPE_BOTTOM_RIGHT; },
				[=]() { param->setValue(BLANK_SCOPE_BOTTOM_RIGHT); }
			));
			menu->addChild(new MenuCheckItem("Center", "",
				[=]() { return param->getValue() == BLANK_SCOPE_CENTER; },
				[=]() { param->setValue(BLANK_SCOPE_CENTER); }
			));
		}
	));

	slider = new MenuSlider(
	/**/ this->module->paramQuantities[Blank::PARAM_SCOPE_SCALE]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);

	slider = new MenuSlider(
	/**/ this->module->paramQuantities[Blank::PARAM_SCOPE_THICKNESS]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);

	slider = new MenuSlider(
	/**/ this->module->paramQuantities[Blank::PARAM_SCOPE_BACK_ALPHA]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);

	slider = new MenuSlider(
	/**/ this->module->paramQuantities[Blank::PARAM_SCOPE_VOLT_ALPHA]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);

	slider = new MenuSlider(
	/**/ this->module->paramQuantities[Blank::PARAM_SCOPE_LABEL_ALPHA]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);

	slider = new MenuSlider(
	/**/ this->module->paramQuantities[Blank::PARAM_SCOPE_ALPHA]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);
}

void BlankWidget::set_panel(int id) {
	if (id == 1) {
		setPanel(createPanel(asset::plugin(pluginInstance,
		/**/ "res/Blank-Wild.svg")));
	} else if (id == 2) {
		setPanel(createPanel(asset::plugin(pluginInstance,
		/**/ "res/Blank-Gang.svg")));
	} else if (id == 3) {
		setPanel(createPanel(asset::plugin(pluginInstance,
		/**/ "res/Blank-Army.svg")));
	} else {
		id = 0;
	}
	if (id == 0) {
		setPanel(createPanel(asset::plugin(pluginInstance,
		/**/ "res/Blank.svg")));
	}
	if (this->module)
		this->module->params[Blank::PARAM_PANEL].setValue(id);
}

