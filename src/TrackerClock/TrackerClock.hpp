
#ifndef TRACKERCLOCK_HPP
#define TRACKERCLOCK_HPP

#include "../plugin.hpp"

#define TCLOCK_MODE_FIXED	0
#define TCLOCK_MODE_LOOP	1

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TrackerClock: Module {
	enum	ParamIds {
		ENUMS(PARAM_FREQ, 4),
		ENUMS(PARAM_PHASE, 4),
		ENUMS(PARAM_PW, 4),
		ENUMS(PARAM_SWING, 4),
		PARAM_MODE,
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
	dsp::TSchmittTrigger<float>	trigger_restart;
	float						phase_play;
	int							count[4];

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
	void appendContextMenu(Menu *menu) override;
};

#endif
