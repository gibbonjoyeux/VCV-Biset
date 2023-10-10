
#ifndef TRACKERPHASE_HPP
#define TRACKERPHASE_HPP

#include "../plugin.hpp"

#define TPHASE_MODE_FIXED	0
#define TPHASE_MODE_LOOP	1

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TrackerPhase: Module {
	enum	ParamIds {
		ENUMS(PARAM_TYPE, 4),
		ENUMS(PARAM_FREQ, 4),
		ENUMS(PARAM_PHASE, 4),
		ENUMS(PARAM_WARP, 4),
		ENUMS(PARAM_INVERT, 4),
		ENUMS(PARAM_OFFSET, 4),
		ENUMS(PARAM_SCALE, 4),
		PARAM_MODE,
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_COUNT
	};
	enum	OutputIds {
		ENUMS(OUTPUT_PHASE, 4),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};
	dsp::TSchmittTrigger<float>	trigger_restart;
	float						phase_play;
	float						phase[4];
	int							count[4];

	TrackerPhase();

	void	process(const ProcessArgs& args) override;
};

struct TrackerPhaseDisplay : LedDisplay {
	TrackerPhase			*module;
	ModuleWidget			*moduleWidget;
	std::string				font_path;
	int						index;

	TrackerPhaseDisplay();
	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
	void onButton(const ButtonEvent &e) override;
};

struct TrackerPhaseWidget : ModuleWidget {
	TrackerPhase			*module;

	TrackerPhaseWidget(TrackerPhase* _module);
	void appendContextMenu(Menu *menu) override;
};

#endif
