
#ifndef IGC_HPP
#define IGC_HPP

#include "../plugin.hpp"

#define IGC_BUFFER				480000	// 10s at 48000Hz sample rate
#define IGC_BUFFER_PRECISION	96		// Display precision

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct IgcPlayhead {
	float						phase;
};

struct Igc : Module {
	enum	ParamIds {
		PARAM_DELAY_TIME,
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_L,
		INPUT_R,
		INPUT_PLAYHEAD,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_L,
		OUTPUT_R,
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	dsp::TSchmittTrigger<float>	trigger_env[16];
	IgcPlayhead					playheads[16];
	int							playhead_count;

	float						audio[2][IGC_BUFFER];
	int							audio_index;

	Igc();

	void	process(const ProcessArgs& args) override;
};

struct IgcWidget : ModuleWidget {
	Igc							*module;

	IgcWidget(Igc* _module);
};

struct IgcDisplay : LedDisplay {
	Igc							*module;
	ModuleWidget				*moduleWidget;

	IgcDisplay();
	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
};

#endif
