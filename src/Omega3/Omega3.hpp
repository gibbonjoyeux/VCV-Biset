
#ifndef OMEGA3_HPP
#define OMEGA3_HPP

#include "../plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct Omega3 : Module {
	enum	ParamIds {
		PARAM_PHASE,
		PARAM_SHAPE,
		PARAM_CURVE,
		PARAM_CURVE_ORDER,
		PARAM_POLYPHONY,
		PARAM_OFFSET,
		PARAM_SCALE,
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_PHASE,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_CV,
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	Omega3();

	void	process(const ProcessArgs& args) override;
};

struct Omega3Widget : ModuleWidget {
	Omega3			*module;

	Omega3Widget(Omega3* _module);
	void appendContextMenu(Menu *menu) override;
};

#endif
