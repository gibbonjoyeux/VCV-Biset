
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

		/// FREQ
		addParam(
		/**/ createParamCentered<KnobMedium>(mm2px(Vec(x + x_step * 0.0, y + y_step * i)),
		/**/ module,
		/**/ TrackerClock::PARAM_FREQ + i));

		/// FREQ DISPLAY
		display = createWidget<TrackerClockDisplay>(mm2px(Vec(x + x_step * 1.0 - 5.25 + 3.0, y + y_step * i - 3.0)));
		display->box.size = mm2px(Vec(10.5, 6.0));
		display->index = i;
		display->module = module;
		display->moduleWidget = this;
		addChild(display);


		/// OUTPUT
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(x + 26.5, y + y_step * i)),
		/**/ module,
		/**/ TrackerClock::OUTPUT_CLOCK + i));
	}
}

