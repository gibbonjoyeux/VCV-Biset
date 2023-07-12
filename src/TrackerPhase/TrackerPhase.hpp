
#ifndef TRACKERSYNTH_HPP
#define TRACKERSYNTH_HPP

#include "../plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TrackerPhase: Module {
	enum	ParamIds {
		ENUMS(PARAM_TYPE, 4),
		ENUMS(PARAM_FREQ, 4),
		ENUMS(PARAM_PHASE, 4),
		ENUMS(PARAM_WIDTH, 4),
		ENUMS(PARAM_ROUND, 4),
		ENUMS(PARAM_INVERSE, 4),
		ENUMS(PARAM_OFFSET, 4),
		ENUMS(PARAM_SCALE, 4),
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
	float		phases[4];

	TrackerPhase();

	void	process(const ProcessArgs& args) override;
};

//struct TrackerPhaseDisplay : LedDisplay {
//	TrackerPhase			*module;
//	ModuleWidget			*moduleWidget;
//	std::string				font_path;
//
//	TrackerPhaseDisplay();
//	void draw(const DrawArgs &args) override {};
//	void drawLayer(const DrawArgs &args, int layer) override;
//	void onButton(const ButtonEvent &e) override;
//};

struct TrackerPhaseWidget : ModuleWidget {
	TrackerPhase			*module;

	TrackerPhaseWidget(TrackerPhase* _module);
	//void onSelect(const SelectEvent &e) override;
	//void onDeselect(const DeselectEvent &e) override;
	//void appendContextMenu(Menu *menu) override;
};

#endif
