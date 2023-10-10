
#include "TrackerClock.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerClockWidget::TrackerClockWidget(TrackerClock* _module) {
	TrackerClockDisplay	*display;
	float				x, y;
	float				x_step, y_step;
	int					i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-Clock.svg")));

	x = 8.0;
	y = 11.85;// + 10.5;
	x_step = 11.0;
	y_step = 30.0 - 2.9;
	//y_step = 30.0 - 3.5;
	//y_step = 30.0 - 12.0;
	//y_step = 30.0 - 8.5;
	for (i = 0; i < 4; ++i) {

		/// FREQ + PHASE + PULSE WIDTH
		addParam(
		/**/ createParamCentered<KnobMedium>(mm2px(Vec(x, y + y_step * i)),
		/**/ module,
		/**/ TrackerClock::PARAM_FREQ + i));
		addParam(
		/**/ createParamCentered<KnobSmall>(mm2px(Vec(x, y + y_step * i + 8.0)),
		/**/ module,
		/**/ TrackerClock::PARAM_PHASE + i));
		addParam(
		/**/ createParamCentered<KnobSmall>(mm2px(Vec(x + 8.5, y + y_step * i + 8.0)),
		/**/ module,
		/**/ TrackerClock::PARAM_PW + i));

		/// FREQ DISPLAY
		display = createWidget<TrackerClockDisplay>(mm2px(Vec(x + 11.0 + 3.0 - 5.25, y + y_step * i - 3.0)));
		display->box.size = mm2px(Vec(10.5, 6.0));
		display->index = i;
		display->module = module;
		display->moduleWidget = this;
		addChild(display);

		/// OUTPUT
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(x + 26.4, y + y_step * i)),
		/**/ module,
		/**/ TrackerClock::OUTPUT_CLOCK + i));
	}
}

void TrackerClockWidget::appendContextMenu(Menu *menu) {
	MenuSeparator	*separator;
	Param			*param_mode;

	separator = new MenuSeparator();
	menu->addChild(separator);


	param_mode = &(this->module->params[TrackerClock::PARAM_MODE]);
	menu->addChild(rack::createSubmenuItem("Mode", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Fixed", "restart on loop",
				[=]() { return param_mode->getValue() == TCLOCK_MODE_FIXED; },
				[=]() { param_mode->setValue(TCLOCK_MODE_FIXED); }
			));
			menu->addChild(new MenuCheckItem("Loop", "keep on loop",
				[=]() { return param_mode->getValue() == TCLOCK_MODE_LOOP; },
				[=]() { param_mode->setValue(TCLOCK_MODE_LOOP); }
			));
		}
	));
}
