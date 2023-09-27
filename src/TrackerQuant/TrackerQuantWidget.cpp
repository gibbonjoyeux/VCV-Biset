
#include "TrackerQuant.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerQuantWidget::TrackerQuantWidget(TrackerQuant* _module) {
	float				x, y;
	float				y_step;
	int					i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-Quant.svg")));

	x = 8.0 - 2.0;
	y = 11.85;
	y_step = 30.0 - 2.9;
	for (i = 0; i < 4; ++i) {
		/// OCTAVE KNOB
		addParam(
		/**/ createParamCentered<KnobMedium>(mm2px(Vec(x + 13.2 + 1, y + y_step * i)),
		/**/ module,
		/**/ TrackerQuant::PARAM_OCTAVE + i));

		/// INPUT
		addInput(
		/**/ createInputCentered<Outlet>(mm2px(Vec(x, y + y_step * i)),
		/**/ module,
		/**/ TrackerQuant::INPUT_QUANT + i));

		/// OUTPUT
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(x + 26.4 + 2.0, y + y_step * i)),
		/**/ module,
		/**/ TrackerQuant::OUTPUT_QUANT + i));
	}
}

void TrackerQuantWidget::appendContextMenu(Menu *menu) {
	MenuSeparator	*separator;
	Param			*param_mode;
	int				i;

	separator = new MenuSeparator();
	menu->addChild(separator);

	for (i = 0; i < 4; ++i) {
		param_mode = &(this->module->params[TrackerQuant::PARAM_MODE + i]);
		menu->addChild(rack::createSubmenuItem(string::f("Mode %d", i + 1), "",
			[=](Menu *menu) {
				menu->addChild(new MenuCheckItem("Index down", "",
					[=]() { return param_mode->getValue() == TQUANT_MODE_INDEX_DOWN; },
					[=]() { param_mode->setValue(TQUANT_MODE_INDEX_DOWN); }
				));
				menu->addChild(new MenuCheckItem("Index up", "",
					[=]() { return param_mode->getValue() == TQUANT_MODE_INDEX_UP; },
					[=]() { param_mode->setValue(TQUANT_MODE_INDEX_UP); }
				));
				menu->addChild(new MenuCheckItem("Index round", "",
					[=]() { return param_mode->getValue() == TQUANT_MODE_INDEX_ROUND; },
					[=]() { param_mode->setValue(TQUANT_MODE_INDEX_ROUND); }
				));
				menu->addChild(new MenuCheckItem("Nearest", "",
					[=]() { return param_mode->getValue() == TQUANT_MODE_NEAREST; },
					[=]() { param_mode->setValue(TQUANT_MODE_NEAREST); }
				));
			}
		));
	}
}
