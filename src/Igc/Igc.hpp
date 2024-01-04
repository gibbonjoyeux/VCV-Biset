
#ifndef IGC_HPP
#define IGC_HPP

#include "../plugin.hpp"

#define IGC_BUFFER			2048	//1024
#define IGC_DIST_MAX		300.0	//150.0
#define IGC_PRECISION		128
#define IGC_CABLES			256

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct IgcCable {
	i64			id;
	math::Vec	pos_in;
	math::Vec	pos_out;
	NVGcolor	color;
	float		buffer[IGC_BUFFER];
};

struct Igc : Module {
	enum	ParamIds {
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

	int						count;
	i64						ids[IGC_CABLES];
	IgcCable				cables[IGC_CABLES];

	int						buffer_i;

	Igc();
	void process(const ProcessArgs& args) override;
};

struct IgcWidget : ModuleWidget {
	Igc				*module;

	IgcWidget(Igc *_module);
	//void appendContextMenu(Menu *menu) override;
};

struct IgcDisplay : LedDisplay {
	Igc				*module;
	ModuleWidget	*moduleWidget;

	IgcDisplay();
	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
};

#endif
