
#include "TrackerControl.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerControlWidget::TrackerControlWidget(TrackerControl* _module) {
	float		px;
	float		step_light;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-Control.svg")));

	px = 30.480 / 2.0;
	step_light = 2.6;

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(px, 12.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_RUN));
	addParam(
	/**/ createParamCentered<ButtonTriggerState>(mm2px(Vec(px - 5.0, 22.0)),
	/**/ module,
	/**/ TrackerControl::PARAM_RUN_MODE));
	addParam(
	/**/ createParamCentered<ButtonTriggerState>(mm2px(Vec(px + 5.0, 22.0)),
	/**/ module,
	/**/ TrackerControl::PARAM_RUN_GATE_MODE));
	addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(px - 5, 27.0)),
	/**/ module, TrackerControl::LIGHT_PATTERN));
	addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(Vec(px - 5, 27.0 + step_light)),
	/**/ module, TrackerControl::LIGHT_SONG));
	addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(px + 5, 27.0)),
	/**/ module, TrackerControl::LIGHT_TRIGGER));
	addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(Vec(px + 5, 27.0 + step_light)),
	/**/ module, TrackerControl::LIGHT_GATE));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(px - 8.0, 50.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_PATTERN_PREV));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(px, 50.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_PATTERN_NEXT));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(px + 8.0, 50.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_PATTERN_RAND));
	addParam(
	/**/ createParamCentered<ButtonTrigger>(mm2px(Vec(px - 8.0, 50.0 + 7.0)),
	/**/ module,
	/**/ TrackerControl::PARAM_PATTERN_PREV));
	addParam(
	/**/ createParamCentered<ButtonTrigger>(mm2px(Vec(px, 50.0 + 7.0)),
	/**/ module,
	/**/ TrackerControl::PARAM_PATTERN_NEXT));
	addParam(
	/**/ createParamCentered<ButtonTrigger>(mm2px(Vec(px + 8.0, 50.0 + 7.0)),
	/**/ module,
	/**/ TrackerControl::PARAM_PATTERN_RAND));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(px - 6.0, 83.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_CLOCK));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(px + 6.0, 83.0)),
	/**/ module,
	/**/ TrackerControl::INPUT_RESET));
	addParam(
	/**/ createParamCentered<ButtonTriggerState>(mm2px(Vec(px, 95.0)),
	/**/ module,
	/**/ TrackerControl::PARAM_CLOCK_MODE));
	addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(px, 100.0)),
	/**/ module, TrackerControl::LIGHT_CLOCK_24));
	addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(px, 100.0 + step_light)),
	/**/ module, TrackerControl::LIGHT_CLOCK_48));
	addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(px, 100.0 + step_light * 2.0)),
	/**/ module, TrackerControl::LIGHT_CLOCK_96));
	addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(Vec(px, 100.0 + step_light * 3.0)),
	/**/ module, TrackerControl::LIGHT_CLOCK_PHASE));
}
