
#ifndef OMEGA6_HPP
#define OMEGA6_HPP

#include "../plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct Omega6 : Module {
	enum	ParamIds {
		PARAM_FROM,
		PARAM_TO,
		PARAM_PHASE,
		PARAM_SHAPE,
		PARAM_CURVE,
		PARAM_CURVE_ORDER,
		PARAM_POLYPHONY,
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_FROM,
		INPUT_TO,
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

	Omega6();

	void	process(const ProcessArgs& args) override;
};

struct Omega6Widget : ModuleWidget {
	Omega6			*module;

	Omega6Widget(Omega6* _module);
	void appendContextMenu(Menu *menu) override;
};

#endif
