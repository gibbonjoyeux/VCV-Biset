
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
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-Control.svg")));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10.0, 10.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_RUN));
	addParam(
	/**/ createParamCentered<ButtonSwitch>(mm2px(Vec(20.0, 10.0)),
	/**/ module,
	/**/ TrackerControl::PARAM_RUN_MODE));
	addParam(
	/**/ createParamCentered<ButtonSwitch>(mm2px(Vec(30.0, 10.0)),
	/**/ module,
	/**/ TrackerControl::PARAM_RUN_GATE_MODE));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10.0, 40.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_CLOCK));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(20.0, 40.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_RESET));
	addParam(
	/**/ createParamCentered<ButtonTriggerState>(mm2px(Vec(30.0, 40.0)),
	/**/ module,
	/**/ TrackerControl::PARAM_CLOCK_MODE));

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
