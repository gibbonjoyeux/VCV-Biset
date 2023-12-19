
#ifndef PKM_HPP
#define PKM_HPP

#include "../plugin.hpp"

#define PKM_RESOLUTION		2048
#define PKM_FEEDBACK		4096
#define PKM_ALGO_SIMPLE		0
#define PKM_ALGO_DOUBLE		1

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct Pkm: Module {
	enum	ParamIds {
		PARAM_PITCH,
		PARAM_DETUNE,
		PARAM_WIDTH,
		PARAM_FEEDBACK,
		PARAM_FEEDBACK_MOD,
		PARAM_FEEDBACK_DELAY,
		PARAM_FEEDBACK_DELAY_MOD,
		PARAM_ALGO_SWITCH,
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_PITCH,
		INPUT_FEEDBACK,
		INPUT_FEEDBACK_DELAY,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_LEFT,
		OUTPUT_RIGHT,
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_DOUBLE,
		LIGHT_COUNT
	};

	float	wave[PKM_RESOLUTION];

	float_4	phase_1[16];
	float_4	phase_2[16];
	float_4	out_1[16];
	float_4	out_2[16];

	float	feedback_buffer_1[16][4][PKM_FEEDBACK];
	float	feedback_buffer_2[16][4][PKM_FEEDBACK];
	int		feedback_i;

	Pkm();

	void		process(const ProcessArgs& args) override;
};

struct PkmWidget : ModuleWidget {
	Pkm			*module;

	PkmWidget(Pkm* _module);
};

#endif
