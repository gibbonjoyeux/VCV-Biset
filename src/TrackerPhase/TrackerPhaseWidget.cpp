
#include "TrackerPhase.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerPhaseWidget::TrackerPhaseWidget(TrackerPhase* _module) {
	float		x, y;
	float		x_step, y_step;
	int			i;

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
		/// SHAPE + FREQ + WIDTH
		addParam(
		/**/ createParamCentered<KnobMedium>(mm2px(Vec(x, y + y_step * i)),
		/**/ module,
		/**/ TrackerPhase::PARAM_TYPE + i));
		addParam(
		/**/ createParamCentered<KnobMedium>(mm2px(Vec(x + x_step * 1.0, y + y_step * i)),
		/**/ module,
		/**/ TrackerPhase::PARAM_FREQ + i));
		addParam(
		/**/ createParamCentered<KnobMedium>(mm2px(Vec(x + x_step * 2.0, y + y_step * i)),
		/**/ module,
		/**/ TrackerPhase::PARAM_WIDTH + i));

		/// OFFSET + SCALE
		addParam(
		/**/ createParamCentered<KnobSmall>(mm2px(Vec(x + x_step * 3.0, y - 3.0 + y_step * i)),
		/**/ module,
		/**/ TrackerPhase::PARAM_OFFSET + i));
		addParam(
		/**/ createParamCentered<KnobSmall>(mm2px(Vec(x + x_step * 3.0, y + 3.0 + y_step * i)),
		/**/ module,
		/**/ TrackerPhase::PARAM_SCALE + i));

		/// ROUND + INVERSE
		addParam(
		/**/ createParamCentered<ButtonSwitch>(mm2px(Vec(x, y + y_step * i + 8.0)),
		/**/ module,
		/**/ TrackerPhase::PARAM_ROUND + i));
		addParam(
		/**/ createParamCentered<ButtonSwitch>(mm2px(Vec(x + x_step, y + y_step * i + 8.0)),
		/**/ module,
		/**/ TrackerPhase::PARAM_INVERT + i));

		/// OUTPUT
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(x + x_step * 4.0 - 2.5, y + y_step * i)),
		/**/ module,
		/**/ TrackerPhase::OUTPUT_PHASE + i));
	}
}

