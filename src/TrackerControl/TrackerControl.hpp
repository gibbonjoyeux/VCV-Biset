
#ifndef TRACKERCONTROL_HPP
#define TRACKERCONTROL_HPP

#include "../plugin.hpp"

#define TCONTROL_RUN_PATTERN	0
#define TCONTROL_RUN_SONG		1
#define TCONTROL_RUN_TRIGGER	0
#define TCONTROL_RUN_GATE		1
#define TCONTROL_CLOCK_24		0
#define TCONTROL_CLOCK_48		1
#define TCONTROL_CLOCK_96		2
#define TCONTROL_CLOCK_PHASE	3

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TrackerControl : Module {
	enum	ParamIds {
		PARAM_RUN_MODE,
		PARAM_RUN_GATE_MODE,
		PARAM_CLOCK_MODE,
		PARAM_PATTERN_NEXT,
		PARAM_PATTERN_PREV,
		PARAM_PATTERN_RAND,
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_RUN,
		INPUT_CLOCK,
		INPUT_RESET,
		INPUT_PATTERN_NEXT,
		INPUT_PATTERN_PREV,
		INPUT_PATTERN_RAND,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_PATTERN,
		LIGHT_SONG,
		LIGHT_TRIGGER,
		LIGHT_GATE,
		LIGHT_CLOCK_24,
		LIGHT_CLOCK_48,
		LIGHT_CLOCK_96,
		LIGHT_CLOCK_PHASE,
		LIGHT_COUNT
	};
	dsp::TSchmittTrigger<float>	trigger_run;
	dsp::TSchmittTrigger<float>	trigger_clock;
	dsp::TSchmittTrigger<float>	trigger_reset;
	dsp::TSchmittTrigger<float>	trigger_pattern_next;
	dsp::TSchmittTrigger<float>	trigger_pattern_prev;
	dsp::TSchmittTrigger<float>	trigger_pattern_rand;

	bool						clock_master;
	float						clock_phase;
	int							clock_between;
	int							clock_between_count;
	int							clock_count;

	TrackerControl();
	~TrackerControl();

	void	process(const ProcessArgs& args) override;
	void	processBypass(const ProcessArgs& args) override;

	void	play(int mode);
	void	stop(void);
	void	reset(void);
};

struct TrackerControlWidget : ModuleWidget {
	TrackerControl			*module;

	TrackerControlWidget(TrackerControl* _module);
};

#endif
