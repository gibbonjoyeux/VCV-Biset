
#include "TrackerDrum.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerDrumWidget::TrackerDrumWidget(TrackerDrum* _module) {
	TrackerDrumDisplay		*display;
	float					x, y;
	int						i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-Drum.svg")));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(27.5, 20.5)),
	/**/ module,
	/**/ TrackerDrum::PARAM_SYNTH));

	for (i = 0; i < 12; ++i) {
		if (i % 2 == 0) {
			x = 6.25;
			y = 36.6;
		} else {
			x = 33.23;
			y = 36.6;
		}
		y += 9.05 * (i / 2);
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(x, y)),
		/**/ module,
		/**/ TrackerDrum::OUTPUT_TRIGGER + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(x + 8.2, y)),
		/**/ module,
		/**/ TrackerDrum::OUTPUT_VELOCITY + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(x + 8.2 * 2, y)),
		/**/ module,
		/**/ TrackerDrum::OUTPUT_PANNING + i));
	}

	for (i = 0; i < 4; ++i) {
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06 * i, 94.75)),
		/**/ module,
		/**/ TrackerDrum::OUTPUT_CV + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06 * i, 103.75)),
		/**/ module,
		/**/ TrackerDrum::OUTPUT_CV + 4 + i));
	}

	/// MAIN LED DISPLAY
	display = createWidget<TrackerDrumDisplay>(mm2px(Vec(14.5 + 2.5, 4.0)));
	display->box.size = mm2px(Vec(21.25, 9.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);
}

void TrackerDrumWidget::onSelect(const SelectEvent &e) {
	int		synth;

	synth = this->module->params[TrackerDrum::PARAM_SYNTH].getValue();
	g_editor.set_synth(synth, true);
}

void TrackerDrumWidget::appendContextMenu(Menu *menu) {
	MenuSeparator	*separator;
	char			str[32];
	int				i;

	separator = new MenuSeparator();
	menu->addChild(separator);

	for (i = 0; i < 8; ++i) {
		sprintf(str, "CV %d", i + 1);
		menu->addChild(rack::createSubmenuItem(str, "",
			[=](Menu *menu) {
				MenuSliderEdit	*slider;
				rack::Widget	*holder;
				MenuItem		*item;

				/// SLIDER MIN
				slider = new MenuSliderEdit(this->module->paramQuantities[TrackerDrum::PARAM_OUT_MIN]);
				slider->box.size.x = 200.f;
				menu->addChild(slider);
				/// SLIDER MAX
				slider = new MenuSliderEdit(this->module->paramQuantities[TrackerDrum::PARAM_OUT_MAX]);
				slider->box.size.x = 200.f;
				menu->addChild(slider);

				/// PRESETS
				//// HOLDER
				holder = new rack::Widget();
				holder->box.size.x = 200.0f;
				holder->box.size.y = 20.0f;
				//// +/-10
				item = new MenuItemStay("+/-10", "",	
					[=]() { }
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				holder->addChild(item);
				//// +/-5
				item = new MenuItemStay("+/-5", "",
					[=]() { }
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				item->box.pos.x = 50.0f;
				holder->addChild(item);
				//// +/-2
				item = new MenuItemStay("+/-2", "",
					[=]() { }
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				item->box.pos.x = 100.0f;
				holder->addChild(item);
				//// +/-1
				item = new MenuItemStay("+/-1", "",
					[=]() { }
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				item->box.pos.x = 150.0f;
				holder->addChild(item);
				menu->addChild(holder);

				//// HOLDER
				holder = new rack::Widget();
				holder->box.size.x = 200.0f;
				holder->box.size.y = 20.0f;
				//// +10
				item = new MenuItemStay("+10", "",	
					[=]() { }
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				holder->addChild(item);
				//// +5
				item = new MenuItemStay("+5", "",
					[=]() { }
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				item->box.pos.x = 50.0f;
				holder->addChild(item);
				//// +2
				item = new MenuItemStay("+2", "",
					[=]() { }
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				item->box.pos.x = 100.0f;
				holder->addChild(item);
				//// +1
				item = new MenuItemStay("+1", "",
					[=]() { }
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				item->box.pos.x = 150.0f;
				holder->addChild(item);

				menu->addChild(holder);
			}
		));
	}
}
