
#include "Tree.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Tree::Tree() {
	int		i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configParam(PARAM_BRANCH_ANGLE_VARIATION, 0, 1, 1, "angle variation", "");
	configParam(PARAM_BRANCH_ANGLE_DIVISION, 0, 1, 1, "angle division variation", "");
	configParam(PARAM_BRANCH_ANGLE_SUN_FORCE, 0, 1, 0.2, "sun force", "");
	configParam(PARAM_BRANCH_DIVISION, 1, 4, 2, "branching", "")->snapEnabled = true;

	this->branch_index = 0;
	this->branch_count = 1;
	this->branches[0].init();

	this->phase_l = 0.0;
	this->phase_r = 0.0;

	this->wind_phase = random::uniform() * 10000.0;
	this->wind_force = 0.0;
	this->wind_angle = 0;

	for (i = 0; i < 4096; ++i)
		this->sine[i] = std::sin(((float)i / 4096.0) * 2.0 * M_PI);
}

void Tree::process(const ProcessArgs& args) {

	if (this->trigger_reset.process(this->inputs[INPUT_RESET].getVoltage())) {
		this->branch_index = 0;
		this->branch_count = 1;
		this->branches[0].init();
	}

	/// [1] GROW TREE
	//// GROW BRANCH
	this->branches[this->branch_index].grow(this, this->branch_index);
	//// SWITCH TO NEXT BRANCH
	this->branch_index += 1;
	if (this->branch_index >= this->branch_count)
		this->branch_index = 0;

	/// [2] PROCESS WIND
	//
	// Source: https://developer.nvidia.com/gpugems/gpugems3/part-i-geometry
	// /chapter-6-gpu-generated-procedural-wind-animations-trees
	//
	// Elastic tree :
	//   cos(t * PI) * cos(t * 3PI) * cos(t * 5PI) * cos(t * 7PI)
	//   + sin(t * 25PI) * 0.1
	//
	// Rigid tree :
	//   cos(t * PI)^2 * cos(t * 3PI) * cos(t * 5PI) * 0.5
	//   + sin(t * 25PI) * 0.02
	//
	this->wind_angle = M_PI / 2.0;
	this->wind_phase += 0.01 / args.sampleRate;
	this->wind_phase -= (int)this->wind_phase;
	/// ALGO - ELASTIC TREES
	this->wind_force =
	/**/   this->sine[(int)(this->wind_phase * 4096) % 4096]
	/**/ * this->sine[(int)(this->wind_phase * 3.0 * 4096) % 4096]
	/**/ * this->sine[(int)(this->wind_phase * 5.0 * 4096) % 4096]
	/**/ * this->sine[(int)(this->wind_phase * 7.0 * 4096) % 4096]
	/**/ + this->sine[(int)(this->wind_phase * 25.0 * 4096 + 2048) % 4096] * 0.1;
	/// ALGO - RIGID TREES
	//this->wind_force =
	///**/   this->sine[(int)(this->wind_phase * 4096) % 4096]
	///**/ * this->sine[(int)(this->wind_phase * 4096) % 4096]
	///**/ * this->sine[(int)(this->wind_phase * 3.0 * 4096) % 4096]
	///**/ * this->sine[(int)(this->wind_phase * 5.0 * 4096) % 4096]
	///**/ * 0.5
	///**/ + this->sine[(int)(this->wind_phase * 25.0 * 4096 + 2048) % 4096]
	///**/ * 0.02;
	/// REMAP [-1:+1] -> [0:1]
	//this->wind_force = this->wind_force * 0.5 + 0.5;
	/// REMAP [-1:+1] -> [-0.2:+0.8]
	//this->wind_force = this->wind_force * 0.5 + 0.3;
	/// ALGO - RANDOM FORCE
	//this->wind_force = random::uniform() * 2.0 - 1.0;
}

Model* modelTree = createModel<Tree, TreeWidget>("Biset-Tree");
