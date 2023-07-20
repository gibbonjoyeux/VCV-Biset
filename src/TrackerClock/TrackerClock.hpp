
#ifndef TRACKERCLOCK_HPP
#define TRACKERCLOCK_HPP

#include "../plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TrackerClock: Module {
	enum	ParamIds {
		ENUMS(PARAM_FREQ, 4),
		ENUMS(PARAM_PHASE, 4),
		ENUMS(PARAM_PW, 4),
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_COUNT
	};
	enum	OutputIds {
		ENUMS(OUTPUT_CLOCK, 4),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};
	dsp::TSchmittTrigger<float>	global_trigger;
	float						global_phase;
	int							count[4];
	float						phase[4];

	TrackerClock();

	void	process(const ProcessArgs& args) override;
};

struct TrackerClockDisplay : LedDisplay {
	TrackerClock			*module;
	ModuleWidget			*moduleWidget;
	std::string				font_path;
	int						index;

	TrackerClockDisplay();
	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
	void onButton(const ButtonEvent &e) override;
};

struct TrackerClockWidget : ModuleWidget {
	TrackerClock			*module;

	TrackerClockWidget(TrackerClock* _module);
	//void onSelect(const SelectEvent &e) override;
	//void onDeselect(const DeselectEvent &e) override;
	//void appendContextMenu(Menu *menu) override;
};

#endif
