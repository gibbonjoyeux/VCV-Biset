
#include "TrackerState.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerStateWidget::TrackerStateWidget(TrackerState* _module) {
	TrackerStateDisplay	*display;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-State.svg")));

	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85, 103.75)),
	/**/ module,
	/**/ TrackerState::OUTPUT_GATE));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06, 103.75)),
	/**/ module,
	/**/ TrackerState::OUTPUT_PLAY));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06 * 2.0, 103.75)),
	/**/ module,
	/**/ TrackerState::OUTPUT_STOP));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06 * 3.0, 103.75)),
	/**/ module,
	/**/ TrackerState::OUTPUT_PLAY_STOP));

	display = createWidget<TrackerStateDisplay>(mm2px(Vec(5.0, 5.0)));
	display->box.size = mm2px(Vec(46, 88.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);
}
