
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
	this->length = 0.0;
	this->pos_root = {0.0, 0.0};
	this->pos_tail = {0.0, 0.0};
	this->energy = 0.0;
	this->energy_total = 0.0;
	this->parent = -1;
	this->index = 0;
	this->level = 1;
	this->is_parent = false;
}

void TreeBranch::grow(Tree *tree, int index) {
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
	/// [3] COMPUTE BRANCH POSITION
	if (this->parent < 0)
		this->pos_root = {0, 0};
	else
		this->pos_root = tree->branches[this->parent].pos_tail;
	vec = {std::cos(this->angle_abs), std::sin(angle_abs)};
	this->pos_tail.x = this->pos_root.x + vec.x * this->length;
	this->pos_tail.y = this->pos_root.y + vec.y * this->length;
	/// [4] GIVE BIRTH
	if (this->is_parent == false && tree->branch_count < TREE_BRANCH_MAX)
		if (random::uniform() * 1000.0 < this->length)							// !
			this->birth(tree, index);
}

void TreeBranch::birth(Tree *tree, int index) {
	TreeBranch	*branch;
	int			i;

	this->is_parent = true;
	/// BIRTH TO SINGLE BRANCH (CONTINUOUS)
	if (random::uniform() * 3.0 < this->index) {								// !
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
