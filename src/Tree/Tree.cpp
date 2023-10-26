
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

	this->branch_index = 0;
	this->branch_count = 1;
	this->branches[0].init();

	this->phase_l = 0.0;
	this->phase_r = 0.0;

	this->wind_phase = random::uniform() * 10000.0;
	this->wind_force = 0.0;

	for (i = 0; i < 4096; ++i)
		this->sine[i] = std::sin(((float)i / 4096.0) * 2.0 * M_PI);
}

void Tree::process(const ProcessArgs& args) {
	TreeBranch		*branch;
	float			freq;
	float			freq_base;
	Vec				out;
	int				i;

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
	this->wind_phase += 0.05 / args.sampleRate;
	this->wind_phase -= (int)this->wind_phase;
	this->wind_force =
	/**/   this->sine[(int)(this->wind_phase * 4096)]
	/**/ * this->sine[(int)(this->wind_phase * 3.0 * 4096)]
	/**/ * this->sine[(int)(this->wind_phase * 5.0 * 4096)]
	/**/ * this->sine[(int)(this->wind_phase * 7.0 * 4096)]
	/**/ + this->sine[(int)(this->wind_phase * 25.0 * 4096 + 2048) % 4096] * 0.1;
	//this->wind_force =
	///**/   this->sine[(int)(this->wind_phase * 4096)]
	///**/ * this->sine[(int)(this->wind_phase * 4096)]
	///**/ * this->sine[(int)(this->wind_phase * 3.0 * 4096)]
	///**/ * this->sine[(int)(this->wind_phase * 5.0 * 4096)]
	///**/ * 0.5
	///**/ + this->sine[(int)(this->wind_phase * 25.0 * 4096 + 2048) % 4096] * 0.02;

	/// [2] PROCESS SOUND ALGORITHM

	/// ALGO SINE ADD
	//freq_base = 110.0;
	//freq = freq_base;
	//for (i = 0; i < this->branch_count; ++i) {
	//	branch = &(this->branches[i]);
	//	branch->phase += (freq + branch->angle_rel * 10.0) / args.sampleRate;
	//	branch->phase -= (int)branch->phase;
	//	out.x += this->sine[(int)(branch->phase * 4096)]
	//	/**/ * branch->length * 0.01;
	//}

	/// ALGO SAW SWARM
	//freq_base = 110.0;
	//freq = freq_base;
	//for (i = 0; i < this->branch_count; ++i) {
	//	branch = &(this->branches[i]);
	//	branch->phase += (freq + branch->angle_rel * 5.0) / args.sampleRate;
	//	branch->phase -= (int)branch->phase;
	//	out.x += (branch->phase * 2.0 - 1.0)
	//	/**/ * branch->length * 0.01;
	//}

	/// ALGO FM
	//freq_base = 110.0;
	//freq = freq_base;
	//for (i = 0; i < this->branch_count; ++i) {
	//	branch = &(this->branches[i]);
	//	branch->phase += (freq_base + branch->angle_rel * 20.0) / args.sampleRate;
	//	branch->phase -= (int)branch->phase;
	//	freq += this->sine[(int)(branch->phase * 4096)] * branch->length * 1.0;
	//}
	//this->phase_l += freq / args.sampleRate;
	//this->phase_l -= (int)this->phase_l;
	//out.x = this->sine[(int)(this->phase_l * 4096)] * 5.0;

	/// ALGO PHASE MODULATION RECURSIVE
	//freq_base = 55.0;
	//freq = freq_base;
	//branch = &(this->branches[0]);
	//while (true) {
	//	/// GO TO CHILDREN
	//	if (branch->children_read < branch->children_count) {
	//		branch->children_read += 1;
	//		branch = &(this->branches[branch->children_read - 1]);
	//	/// GO TO PARENT
	//	} else {
	//		/// PHASE MODULATION
	//		branch->phase += (freq_base + branch->angle_rel * 20.0) / args.sampleRate;
	//		branch->phase -= (int)branch->phase;
	//		if (branch->phase < 0)
	//			branch->phase = 1.0 - branch->phase;
	//		if (branch->parent >= 0) {
	//			/// SINE
	//			//this->branches[branch->parent].phase +=
	//			///**/ this->sine[(int)(branch->phase * 4096)] * branch->length
	//			///**/ * 0.005;
	//			/// SAW
	//			this->branches[branch->parent].phase +=
	//			/**/ (branch->phase * 2.0 - 1.0)
	//			/**/ * branch->length * 0.05;
	//		}

	//		/// GO TO PARENT
	//		branch->children_read = 0;
	//		if (branch->parent < 0)
	//			break;
	//		branch = &(this->branches[branch->parent]);
	//	}
	//}
	///// SINE
	////out.x += this->sine[(int)(this->branches[0].phase * 4096)] * 5.0;
	///// SAW
	//out.x += (this->branches[0].phase * 2.0 - 1.0) * 5.0;

	//if (out.x > 5.0)
	//	out.x = 5.0;
	//if (out.x < -5.0)
	//	out.x = -5.0;
	//this->outputs[OUTPUT_LEFT].setVoltage(out.x);
	//this->outputs[OUTPUT_RIGHT].setVoltage(out.y);
}

Model* modelTree = createModel<Tree, TreeWidget>("Biset-Tree");
