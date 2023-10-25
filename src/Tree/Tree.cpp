
#include "Tree.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Tree::Tree() {
	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	this->branch_index = 0;
	this->branch_count = 1;
	this->branches[0].init();
}

void Tree::process(const ProcessArgs& args) {
	/// [1] GROW TREE
	//// GROW BRANCH
	this->branches[this->branch_index].grow(this, this->branch_index);
	//// SWITCH TO NEXT BRANCH
	this->branch_index += 1;
	if (this->branch_index >= this->branch_count)
		this->branch_index = 0;

	/// [2] PROCESS SOUND ALGORITHM
	TreeBranch		*branch;
	float			freq;
	float			out;

	branch = &(this->branches[this->branch_index]);
	freq = 220.0;
	out = std::sin(((float)args.frame / 48000.0) * freq * 2.0 * M_PI);
	this->outputs[OUTPUT_LEFT].setVoltage(out);
	this->outputs[OUTPUT_RIGHT].setVoltage(out);
}

Model* modelTree = createModel<Tree, TreeWidget>("Biset-Tree");
