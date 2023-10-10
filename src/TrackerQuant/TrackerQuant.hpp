
#ifndef TRACKERQUANT_HPP
#define TRACKERQUANT_HPP

#include "../plugin.hpp"

#define TQUANT_MODE_INDEX_DOWN	0
#define TQUANT_MODE_INDEX_UP	1
#define TQUANT_MODE_INDEX_ROUND	2
#define TQUANT_MODE_NEAREST		3

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TrackerQuant: Module {
	enum	ParamIds {
		ENUMS(PARAM_OCTAVE, 4),
		ENUMS(PARAM_MODE, 4),
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

	void appendContextMenu(Menu *menu) override;
};

#endif
