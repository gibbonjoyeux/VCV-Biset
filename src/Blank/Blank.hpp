
#ifndef BLANK_HPP
#define BLANK_HPP

#include "../plugin.hpp"

#define BLANK_BUFFER					2048
#define BLANK_DIST_MAX					300.0
#define BLANK_PRECISION					128
#define BLANK_PRECISION_SCOPE			256
#define BLANK_CABLES					256
#define BLANK_SCOPE_LABEL_BUFFER		128	// "X output to Y input\0"
#define BLANK_SCOPE_LABEL				55	// (128 - 18) / 2

#define BLANK_CABLE_POLY_FIRST			0
#define BLANK_CABLE_POLY_SUM			1
#define BLANK_CABLE_POLY_SUM_DIVIDED	2

#define BLANK_SCOPE_TOP_LEFT			0
#define BLANK_SCOPE_TOP_RIGHT			1
#define BLANK_SCOPE_BOTTOM_LEFT			2
#define BLANK_SCOPE_BOTTOM_RIGHT		3
#define BLANK_SCOPE_CENTER				4
#define BLANK_SCOPE_CIRCULAR			0
#define BLANK_SCOPE_LINEAR				1
#define BLANK_CABLE_INCOMPLETE_OFF		0
#define BLANK_CABLE_INCOMPLETE_IN		1
#define BLANK_CABLE_INCOMPLETE_OUT		2

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct BlankCable;
struct Blank;
struct BlankWidget;
struct BlankCables;
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
		PARAM_SCOPE_MAJ,			// Scope appears only with MAJ pressed
		PARAM_SCOPE_MODE,			// Scope display mode (circular / redraw)
		PARAM_SCOPE_POSITION,		// Scope position mode
		PARAM_SCOPE_SCALE,			// Scope scale
		PARAM_SCOPE_THICKNESS,		// Scope line thickness
		PARAM_SCOPE_BACK_ALPHA,		// Scope background alpha
		PARAM_SCOPE_VOLT_ALPHA,		// Scope voltage indicator alpha
		PARAM_SCOPE_LABEL_ALPHA,	// Scope port name alpha
		PARAM_SCOPE_ALPHA,			// Scope alpha

		PARAM_CABLE_ENABLED,
		PARAM_CABLE_BRIGHTNESS,		// Cable impacted by brightness
		PARAM_CABLE_LIGHT,			// TODO - Cable plug light
		PARAM_CABLE_POLY_THICK,		// Polyphonic cables thicker
		PARAM_CABLE_POLY_MODE,		// Polyphonic cables behavior (1st or sum)
		PARAM_CABLE_FAST,			// Cable animation computation mode
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
	BlankCables		*display;
	char			scope_label[BLANK_SCOPE_LABEL_BUFFER];

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

struct BlankCables : Widget {
	Blank				*module;

	BlankCables();
	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
};

struct BlankScope : Widget {
	Blank				*module;
	std::string			font_path;

	BlankScope();
	void draw(const DrawArgs &args) override;
};

extern Blank	*g_blank;

#endif
