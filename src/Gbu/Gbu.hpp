
#ifndef GBU_HPP
#define GBU_HPP

#include "../plugin.hpp"

#define GBU_RESOLUTION	2048

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct Gbu: Module {
	enum	ParamIds {
		PARAM_FREQ_GLOBAL,
		PARAM_FREQ_1,
		PARAM_FREQ_2,
		PARAM_FREQ_3,
		PARAM_FEEDBACK_1,
		PARAM_FEEDBACK_2,
		PARAM_FEEDBACK_3,
		PARAM_FM_1_2,				// FM Good to Bad
		PARAM_FM_2_1,				// FM Bad to Good
		PARAM_FM_3_1,				// FM Ugly to Good
		PARAM_FM_3_2,				// FM Ugly to Bad
		PARAM_RM_1_2_1,				// RM Good and Bad
		PARAM_RM_3_1,				// RM Ugly to Good
		PARAM_RM_3_2,				// RM Ugly to Bad
		PARAM_RM_MODE,				// RM / AM mix
		PARAM_NOISE_SPEED,			// Ugly noise speed
		PARAM_NOISE_AMP,			// Ugly noise amplitude
		PARAM_FOLLOW_ATTRACTION,	// Ugly pitch following attraction force
		PARAM_FOLLOW_FRICTION,		// Ugly pitch following friction
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_PITCH_1,
		INPUT_PITCH_2,
		INPUT_PITCH_3,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_LEFT,
		OUTPUT_RIGHT,
		OUTPUT_DEBUG,
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	float	wave[GBU_RESOLUTION];
	float	phase_1;
	float	phase_2;
	float	phase_3;
	float	out_1;
	float	out_2;
	float	out_3;
	float	pitch_3;
	float	pitch_3_acc;
	float	pitch_3_noise_phase;

	Gbu();

	void	process(const ProcessArgs& args) override;
};

struct GbuWidget : ModuleWidget {
	Gbu			*module;

	GbuWidget(Gbu* _module);
};

#endif
