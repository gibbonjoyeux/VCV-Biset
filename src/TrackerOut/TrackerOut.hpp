
#ifndef TRACKEROUT_HPP
#define TRACKEROUT_HPP

#include "../plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TrackerOut: Module {
	enum	ParamIds {
		PARAM_SYNTH,
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

	TrackerOut();
	void	process(const ProcessArgs& args) override;
};

struct TrackerOutDisplay : LedDisplay {
	TrackerOut			*module;
	ModuleWidget		*moduleWidget;
	std::string			font_path;
	char				str_synth[4];

	TrackerOutDisplay();
	void draw(const DrawArgs &args) override {};
	void drawLayer(const DrawArgs &args, int layer) override;
};

struct TrackerOutWidget : ModuleWidget {
	TrackerOut					*module;

	TrackerOutWidget(TrackerOut* _module);
	void onSelect(const SelectEvent &e) override;
};

#endif
