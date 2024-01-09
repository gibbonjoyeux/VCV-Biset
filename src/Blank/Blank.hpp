
#ifndef BLANK_HPP
#define BLANK_HPP

#include "../plugin.hpp"

#define BLANK_BUFFER					2048
#define BLANK_DIST_MAX				300.0
#define BLANK_PRECISION				128
#define BLANK_PRECISION_SCOPE			256
#define BLANK_CABLES					256

#define BLANK_CABLE_POLY_FIRST		0
#define BLANK_CABLE_POLY_SUM			1
#define BLANK_CABLE_POLY_SUM_DIVIDED	2

#define BLANK_SCOPE_TOP_LEFT			0
#define BLANK_SCOPE_TOP_RIGHT			1
#define BLANK_SCOPE_BOTTOM_LEFT		2
#define BLANK_SCOPE_BOTTOM_RIGHT		3
#define BLANK_SCOPE_CENTER			4
#define BLANK_SCOPE_CIRCULAR			0
#define BLANK_SCOPE_LINEAR			1
#define BLANK_CABLE_INCOMPLETE_OFF	0
#define BLANK_CABLE_INCOMPLETE_IN		1
#define BLANK_CABLE_INCOMPLETE_OUT	2

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct BlankCable;
struct Blank;
struct BlankWidget;
struct BlankDisplay;
struct BlankScope;

struct BlankCable {
	i64			id;
	math::Vec	pos_in;
	math::Vec	pos_out;
	NVGcolor	color;
	bool		thick;
	float		buffer[BLANK_BUFFER];
};

struct Blank : Module {
	enum	ParamIds {
		PARAM_SCOPE_ENABLED,
		PARAM_SCOPE_BACKGROUND,
		PARAM_SCOPE_DETAILS,
		PARAM_SCOPE_MODE,
		PARAM_SCOPE_POSITION,
		PARAM_SCOPE_SCALE,
		PARAM_SCOPE_ALPHA,
		PARAM_CABLE_ENABLED,		// Cable animation state
		PARAM_CABLE_BRIGHTNESS,		// Cable impacted by brightness
		PARAM_CABLE_LED,			// Cable with leds
		PARAM_CABLE_POLY_THICK,		// Polyphonic cables thicker
		PARAM_CABLE_POLY_MODE,		// Polyphonic cables behavior (1st or sum)
		PARAM_CABLE_SCALE,			// Cable animation scale
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

	int				cable_count;
	int				cable_incomplete;
	BlankCable		cables[BLANK_CABLES + 1];
	int				buffer_i;

	int				scope_index;
	BlankScope		*scope;
	BlankDisplay		*display;

	Blank();
	~Blank();
	void processBypass(const ProcessArgs& args) override;
	void process(const ProcessArgs& args) override;
};

struct BlankWidget : ModuleWidget {
	Blank				*module;

	BlankWidget(Blank *_module);
	void appendContextMenu(Menu *menu) override;
};

struct BlankDisplay : Widget {
	Blank				*module;

	BlankDisplay();
	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
};

struct BlankScope : Widget {
	Blank				*module;

	BlankScope();
	void draw(const DrawArgs &args) override;
};

extern Blank	*g_igc;

#endif
