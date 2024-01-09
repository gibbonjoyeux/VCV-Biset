
#include "Igc.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

IgcWidget::IgcWidget(Igc* _module) {
	IgcScope	*scope;
	IgcDisplay	*display;

	this->module = _module;
	setModule(this->module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Igc.svg")));

	/// ADD CABLE DISPLAY
	if (this->module) {
		display = createWidget<IgcDisplay>(Vec(0, 0));
		display->box.size = math::Vec(INFINITY, INFINITY);
		display->module = module;
		this->module->display = display;
		APP->scene->rack->addChild(display);
	}

	/// ADD SCOPE DISPLAY
	if (this->module) {
		scope = createWidget<IgcScope>(Vec(0, 0));
		scope->box.size = math::Vec(INFINITY, INFINITY);
		scope->module = module;
		this->module->scope = scope;
		APP->scene->addChild(scope);
	}
}

void IgcWidget::appendContextMenu(Menu *menu) {
	Param		*param;
	MenuSlider	*slider;
	MenuLabel	*label;

	/// [1] CABLE

	menu->addChild(new MenuSeparator);
	label = new MenuLabel();
	label->text = "Cables";
	menu->addChild(label);

	param = &(this->module->params[Igc::PARAM_CABLE_ENABLED]);
	menu->addChild(new MenuCheckItem("Cable animation enabled", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Igc::PARAM_CABLE_BRIGHTNESS]);
	menu->addChild(new MenuCheckItem("Cable brightness", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Igc::PARAM_CABLE_LED]);
	menu->addChild(new MenuCheckItem("Cable led", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Igc::PARAM_CABLE_POLY_THICK]);
	menu->addChild(new MenuCheckItem("Cable polyphonic thickness", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Igc::PARAM_CABLE_POLY_MODE]);
	menu->addChild(rack::createSubmenuItem("Cable polyphonic", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("1st channel", "",
				[=]() { return param->getValue() == IGC_CABLE_POLY_FIRST; },
				[=]() { param->setValue(IGC_CABLE_POLY_FIRST); }
			));
			menu->addChild(new MenuCheckItem("Sum", "",
				[=]() { return param->getValue() == IGC_CABLE_POLY_SUM; },
				[=]() { param->setValue(IGC_CABLE_POLY_SUM); }
			));
			menu->addChild(new MenuCheckItem("Sum / channel count", "",
				[=]() { return param->getValue() == IGC_CABLE_POLY_SUM_DIVIDED; },
				[=]() { param->setValue(IGC_CABLE_POLY_SUM_DIVIDED); }
			));
		}
	));

	slider = new MenuSlider(
	/**/ this->module->paramQuantities[Igc::PARAM_CABLE_SCALE]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);

	/// [2] SCOPE

	menu->addChild(new MenuSeparator);
	label = new MenuLabel();
	label->text = "Scope";
	menu->addChild(label);

	param = &(this->module->params[Igc::PARAM_SCOPE_ENABLED]);
	menu->addChild(new MenuCheckItem("Scope enabled", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Igc::PARAM_SCOPE_BACKGROUND]);
	menu->addChild(new MenuCheckItem("Scope background", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Igc::PARAM_SCOPE_DETAILS]);
	menu->addChild(new MenuCheckItem("Scope details", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Igc::PARAM_SCOPE_MODE]);
	menu->addChild(new MenuCheckItem("Scope display mode", "",
		[=]() { return param->getValue() == 1; },
		[=]() { param->setValue(!(int)param->getValue()); }
	));

	param = &(this->module->params[Igc::PARAM_SCOPE_POSITION]);
	menu->addChild(rack::createSubmenuItem("Scope position", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Top left", "",
				[=]() { return param->getValue() == IGC_SCOPE_TOP_LEFT; },
				[=]() { param->setValue(IGC_SCOPE_TOP_LEFT); }
			));
			menu->addChild(new MenuCheckItem("Top right", "",
				[=]() { return param->getValue() == IGC_SCOPE_TOP_RIGHT; },
				[=]() { param->setValue(IGC_SCOPE_TOP_RIGHT); }
			));
			menu->addChild(new MenuCheckItem("Bottom left", "",
				[=]() { return param->getValue() == IGC_SCOPE_BOTTOM_LEFT; },
				[=]() { param->setValue(IGC_SCOPE_BOTTOM_LEFT); }
			));
			menu->addChild(new MenuCheckItem("Bottom right", "",
				[=]() { return param->getValue() == IGC_SCOPE_BOTTOM_RIGHT; },
				[=]() { param->setValue(IGC_SCOPE_BOTTOM_RIGHT); }
			));
			menu->addChild(new MenuCheckItem("Center", "",
				[=]() { return param->getValue() == IGC_SCOPE_CENTER; },
				[=]() { param->setValue(IGC_SCOPE_CENTER); }
			));
		}
	));

	slider = new MenuSlider(
	/**/ this->module->paramQuantities[Igc::PARAM_SCOPE_SCALE]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);

	slider = new MenuSlider(
	/**/ this->module->paramQuantities[Igc::PARAM_SCOPE_ALPHA]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);
}
