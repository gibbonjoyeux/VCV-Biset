
#include "TrackerOut.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerOutWidget::TrackerOutWidget(TrackerOut* _module) {
	TrackerOutDisplay		*display;
	int						i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-Out.svg")));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(25.0, 30.0)),
	/**/ module,
	/**/ TrackerOut::PARAM_SYNTH));

	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(10.5, 43.0)),
	/**/ module,
	/**/ TrackerOut::OUTPUT_PITCH));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(10.5 + 10.0, 43.0)),
	/**/ module,
	/**/ TrackerOut::OUTPUT_GATE));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(10.5 + 10.0 * 2.0, 43.0)),
	/**/ module,
	/**/ TrackerOut::OUTPUT_VELOCITY));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(10.5 + 10.0 * 3.0, 43.0)),
	/**/ module,
	/**/ TrackerOut::OUTPUT_PANNING));

	for (i = 0; i < 4; ++i) {
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(20.5, 65.0 + 10.0 * i)),
		/**/ module,
		/**/ TrackerOut::OUTPUT_CV + i * 2));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(20.5 + 10.7, 65.0 + 10.0 * i)),
		/**/ module,
		/**/ TrackerOut::OUTPUT_CV + i * 2 + 1));
	}

	/// MAIN LED DISPLAY
	display = createWidget<TrackerOutDisplay>(mm2px(Vec(4.0, 4.0)));
	display->box.size = mm2px(Vec(42.5, 18.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);
}

void TrackerOutWidget::onSelect(const SelectEvent &e) {
	int		synth;

	synth = this->module->params[TrackerOut::PARAM_SYNTH].getValue();
	g_editor.set_synth(synth, true);
}

void TrackerOutWidget::appendContextMenu(Menu *menu) {
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

				/// SLIDER MIN
				slider = new MenuSliderEdit(this->module->paramQuantities[TrackerOut::PARAM_OUT_MIN]);
				slider->box.size.x = 200.f;
				menu->addChild(slider);
				/// SLIDER MAX
				slider = new MenuSliderEdit(this->module->paramQuantities[TrackerOut::PARAM_OUT_MAX]);
				slider->box.size.x = 200.f;
				menu->addChild(slider);
			}
		));
	}
}
