
#ifndef TRACKERQUANT_HPP
#define TRACKERQUANT_HPP

#include "../plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TrackerQuant: Module {
	enum	ParamIds {
		ENUMS(PARAM_OCTAVE, 4),
		PARAM_COUNT
	};
	enum	InputIds {
		ENUMS(INPUT_QUANT, 4),
		INPUT_COUNT
	};
	enum	OutputIds {
		ENUMS(OUTPUT_QUANT, 4),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	TrackerQuant();

	void	process(const ProcessArgs& args) override;
};

struct TrackerQuantWidget : ModuleWidget {
	TrackerQuant			*module;

	TrackerQuantWidget(TrackerQuant* _module);
};

#endif
