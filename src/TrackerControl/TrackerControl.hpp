
#ifndef TRACKERCONTROL_HPP
#define TRACKERCONTROL_HPP

#include "../plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TrackerControl : Module {
	enum	ParamIds {
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_PLAY_SONG,
		INPUT_PLAY_PATTERN,
		INPUT_CLOCK,
		INPUT_PATTERN_NEXT,
		INPUT_PATTERN_PREV,
		INPUT_PATTERN_RAND,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};
	dsp::TSchmittTrigger<float>	trigger_play_song;
	dsp::TSchmittTrigger<float>	trigger_play_pattern;
	dsp::TSchmittTrigger<float>	trigger_clock;
	dsp::TSchmittTrigger<float>	trigger_pattern_next;
	dsp::TSchmittTrigger<float>	trigger_pattern_prev;
	dsp::TSchmittTrigger<float>	trigger_pattern_rand;

	bool						clock_master;
	float						clock_phase;
	int							clock_between;
	int							clock_between_count;
	int							clock_count;

	TrackerControl();

	void	process(const ProcessArgs& args) override;
};

struct TrackerControlWidget : ModuleWidget {
	TrackerControl			*module;

	TrackerControlWidget(TrackerControl* _module);
	//void appendContextMenu(Menu *menu) override;
};

#endif
