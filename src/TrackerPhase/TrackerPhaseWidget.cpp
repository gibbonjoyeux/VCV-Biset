
#include "TrackerPhase.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerPhaseWidget::TrackerPhaseWidget(TrackerPhase* _module) {
	TrackerPhaseDisplay	*display;
	float				x, y;
	float				x_step, y_step;
	int					i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-Phase.svg")));

	x = 8.0;
	y = 11.85;// + 10.5;
	x_step = 11.0;
	y_step = 30.0 - 2.9;
	//y_step = 30.0 - 3.5;
	//y_step = 30.0 - 12.0;
	//y_step = 30.0 - 8.5;
	for (i = 0; i < 4; ++i) {
		/// FREQ + SHAPE
		addParam(
		/**/ createParamCentered<KnobMedium>(mm2px(Vec(x, y + y_step * i)),
		/**/ module,
		/**/ TrackerPhase::PARAM_FREQ + i));
		addParam(
		/**/ createParamCentered<KnobSmall>(mm2px(Vec(x + x_step * 1.0, y + y_step * i)),
		/**/ module,
		/**/ TrackerPhase::PARAM_TYPE + i));

		/// OFFSET + SCALE
		addParam(
		/**/ createParamCentered<KnobSmall>(mm2px(Vec(x + x_step * 3.0, y - 3.0 + y_step * i)),
		/**/ module,
		/**/ TrackerPhase::PARAM_OFFSET + i));
		addParam(
		/**/ createParamCentered<KnobSmall>(mm2px(Vec(x + x_step * 3.0, y + 3.0 + y_step * i)),
		/**/ module,
		/**/ TrackerPhase::PARAM_SCALE + i));

		/// PHASE + WARP + INVERT
		addParam(
		/**/ createParamCentered<KnobSmall>(mm2px(Vec(x, y + y_step * i + 8.0)),
		/**/ module,
		/**/ TrackerPhase::PARAM_PHASE + i));
		addParam(
		/**/ createParamCentered<KnobSmall>(mm2px(Vec(x + x_step * 1.0, y + y_step * i + 8.0)),
		/**/ module,
		/**/ TrackerPhase::PARAM_WARP + i));
		addParam(
		/**/ createParamCentered<ButtonSwitch>(mm2px(Vec(x + x_step * 2.0, y + y_step * i + 8.0)),
		/**/ module,
		/**/ TrackerPhase::PARAM_INVERT + i));

		/// FREQ DISPLAY
		display = createWidget<TrackerPhaseDisplay>(mm2px(Vec(x + x_step * 2.0 - 5.25, y + y_step * i - 3.0)));
		display->box.size = mm2px(Vec(10.5, 6.0));
		display->index = i;
		display->module = module;
		display->moduleWidget = this;
		addChild(display);

		/// OUTPUT
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(x + x_step * 4.0 - 2.5, y + y_step * i)),
		/**/ module,
		/**/ TrackerPhase::OUTPUT_PHASE + i));
	}
}

void TrackerPhaseWidget::appendContextMenu(Menu *menu) {
	MenuSeparator	*separator;
	Param			*param_mode;

	separator = new MenuSeparator();
	menu->addChild(separator);


	param_mode = &(this->module->params[TrackerPhase::PARAM_MODE]);
	menu->addChild(rack::createSubmenuItem("Mode", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Fixed", "restart on loop",
				[=]() { return param_mode->getValue() == TPHASE_MODE_FIXED; },
				[=]() { param_mode->setValue(TPHASE_MODE_FIXED); }
			));
			menu->addChild(new MenuCheckItem("Loop", "keep on loop",
				[=]() { return param_mode->getValue() == TPHASE_MODE_LOOP; },
				[=]() { param_mode->setValue(TPHASE_MODE_LOOP); }
			));
		}
	));
}
