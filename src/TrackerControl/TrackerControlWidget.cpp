
#include "TrackerControl.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerControlWidget::TrackerControlWidget(TrackerControl* _module) {

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-Clock.svg")));

	/// INPUTS
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10.0, 10.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_PLAY_SONG));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10.0, 20.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_PLAY_PATTERN));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10.0, 40.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_CLOCK));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10.0, 60.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_PATTERN_NEXT));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10.0, 70.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_PATTERN_PREV));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10.0, 80.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_PATTERN_RAND));
}
