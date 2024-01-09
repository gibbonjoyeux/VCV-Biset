
#ifndef IGC_HPP
#define IGC_HPP

#include "../plugin.hpp"

#define IGC_BUFFER					2048
#define IGC_DIST_MAX				300.0
#define IGC_PRECISION				128
#define IGC_PRECISION_SCOPE			256
#define IGC_CABLES					256

#define IGC_CABLE_POLY_FIRST		0
#define IGC_CABLE_POLY_SUM			1
#define IGC_CABLE_POLY_SUM_DIVIDED	2

#define IGC_SCOPE_TOP_LEFT			0
#define IGC_SCOPE_TOP_RIGHT			1
#define IGC_SCOPE_BOTTOM_LEFT		2
#define IGC_SCOPE_BOTTOM_RIGHT		3
#define IGC_SCOPE_CENTER			4
#define IGC_SCOPE_CIRCULAR			0
#define IGC_SCOPE_LINEAR			1
#define IGC_CABLE_INCOMPLETE_OFF	0
#define IGC_CABLE_INCOMPLETE_IN		1
#define IGC_CABLE_INCOMPLETE_OUT	2

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
	bool		thick;
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
	IgcCable		cables[IGC_CABLES + 1];
	int				buffer_i;

	int				scope_index;
	IgcScope		*scope;
	IgcDisplay		*display;

	Igc();
	~Igc();
	void processBypass(const ProcessArgs& args) override;
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
