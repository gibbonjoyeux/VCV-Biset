
#include "Tree.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void TreeBranch::init(void) {
	this->angle_abs = 0.0;
	this->angle_rel = 0.0;
	this->angle_wind = 0.0;
	this->length = 0.0;
	this->fpos_root = {0.0, 0.0};
	this->fpos_tail = {0.0, 0.0};
	this->wpos_root = {0.0, 0.0};
	this->wpos_tail = {0.0, 0.0};
	this->energy = 0.0;
	this->energy_total = 0.0;
	this->parent = -1;
	this->children_count = 0;
	this->children_read = 0;
	this->index = 0;
	this->level = 1;

	this->phase = 0.0;
}

void TreeBranch::grow(Tree *tree, int index) {
	float	angle;
	Vec		vec;

	/// [1] COMPUTE ENERGY
	if (this->parent < 0)
		this->energy = (tree->branch_count < TREE_BRANCH_MAX) ? 1.0 : 0.0;		// !
	else
		this->energy = tree->branches[this->parent].energy / (float)this->level;
	/// [2] GROW BRANCH
	this->energy_total += energy;
	this->length = std::log(1.0 + this->energy_total);
	//this->width = std::log10(1.0 + this->energy_total);
	this->width = std::exp(this->energy_total / 1000.0);
	if (this->width > 10.0)
		this->width = 10.0;
	/// [3] COMPUTE BRANCH FIXED POSITION
	if (this->parent < 0)
		this->fpos_root = {0, 0};
	else
		this->fpos_root = tree->branches[this->parent].fpos_tail;
	vec = {std::cos(this->angle_abs), std::sin(angle_abs)};
	this->fpos_tail.x = this->fpos_root.x + vec.x * this->length;
	this->fpos_tail.y = this->fpos_root.y + vec.y * this->length;
	/// [4] COMPUTE BRANCH WIND POSITION
	if (this->parent < 0) {
		this->wpos_root = {0, 0};
		this->angle_wind = 0.0;
	} else {
		this->wpos_root = tree->branches[this->parent].wpos_tail;
		this->angle_wind = tree->branches[this->parent].angle_wind
		/**/ + this->angle_rel;
	}
	/// COMPUTE WIND ANGLE IMPACT
	//// OFFSET
	angle = this->angle_wind - M_PI / 2.0;
	//// COMPUTE FORCE
	angle = fmod(fmod(angle, 2.0 * M_PI) + 2.0 * M_PI, 2.0 * M_PI);
	if (angle > M_PI)
		angle -= 2.0 * M_PI;
	angle *= 1.0 - (tree->wind_force * 0.2 / (0.01 + this->width * 1.0));
	//// RE-OFFSET
	this->angle_wind = angle + M_PI / 2.0;

	//this->angle_wind *= 1.0 -
	///**/ (tree->wind_force * 0.2 / (0.01 + this->width * 1.0));

	vec = {std::cos(this->angle_wind), std::sin(angle_wind)};
	this->wpos_tail.x = this->wpos_root.x + vec.x * this->length;
	this->wpos_tail.y = this->wpos_root.y + vec.y * this->length;
	/// [5] GIVE BIRTH
	if (this->children_count == 0 && tree->branch_count < TREE_BRANCH_MAX)
		if (random::uniform() * 1000.0 < this->length)							// !
			this->birth(tree, index);
}

void TreeBranch::birth(Tree *tree, int index) {
	TreeBranch	*branch;
	int			i;

	/// BIRTH TO SINGLE BRANCH (CONTINUOUS)
	if (random::uniform() * 3.0 < this->index) {								// !
		/// ADD CHILDREN TO PARENT
		this->children_count = 1;
		this->childrens[0] = tree->branch_count;
		/// INIT BRANCH
		branch = &(tree->branches[tree->branch_count]);
		branch->init();
		branch->parent = index;
		branch->index = 1;
		/// COMPUTE BRANCH ANGLE
		branch->angle_rel = ((random::uniform() * 2.0) - 1.0) * (M_PI / 8.0);	// !
		branch->angle_abs = this->angle_abs + branch->angle_rel;
		/// COMPUTE BRANCH SUN ATTRACTION
		branch->angle_abs *= 0.8;												// !
		branch->angle_rel = branch->angle_abs - this->angle_abs;
		tree->branch_count += 1;
	/// BIRTH TO MULTI BRANCHES (SPLIT)
	} else {
		for (i = 0; i < 2; ++i) {												// !
			if (tree->branch_count >= TREE_BRANCH_MAX)
				return;
			/// ADD CHILDREN TO PARENT
			this->children_count += 1;
			this->childrens[i] = tree->branch_count;
			/// INIT BRANCH
			branch = &(tree->branches[tree->branch_count]);
			branch->init();
			branch->parent = index;
			branch->index = 2;													// !
			/// COMPUTE BRANCH ANGLE
			branch->angle_rel = ((random::uniform() * 2.0) - 1.0) * (M_PI / 3.0);	// !
			branch->angle_abs = this->angle_abs + branch->angle_rel;
			/// COMPUTE BRANCH SUN ATTRACTION
			branch->angle_abs *= 0.8;												// !
			branch->angle_rel = branch->angle_abs - this->angle_abs;
			tree->branch_count += 1;
		}
	}
}
