
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
		ENUMS(PARAM_MENU, 2),
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
	ParamHandleRange	map_handles[8][4];
	bool				map_learn;
	int					map_learn_cv;
	int					map_learn_map;

	TrackerSynth();
	~TrackerSynth();
	void	process(const ProcessArgs& args) override;
	json_t	*dataToJson(void) override;
	void	dataFromJson(json_t *j_root) override;

	void	learn_enable(int cv, int map);
	void	learn_disable(void);
	void	learn_map(i64 module_id, int param_id);
};

struct TrackerSynthDisplay : LedDisplay {
	TrackerSynth			*module;
	ModuleWidget			*moduleWidget;
	std::string				font_path;
	char					str_synth[4];

	TrackerSynthDisplay();
	void draw(const DrawArgs &args) override {};
	void drawLayer(const DrawArgs &args, int layer) override;
	void onButton(const ButtonEvent &e) override;
};

struct TrackerSynthWidget : ModuleWidget {
	TrackerSynth			*module;

	TrackerSynthWidget(TrackerSynth* _module);
	void onSelect(const SelectEvent &e) override;
	void onDeselect(const DeselectEvent &e) override;
	void appendContextMenu(Menu *menu) override;
};

#endif
