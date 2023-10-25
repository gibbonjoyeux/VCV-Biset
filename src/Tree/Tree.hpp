
#ifndef TREE_HPP
#define TREE_HPP

#include "../plugin.hpp"

#define TREE_MAX	1024

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct Tree;

struct TreeBranch {
	float	angle_abs;		// Absolute angle
	float	angle_rel;		// Relative angle (from parent)
	Vec		pos_root;		// Start position
	Vec		pos_tail;		// End position
	float	length;			// Length
	float	energy;			// Available energy
	float	energy_total;	// Used energy
	int		parent;			// Parent index
	int		index;			// Count single branching from multi branching
	int		level;			// Branch type (1: single, 2: two branch, 3: tree ...)
	bool	is_parent;		// Is branch parent

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
		OUTPUT_MAIN,
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};
	TreeBranch		branches[TREE_MAX];
	int				branch_count;
	int				branch_index;

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
