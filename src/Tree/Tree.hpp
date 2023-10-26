
#ifndef TREE_HPP
#define TREE_HPP

#include "../plugin.hpp"

#define TREE_BRANCH_MAX		1024

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct Tree;

struct TreeBranch {
	float	angle_abs;		// Fixed absolute angle
	float	angle_rel;		// Fixed relative angle (from parent)
	float	angle_wind;		// Absolute angle pushed by wind
	Vec		fpos_root;		// Fixed start position
	Vec		fpos_tail;		// Fixed end position
	Vec		wpos_root;		// Wind pushed start position
	Vec		wpos_tail;		// Wind pushed end position
	float	length;			// Length
	float	width;			// Width
	float	energy;			// Available energy
	float	energy_total;	// Used energy
	int		parent;			// Parent index
	int		childrens[3];	// Children indexes
	int		children_count;	// Children count
	int		children_read;	// Reading children index
	int		index;			// Count single branching from multi branching
	int		level;			// Branch type (1: single, 2: two branch, 3: tree ...)

	float	phase;			// Playing phase

	void init(void);
	void grow(Tree *tree, int index);
	void birth(Tree *tree, int index);
};

struct Tree : Module {
	enum	ParamIds {
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_LEFT,
		OUTPUT_RIGHT,
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};
	float			sine[4096];
	TreeBranch		branches[TREE_BRANCH_MAX];
	int				branch_count;
	int				branch_index;

	float			phase_l;
	float			phase_r;

	float			wind_phase;
	float			wind_force;
	float			wind_angle;

	Tree();

	void	process(const ProcessArgs& args) override;
};

struct TreeWidget : ModuleWidget {
	Tree			*module;

	TreeWidget(Tree* _module);
};

struct TreeDisplay : LedDisplay {
	Tree			*module;
	ModuleWidget			*moduleWidget;

	TreeDisplay();
	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
};

#endif
