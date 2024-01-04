
#ifndef IGC_HPP
#define IGC_HPP

#include "../plugin.hpp"

#define IGC_BUFFER				2048	//1024
#define IGC_DIST_MAX			300.0	//150.0
#define IGC_PRECISION			128
#define IGC_PRECISION_SCOPE		256
#define IGC_CABLES				256
#define IGC_SCOPE_TOP_LEFT		0
#define IGC_SCOPE_TOP_RIGHT		1
#define IGC_SCOPE_BOTTOM_LEFT	2
#define IGC_SCOPE_BOTTOM_RIGHT	3
#define IGC_SCOPE_CENTER		4
#define IGC_SCOPE_CIRCULAR		0
#define IGC_SCOPE_LINEAR		1

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct IgcCable;
struct Igc;
struct IgcWidget;
struct IgcDisplay;
struct IgcScope;

struct IgcCable {
	i64			id;
	math::Vec	pos_in;
	math::Vec	pos_out;
	NVGcolor	color;
	float		buffer[IGC_BUFFER];
};

struct Igc : Module {
	enum	ParamIds {
		PARAM_SCOPE_ENABLED,
		PARAM_SCOPE_BACKGROUND,
		PARAM_SCOPE_DETAILS,
		PARAM_SCOPE_MODE,
		PARAM_SCOPE_POSITION,
		PARAM_SCOPE_SCALE,
		PARAM_SCOPE_ALPHA,
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

	int				count;
	i64				ids[IGC_CABLES];
	IgcCable		cables[IGC_CABLES];
	int				buffer_i;

	int				scope_index;
	IgcScope		*scope;

	Igc();
	~Igc();
	void onRemove(const RemoveEvent &e) override;
	void process(const ProcessArgs& args) override;
};

struct IgcWidget : ModuleWidget {
	Igc				*module;

	IgcWidget(Igc *_module);
	void appendContextMenu(Menu *menu) override;
};

struct IgcDisplay : Widget {
	Igc				*module;

	IgcDisplay();
	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
};

struct IgcScope : Widget {
	Igc				*module;

	IgcScope();
	void draw(const DrawArgs &args) override;
};

extern Igc	*g_igc;

#endif
