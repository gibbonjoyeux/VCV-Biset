
#ifndef SEGFAULT_HPP
#define SEGFAULT_HPP

#include "../plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct Segfault : Module {
	enum	ParamIds {
		ENUMS(PARAM_BYPASS, 12),
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_PITCH,
		INPUT_GATE,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_PITCH,
		OUTPUT_GATE,
		ENUMS(OUTPUT_GATE_KEY, 12),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	Segfault();
	void process(const ProcessArgs& args) override;
	//void onReset(const ResetEvent &e) override;
};

struct SegfaultWidget : ModuleWidget {
	Segfault				*module;

	SegfaultWidget(Segfault *_module);
	//void appendContextMenu(Menu *menu) override;
};

#endif
