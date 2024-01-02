
#include "Pkm.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

PkmWidget::PkmWidget(Pkm* _module) {
	float	dist_pitch;
	float	dist_feedback;
	float	dist_delay;
	Vec		p;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Pkm.svg")));

	p.x = 20.32;
	p.y = 53.5;

	dist_pitch = 12.0;
	dist_feedback = 45.0 + 2.6 + 3.5;
	dist_delay = 75.0 + 2.6 + 3.5;

	/// PARAMS
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(p.x, dist_pitch)),
	/**/ module,
	/**/ Pkm::PARAM_PITCH));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(p.x, dist_pitch + 9.5)),
	/**/ module,
	/**/ Pkm::INPUT_PITCH));

	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(p.x - 10.0, dist_pitch + 9.5)),
	/**/ module,
	/**/ Pkm::PARAM_DETUNE));

	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(p.x + 10.0, dist_pitch + 9.5)),
	/**/ module,
	/**/ Pkm::PARAM_WIDTH));

	addParam(
	/**/ createParamCentered<KnobBig>(mm2px(Vec(p.x, dist_feedback)),
	/**/ module,
	/**/ Pkm::PARAM_FEEDBACK));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(p.x - 4.0, dist_feedback + 10.5)),
	/**/ module,
	/**/ Pkm::PARAM_FEEDBACK_MOD));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(p.x + 4.0, dist_feedback + 10.5)),
	/**/ module,
	/**/ Pkm::INPUT_FEEDBACK));

	addParam(
	/**/ createParamCentered<KnobBig>(mm2px(Vec(p.x, dist_delay)),
	/**/ module,
	/**/ Pkm::PARAM_FEEDBACK_DELAY));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(p.x - 4.0, dist_delay - 10.5)),
	/**/ module,
	/**/ Pkm::PARAM_FEEDBACK_DELAY_MOD));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(p.x + 4.0, dist_delay - 10.5)),
	/**/ module,
	/**/ Pkm::INPUT_FEEDBACK_DELAY));

	/// OUTPUTS
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(p.x - 5.0, 103.8)),
	/**/ module,
	/**/ Pkm::OUTPUT_LEFT));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(p.x + 5.0, 103.8)),
	/**/ module,
	/**/ Pkm::OUTPUT_RIGHT));

	///  ALGO SWITCH
	addParam(
	/**/ createParamCentered<ButtonTriggerState>(mm2px(Vec(35.0, 6.75)),
	/**/ module,
	/**/ Pkm::PARAM_ALGO_SWITCH));
	/// ALGO LIGHTS
	addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(p.x - 10.5, 32.0)),
	/**/ module, Pkm::LIGHT_SIMPLE));
	addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(p.x - 3.5, 32.0)),
	/**/ module, Pkm::LIGHT_DOUBLE));
	addChild(createLightCentered<MediumLight<BlueLight>>(mm2px(Vec(p.x + 3.5, 32.0)),
	/**/ module, Pkm::LIGHT_ENTANGLED));
	addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(p.x + 10.5, 32.0)),
	/**/ module, Pkm::LIGHT_ECHO));
}
