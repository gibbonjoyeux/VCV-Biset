
#include "TrackerQuant.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerQuantWidget::TrackerQuantWidget(TrackerQuant* _module) {
	float				x, y;
	float				x_step, y_step;
	int					i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-Quant.svg")));

	x = 8.0 - 2.0;
	y = 11.85;
	x_step = 11.0;
	y_step = 30.0 - 2.9;
	for (i = 0; i < 4; ++i) {

		/// OCTAVE KNOB
		addParam(
		/**/ createParamCentered<KnobMedium>(mm2px(Vec(x + 13.2 + 1, y + y_step * i)),
		/**/ module,
		/**/ TrackerQuant::PARAM_OCTAVE + i));

		/// INPUT
		addInput(
		/**/ createInputCentered<Outlet>(mm2px(Vec(x, y + y_step * i)),
		/**/ module,
		/**/ TrackerQuant::INPUT_QUANT + i));

		/// OUTPUT
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(x + 26.4 + 2.0, y + y_step * i)),
		/**/ module,
		/**/ TrackerQuant::OUTPUT_QUANT + i));
	}
}

