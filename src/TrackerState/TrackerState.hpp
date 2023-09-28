
#ifndef TRACKERSTATE_HPP
#define TRACKERSTATE_HPP

#include "../plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TrackerState: Module {
	enum	ParamIds {
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_GATE,
		OUTPUT_PLAY,
		OUTPUT_STOP,
		OUTPUT_PLAY_STOP,
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	TrackerState();

	void	process(const ProcessArgs& args) override;
};

struct TrackerStateWidget : ModuleWidget {
	TrackerState			*module;

	TrackerStateWidget(TrackerState* _module);
};

struct TrackerStateDisplay : LedDisplay {
	TrackerState			*module;
	ModuleWidget			*moduleWidget;

	TrackerStateDisplay();
	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
};

#endif
