
#include "Tree.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static float get_random_value(void) {
	float	value;

	value = random::uniform();
	if (value < 0.0)
		value = 0.0;
	if (value > 1.0)
		value = 1.0;
	return value;
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void TreeBranch::init(void) {
	this->angle_abs = 0.0;
	this->angle_rel = 0.0;
	this->angle_wind_abs = 0.0;
	this->angle_wind_rel = 0.0;
	this->length = 0.0;
	this->fpos_root = {0.0, 0.0};
	this->fpos_tail = {0.0, 0.0};
	this->wpos_root = {0.0, 0.0};
	this->wpos_tail = {0.0, 0.0};
	this->energy = 0.0;
	this->energy_total = 0.0;
	this->parent = -1;
	this->children_count = 0;
	this->solo_count = 0;
	this->level = 1;
}

void TreeBranch::grow(Tree *tree, int index) {
	float	wind_force;
	float	wind_deformation;
	float	angle_aim_abs;
	float	angle_parent;
	float	angle;
	float	phase;
	Vec		vec;

	/// [1] COMPUTE ENERGY
	if (this->parent < 0)
		this->energy = (tree->branch_count < TREE_BRANCH_MAX) ? 1.0 : 0.0;
	else
		this->energy = tree->branches[this->parent].energy / (float)this->level;

	/// [2] GROW BRANCH
	this->energy_total += energy;
	this->length = std::log(1.0 + this->energy_total);
	this->width = std::exp(this->energy_total / 1000.0);
	if (this->width > 10.0)
		this->width = 10.0;

	/// [3] COMPUTE BRANCH WIND POSITION
	if (this->parent < 0) {
		this->wpos_root = {0, 0};
		angle_parent = 0.0;
		angle = 0.0;
	} else {
		this->wpos_root = tree->branches[this->parent].wpos_tail;
		angle_parent = tree->branches[this->parent].angle_wind_abs;
		angle = angle_parent + this->angle_rel;
	}
	/// COMPUTE WIND DEFORMATION
	phase = tree->wind_phase + (float)index * 123.456;
	phase -= (int)phase;
	wind_force =
	/**/   (tree->sine[(int)(phase * 4096) % 4096]
	/**/ * tree->sine[(int)(phase * 3.152 * 4096) % 4096]
	/**/ * tree->sine[(int)(phase * 4.936 * 4096) % 4096]) * 1.0
	/**/ + tree->sine[(int)(phase * 24.967 * 4096 + 2048) % 4096] * 0.05;
	wind_deformation = (wind_force * 0.4) / (1.0 + this->width * 3.0);
	/// COMPUTE WIND DEFORMATION
	angle_aim_abs = angle + wind_deformation * 2.0;
	this->angle_wind_abs = this->angle_wind_abs * 0.9 + angle_aim_abs * 0.1;
	this->angle_wind_rel = this->angle_wind_abs - this->angle_abs;
	/// COMPUTE NEW POSITION
	vec = {std::cos(this->angle_wind_abs), std::sin(this->angle_wind_abs)};
	this->wpos_tail.x = this->wpos_root.x + vec.x * this->length;
	this->wpos_tail.y = this->wpos_root.y + vec.y * this->length;

	/// [4] GIVE BIRTH
	if (this->children_count == 0 && tree->branch_count < TREE_BRANCH_MAX)
		if (random::uniform() * 1000.0 < this->length)
			this->birth(tree, index);
}

void TreeBranch::birth(Tree *tree, int index) {
	TreeBranch	*branch;
	float		angle_variation;
	float		angle_division;
	float		angle_sun_force;
	int			branch_division;
	int			i, j;

	angle_variation = (M_PI / 8.0)
	/**/ * tree->params[Tree::PARAM_BRANCH_ANGLE_VARIATION].getValue();
	angle_division = (M_PI / 3.0)
	/**/ * tree->params[Tree::PARAM_BRANCH_ANGLE_DIVISION].getValue();
	angle_sun_force = 1.0
	/**/ - tree->params[Tree::PARAM_BRANCH_ANGLE_SUN_FORCE].getValue();
	branch_division = tree->params[Tree::PARAM_BRANCH_DIVISION].getValue();

	/// BIRTH TO SINGLE BRANCH (CONTINUOUS)
	if (random::uniform() * 3.0 > this->solo_count) {
		/// ADD CHILDREN TO PARENT
		this->children_count = 1;
		this->childrens[0] = tree->branch_count;
		/// INIT BRANCH
		branch = &(tree->branches[tree->branch_count]);
		branch->init();
		branch->parent = index;
		branch->solo_count = this->solo_count + 1;
		for (j = 0; j < 5; ++j)
			branch->value[j] = get_random_value();
		/// COMPUTE BRANCH ANGLE
		branch->angle_rel = ((random::uniform() * 2.0) - 1.0) * angle_variation;
		branch->angle_abs = this->angle_abs + branch->angle_rel;
		/// COMPUTE BRANCH SUN ATTRACTION
		branch->angle_abs *= angle_sun_force;
		branch->angle_rel = branch->angle_abs - this->angle_abs;
		/// INIT WIND ANGLES
		branch->angle_wind_abs = branch->angle_abs;
		branch->angle_wind_rel = branch->angle_rel;
		tree->branch_count += 1;
	/// BIRTH TO MULTI BRANCHES (SPLIT)
	} else {
		for (i = 0; i < branch_division; ++i) {
			if (tree->branch_count >= TREE_BRANCH_MAX)
				return;
			/// ADD CHILDREN TO PARENT
			this->children_count += 1;
			this->childrens[i] = tree->branch_count;
			/// INIT BRANCH
			branch = &(tree->branches[tree->branch_count]);
			branch->init();
			branch->parent = index;
			branch->solo_count = 1;
			for (j = 0; j < 5; ++j)
				branch->value[j] = get_random_value();
			/// COMPUTE BRANCH ANGLE
			branch->angle_rel = ((random::uniform() * 2.0) - 1.0) * angle_division;
			branch->angle_abs = this->angle_abs + branch->angle_rel;
			/// COMPUTE BRANCH SUN ATTRACTION
			branch->angle_abs *= angle_sun_force;
			branch->angle_rel = branch->angle_abs - this->angle_abs;
			/// INIT WIND ANGLES
			branch->angle_wind_abs = branch->angle_abs;
			branch->angle_wind_rel = branch->angle_rel;
			tree->branch_count += 1;
		}
	}
}
