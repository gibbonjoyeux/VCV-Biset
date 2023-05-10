
#ifndef TRACKERSYNTH_HPP
#define TRACKERSYNTH_HPP

#include "../plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TrackerDrum: Module {
	enum	ParamIds {
		PARAM_SYNTH,
		ENUMS(PARAM_OUT_MIN, 8),
		ENUMS(PARAM_OUT_MAX, 8),
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_COUNT
	};
	enum	OutputIds {
		ENUMS(OUTPUT_TRIGGER, 12),
		ENUMS(OUTPUT_VELOCITY, 12),
		ENUMS(OUTPUT_PANNING, 12),
		ENUMS(OUTPUT_CV, 8),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	TrackerDrum();
	void	process(const ProcessArgs& args) override;
};

struct TrackerDrumDisplay : LedDisplay {
	TrackerDrum				*module;
	ModuleWidget			*moduleWidget;
	std::string				font_path;
	char					str_synth[4];

	TrackerDrumDisplay();
	void draw(const DrawArgs &args) override {};
	void drawLayer(const DrawArgs &args, int layer) override;
};

struct TrackerDrumWidget : ModuleWidget {
	TrackerDrum				*module;

	TrackerDrumWidget(TrackerDrum* _module);
	void onSelect(const SelectEvent &e) override;
	void appendContextMenu(Menu *menu) override;
};

#endif
