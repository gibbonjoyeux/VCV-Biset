
#ifndef TREE_SEED_HPP
#define TREE_SEED_HPP

#include "../plugin.hpp"

#define TREE_SEED_RANGE_UNI_1	0
#define TREE_SEED_RANGE_UNI_2	1
#define TREE_SEED_RANGE_UNI_3	2
#define TREE_SEED_RANGE_UNI_5	3
#define TREE_SEED_RANGE_UNI_10	4
#define TREE_SEED_RANGE_BI_1	5
#define TREE_SEED_RANGE_BI_2	6
#define TREE_SEED_RANGE_BI_3	7
#define TREE_SEED_RANGE_BI_5	8
#define TREE_SEED_RANGE_BI_10	9

#define TREE_SEED_GATE_TRIGGER	0
#define TREE_SEED_GATE_GATE		1

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TreeSeed : Module {
	enum	ParamIds {
		PARAM_THRESHOLD,
		PARAM_THRESHOLD_MOD,
		PARAM_RANGE,
		PARAM_GATE_MODE,
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_THRESHOLD,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_GATE,
		OUTPUT_PITCH,
		ENUMS(OUTPUT_CV, 3),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	dsp::TSchmittTrigger<float>	trigger_in_seq;
	dsp::PulseGenerator			trigger_out;
	GateGenerator				gate_out;
	bool						fire;

	TreeSeed();

	void	process(const ProcessArgs& args) override;
};

struct TreeSeedWidget : ModuleWidget {
	TreeSeed			*module;

	TreeSeedWidget(TreeSeed* _module);
	void appendContextMenu(Menu *menu) override;
};

#endif
