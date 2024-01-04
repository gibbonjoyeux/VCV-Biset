
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
		display = createWidget<IgcDisplay>(mm2px(Vec(0.0, 0.0)));
		display->box.size = mm2px(Vec(35.56, 128.50));
		display->module = module;
		addChild(display);
	}

	/// ADD SCOPE DISPLAY
	if (this->module) {
		scope = createWidget<IgcScope>(Vec(0.0, 0.0));
		scope->box.size = Vec(5.0, 5.0);
		scope->module = module;
		this->module->scope = scope;
		APP->scene->addChild(scope);
	}
}

void IgcWidget::appendContextMenu(Menu *menu) {
	Param		*param;
	MenuSlider	*slider;
	MenuLabel	*label;

	menu->addChild(new MenuSeparator);
	label = new MenuLabel();
	label->text = "Scope";
	menu->addChild(label);

	param = &(this->module->params[Igc::PARAM_SCOPE_ENABLED]);
	menu->addChild(new MenuCheckItem("Scope enabled", "",
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
