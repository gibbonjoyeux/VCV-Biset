
#ifndef RIVER_HPP
#define RIVER_HPP

#include "../plugin.hpp"
#include <dirent.h>
#include <sys/stat.h>

#define RIVER_DIR	0
#define RIVER_FILE	1
#define RIVER_WAV	2

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct RiverFile {
	std::string		name;
	int				type;

	RiverFile(char *name, int type);
};

struct River : Module {
	enum	ParamIds {
		PARAM_SELECT_WHEEL,
		PARAM_SELECT_CLICK,
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	dsp::TSchmittTrigger<float>	button_select;

	float						*buffer;
	float						buffer_rate;
	size_t						buffer_i;
	size_t						buffer_length;

	std::vector<RiverFile>		dir;
	std::string					dir_path;
	int							dir_selected;

	River();
	~River();
	void process(const ProcessArgs& args) override;

	void buffer_init(void);
	void dir_read(void);
	void dir_open(void);
};

struct RiverWidget : ModuleWidget {
	River			*module;

	RiverWidget(River* _module);
	void onPathDrop(const PathDropEvent &e) override;
};

struct RiverDisplay : LedDisplay {
	River			*module;
	ModuleWidget	*moduleWidget;
	std::string		font_path;

	RiverDisplay();
	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
};

#endif
