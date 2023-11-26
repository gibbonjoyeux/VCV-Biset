
#include "Gbu.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

GbuWidget::GbuWidget(Gbu* _module) {
	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-State.svg")));

	/// PARAMS
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(10.0, 10.0)),
	/**/ module,
	/**/ Gbu::PARAM_FREQ_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(16.0, 16.0)),
	/**/ module,
	/**/ Gbu::PARAM_FEEDBACK_1));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(20.0, 10.0)),
	/**/ module,
	/**/ Gbu::PARAM_FREQ_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(26.0, 16.0)),
	/**/ module,
	/**/ Gbu::PARAM_FEEDBACK_2));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(30.0, 10.0)),
	/**/ module,
	/**/ Gbu::PARAM_FREQ_3));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(36.0, 16.0)),
	/**/ module,
	/**/ Gbu::PARAM_FEEDBACK_3));

	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(10.0, 25.0)),
	/**/ module,
	/**/ Gbu::PARAM_FM_1_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(15.0, 25.0)),
	/**/ module,
	/**/ Gbu::PARAM_RM_1_2_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(10.0, 30.0)),
	/**/ module,
	/**/ Gbu::PARAM_FM_2_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(10.0, 40.0)),
	/**/ module,
	/**/ Gbu::PARAM_FM_3_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(15.0, 40.0)),
	/**/ module,
	/**/ Gbu::PARAM_FM_3_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(20.0, 40.0)),
	/**/ module,
	/**/ Gbu::PARAM_RM_3_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(25.0, 40.0)),
	/**/ module,
	/**/ Gbu::PARAM_RM_3_2));

	/// INPUTS
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(10.0, 90.0)),
	/**/ module,
	/**/ Gbu::INPUT_PITCH_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(20.0, 90.0)),
	/**/ module,
	/**/ Gbu::INPUT_PITCH_2));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(30.0, 90.0)),
	/**/ module,
	/**/ Gbu::INPUT_PITCH_3));

	/// OUTPUTS
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(10.0, 100.0)),
	/**/ module,
	/**/ Gbu::OUTPUT_LEFT));
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(20.0, 100.0)),
	/**/ module,
	/**/ Gbu::OUTPUT_RIGHT));

	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(40.0, 100.0)),
	/**/ module,
	/**/ Gbu::OUTPUT_DEBUG));
}
