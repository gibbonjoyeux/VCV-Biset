
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
	this->wind_angle = 0;

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

	this->wind_angle = M_PI / 2.0;
	//this->wind_angle = (float)(args.frame / 6.0)
	///**/ / (float)args.sampleRate
	///**/ * 2.0 * M_PI;
	//while (this->wind_angle >= 2.0 * M_PI)
	//	this->wind_angle -= 2.0 * M_PI;

	/// [2] PROCESS SOUND ALGORITHM

	if (this->outputs[OUTPUT_LEFT].isConnected() == false)
		return;

	int	j;
	j = 0;
	this->outputs[OUTPUT_LEFT].setChannels(16);
	out.x = 0.0;
	for (i = 0; i < this->branch_count; ++i) {
		branch = &(this->branches[i]);
		if (branch->children_count == 0) {
			this->outputs[OUTPUT_LEFT].setVoltage(branch->wpos_tail.y / 10.0, j);
			j += 1;
			if (j >= 16)
				break;
		}
	}

	/// ALGO SAW PHASE JUMP CONTINUOUS
	//float phase;
	//branch = &(this->branches[this->branch_index]);
	//freq_base = 55.0;
	//freq = freq_base;
	//// MAIN PHASE
	//this->phase_l += freq / args.sampleRate;
	//this->phase_l -= (int)this->phase_l;
	//// BRANCH PHASE
	//phase = branch->angle_wind * 0.01 * branch->length;
	//// OUT SINE
	////out.x = std::sin((this->phase_l + phase) * 2.0 * M_PI) * 5.0;
	//// OUT SAW
	//out.x = (fmod(this->phase_l + phase, 1.0) * 2.0 - 1.0) * 5.0;

	/// ALGO SAW PHASE JUMP BRANCH BY BRANCH
	//float phase;
	//freq_base = 110.0;
	//freq = freq_base;
	//// MAIN PHASE
	//this->phase_l += freq / args.sampleRate;
	//if (this->phase_l >= 1.0)
	//	this->phase_r += 1.0 / this->branch_count;
	//if (this->phase_r > 1.0)
	//	this->phase_r = 0.0;
	//this->phase_l -= (int)this->phase_l;
	//// BRANCH PHASE
	//branch = &(this->branches[(int)(this->phase_r * this->branch_count)]);
	//phase = branch->angle_wind * 0.01 * branch->length;
	//// OUT SAW
	//out.x = (fmod(this->phase_l + phase, 1.0) * 2.0 - 1.0) * 5.0;

	/// ALGO SINE ADD
	//float a;
	//freq_base = 220.0;
	//freq = freq_base;
	//for (i = 0; i < this->branch_count; ++i) {
	//	branch = &(this->branches[i]);
	//	a = branch->angle_rel - branch->angle_abs + branch->angle_wind;
	//	branch->phase += (freq + a * 10.0) / args.sampleRate;
	//	branch->phase -= (int)branch->phase;
	//	out.x += this->sine[(int)(branch->phase * 4096)]
	//	/**/ * branch->length * 0.01;
	//}

	/// ALGO SAW SWARM
	//float a;
	//freq_base = 110.0;
	//freq = freq_base;
	//for (i = 0; i < this->branch_count; ++i) {
	//	branch = &(this->branches[i]);
	//	a = branch->angle_rel - branch->angle_abs + branch->angle_wind;
	//	branch->phase += (freq + a * 10.0) / args.sampleRate;
	//	branch->phase -= (int)branch->phase;
	//	out.x += (branch->phase * 2.0 - 1.0)
	//	/**/ * branch->length * 0.01;
	//}

	/// ALGO FM
	//float a;
	//freq_base = 110.0;
	//freq = freq_base;
	//for (i = 0; i < this->branch_count; ++i) {
	//	branch = &(this->branches[i]);
	//	a = branch->angle_rel - branch->angle_abs + branch->angle_wind;
	//	branch->phase += (freq_base + a * 20.0) / args.sampleRate;
	//	branch->phase -= (int)branch->phase;
	//	freq += this->sine[(int)(branch->phase * 4096)] * branch->length * 1.0;
	//}
	//this->phase_l += freq / args.sampleRate;
	//this->phase_l -= (int)this->phase_l;
	//out.x = this->sine[(int)(this->phase_l * 4096)] * 5.0;

	/// ALGO PHASE MODULATION RECURSIVE
	//float a;
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
	//		//a = branch->angle_rel - branch->angle_abs + branch->angle_wind;
	//		a = branch->angle_wind;
	//		branch->phase += (freq_base + a * 10.0) / args.sampleRate;
	//		branch->phase -= (int)branch->phase;
	//		if (branch->phase < 0)
	//			branch->phase = 1.0 - branch->phase;
	//		if (branch->parent >= 0) {
	//			/// SINE
	//			this->branches[branch->parent].phase +=
	//			/**/ this->sine[(int)(branch->phase * 4096) % 4096] * branch->length
	//			/**/ * 0.01; // 0.005
	//			/// SAW
	//			//this->branches[branch->parent].phase +=
	//			///**/ (branch->phase * 2.0 - 1.0)
	//			///**/ * branch->length * 0.001;
	//		}

	//		/// GO TO PARENT
	//		branch->children_read = 0;
	//		if (branch->parent < 0)
	//			break;
	//		branch = &(this->branches[branch->parent]);
	//	}
	//}
	///// SINE
	//out.x += this->sine[(int)(this->branches[0].phase * 4096) % 4096] * 5.0;
	///// SAW
	////out.x += (this->branches[0].phase * 2.0 - 1.0) * 5.0;

	//if (out.x > 5.0)
	//	out.x = 5.0;
	//if (out.x < -5.0)
	//	out.x = -5.0;
	//this->outputs[OUTPUT_LEFT].setVoltage(out.x);
	//this->outputs[OUTPUT_RIGHT].setVoltage(out.y);
}

Model* modelTree = createModel<Tree, TreeWidget>("Biset-Tree");
