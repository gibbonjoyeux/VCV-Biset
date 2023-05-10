
#ifndef TRACKERSYNTH_HPP
#define TRACKERSYNTH_HPP

#include "../plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TrackerSynth: Module {
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
		OUTPUT_PITCH,
		OUTPUT_GATE,
		OUTPUT_VELOCITY,
		OUTPUT_PANNING,
		ENUMS(OUTPUT_CV, 8),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	TrackerSynth();
	void	process(const ProcessArgs& args) override;
};

struct TrackerSynthDisplay : LedDisplay {
	TrackerSynth			*module;
	ModuleWidget			*moduleWidget;
	std::string				font_path;
	char					str_synth[4];

	TrackerSynthDisplay();
	void draw(const DrawArgs &args) override {};
	void drawLayer(const DrawArgs &args, int layer) override;
};

struct TrackerSynthWidget : ModuleWidget {
	TrackerSynth			*module;

	TrackerSynthWidget(TrackerSynth* _module);
	void onSelect(const SelectEvent &e) override;
	void appendContextMenu(Menu *menu) override;
};

#endif
