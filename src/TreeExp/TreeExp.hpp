
#ifndef TREE_EXP_HPP
#define TREE_EXP_HPP

#include "../plugin.hpp"

#define TREE_EXP_RANGE_UNI_1	0
#define TREE_EXP_RANGE_UNI_2	1
#define TREE_EXP_RANGE_UNI_3	2
#define TREE_EXP_RANGE_UNI_5	3
#define TREE_EXP_RANGE_UNI_10	4
#define TREE_EXP_RANGE_BI_1		5
#define TREE_EXP_RANGE_BI_2		6
#define TREE_EXP_RANGE_BI_3		7
#define TREE_EXP_RANGE_BI_5		8
#define TREE_EXP_RANGE_BI_10	9

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TreeExp : Module {
	enum	ParamIds {
		PARAM_THRESHOLD,
		PARAM_RANGE,
		PARAM_COUNT
	};
	enum	InputIds {
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
	bool						fire;

	TreeExp();

	void	process(const ProcessArgs& args) override;
};

struct TreeExpWidget : ModuleWidget {
	TreeExp			*module;

	TreeExpWidget(TreeExp* _module);
	void appendContextMenu(Menu *menu) override;
};

#endif
