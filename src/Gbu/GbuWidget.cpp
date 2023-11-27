
#include "Gbu.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

inline Vec triangle(
	Vec		center,
	float	angle,
	float	dist,
	float	off_x = 0,
	float	off_y = 0) {
	float	x;
	float	y;

	x = center.x + cos(angle * 2.0 * M_PI - M_PI - M_PI / 6.0) * dist + off_x;
	y = center.y + sin(angle * 2.0 * M_PI - M_PI - M_PI / 6.0) * dist + off_y;
	return Vec(x, y);
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

GbuWidget::GbuWidget(Gbu* _module) {
	float	dist_fm;
	float	dist_rm;
	float	dist_rm_in;
	float	dist_fm_in;
	Vec		p;
	int		i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Gbu.svg")));

	p.x = 45.7;	// 45.7
	p.y = 53.5;	// 53.5 or 59.0

	/// PARAMS
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(triangle(p, -0.5/6.0, 40.0)),
	/**/ module,
	/**/ Gbu::PARAM_FREQ_GLOBAL));
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(triangle(p, -1.5/6.0, 40.0)),
	/**/ module,
	/**/ Gbu::PARAM_RM_MODE));

	for (i = 0; i < 3; ++i) {
		addParam(
		/**/ createParamCentered<KnobSmall>(mm2px(triangle(p, (float)-i/3.0, 12.5)),
		/**/ module,
		/**/ Gbu::PARAM_FREQ_1 + i));
		addParam(
		/**/ createParamCentered<KnobSmall>(mm2px(triangle(p, (float)-i/3.0, 36.5)),
		/**/ module,
		/**/ Gbu::PARAM_FEEDBACK_1 + i));
		addParam(
		/**/ createParamCentered<KnobMedium>(mm2px(triangle(p, (float)-i/3.0, 28.5)),
		/**/ module,
		/**/ Gbu::PARAM_LEVEL_1 + i));

		addInput(
		/**/ createInputCentered<Outlet>(mm2px(triangle(p, (float)-i/3.0, 20.0)),
		/**/ module,
		/**/ Gbu::INPUT_LEVEL_1 + i));
		addInput(
		/**/ createInputCentered<Outlet>(mm2px(triangle(p, (float)-i/3.0, 43.0)),
		/**/ module,
		/**/ Gbu::INPUT_FEEDBACK_1 + i));
	}
	dist_rm_in = 10.25;
	dist_fm_in = 28.75;
	dist_rm = 16.5;
	dist_fm = 22.5;
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(triangle(p, -1.0/6.0, dist_fm, -3.0)),
	/**/ module,
	/**/ Gbu::PARAM_FM_1_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(triangle(p, -1.0/6.0, dist_fm, +3.0)),
	/**/ module,
	/**/ Gbu::PARAM_FM_2_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(triangle(p, -1.0/6.0, dist_rm)),
	/**/ module,
	/**/ Gbu::PARAM_RM_1_2_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(triangle(p, -1.0/6.0, dist_rm_in)),
	/**/ module,
	/**/ Gbu::INPUT_RM_1_2_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(triangle(p, -1.0/6.0, dist_fm_in, -3.5)),
	/**/ module,
	/**/ Gbu::INPUT_FM_1_2));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(triangle(p, -1.0/6.0, dist_fm_in, +3.5)),
	/**/ module,
	/**/ Gbu::INPUT_FM_2_1));

	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(triangle(p, -1.0/6.0-2.0/3.0, dist_rm)),
	/**/ module,
	/**/ Gbu::PARAM_RM_3_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(triangle(p, -1.0/6.0-1.0/3.0, dist_rm)),
	/**/ module,
	/**/ Gbu::PARAM_RM_3_2));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(triangle(p, -1.0/6.0-2.0/3.0, dist_fm)),
	/**/ module,
	/**/ Gbu::PARAM_FM_3_1));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(triangle(p, -1.0/6.0-1.0/3.0, dist_fm)),
	/**/ module,
	/**/ Gbu::PARAM_FM_3_2));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(triangle(p, -1.0/6.0-2.0/3.0, dist_rm_in)),
	/**/ module,
	/**/ Gbu::INPUT_RM_3_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(triangle(p, -1.0/6.0-1.0/3.0, dist_rm_in)),
	/**/ module,
	/**/ Gbu::INPUT_RM_3_2));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(triangle(p, -1.0/6.0-2.0/3.0, dist_fm_in)),
	/**/ module,
	/**/ Gbu::INPUT_FM_3_1));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(triangle(p, -1.0/6.0-1.0/3.0, dist_fm_in)),
	/**/ module,
	/**/ Gbu::INPUT_FM_3_2));

	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(triangle(p, -2.0/3.0, 28.5, -8.0, -6.0)),
	/**/ module,
	/**/ Gbu::PARAM_NOISE_SPEED));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(triangle(p, -2.0/3.0, 28.5, -10.0)),
	/**/ module,
	/**/ Gbu::PARAM_NOISE_AMP));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(triangle(p, -2.0/3.0, 28.5, +10.0)),
	/**/ module,
	/**/ Gbu::PARAM_FOLLOW_ATTRACTION));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(triangle(p, -2.0/3.0, 28.5, +8.0, -6.0)),
	/**/ module,
	/**/ Gbu::PARAM_FOLLOW_FRICTION));





	/// INPUTS
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(45.7 - 5.25, 103.75)),
	/**/ module,
	/**/ Gbu::INPUT_PITCH));

	/// OUTPUTS
	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(45.7 + 5.25, 103.75)),
	/**/ module,
	/**/ Gbu::OUTPUT_MIX));

	addOutput(
	/**/ createOutputCentered<Outlet>(mm2px(Vec(80.0, 110.0)),
	/**/ module,
	/**/ Gbu::OUTPUT_DEBUG));
}
