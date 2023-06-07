
#include "TrackerSynth.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerSynthWidget::TrackerSynthWidget(TrackerSynth* _module) {
	TrackerSynthDisplay		*display;
	int						i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-Synth.svg")));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(25.0 + 2.5, 20.5)),
	/**/ module,
	/**/ TrackerSynth::PARAM_SYNTH));

	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85, 36.6)),
	/**/ module,
	/**/ TrackerSynth::OUTPUT_PITCH));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06, 36.6)),
	/**/ module,
	/**/ TrackerSynth::OUTPUT_GATE));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06 * 2.0, 36.6)),
	/**/ module,
	/**/ TrackerSynth::OUTPUT_VELOCITY));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06 * 3.0, 36.6)),
	/**/ module,
	/**/ TrackerSynth::OUTPUT_PANNING));

	for (i = 0; i < 4; ++i) {
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06 * i, 94.75)),
		/**/ module,
		/**/ TrackerSynth::OUTPUT_CV + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06 * i, 103.75)),
		/**/ module,
		/**/ TrackerSynth::OUTPUT_CV + 4 + i));
	}

	/// MAIN LED DISPLAY
	display = createWidget<TrackerSynthDisplay>(mm2px(Vec(14.5 + 2.5, 4.0)));
	display->box.size = mm2px(Vec(21.25, 9.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);
}

void TrackerSynthWidget::onSelect(const SelectEvent &e) {
	int		synth;

	synth = this->module->params[TrackerSynth::PARAM_SYNTH].getValue();
	g_editor.set_synth(synth, true);
}

void TrackerSynthWidget::appendContextMenu(Menu *menu) {
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
				slider = new MenuSliderEdit(this->module->paramQuantities[TrackerSynth::PARAM_OUT_MIN]);
				slider->box.size.x = 200.f;
				menu->addChild(slider);
				/// SLIDER MAX
				slider = new MenuSliderEdit(this->module->paramQuantities[TrackerSynth::PARAM_OUT_MAX]);
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
