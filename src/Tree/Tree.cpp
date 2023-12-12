
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

	configParam(PARAM_SEQ_LENGTH, 1, 64, 8, "Sequence length", "")->snapEnabled = true;
	configParam(PARAM_SEQ_OFFSET, 0, 1, 0, "Sequence offset", "%", 0, 100);
	configParam(PARAM_SEQ_WIND_INFLUENCE, 0, 1, 0, "Sequence wind influence", "%", 0, 100);

	this->branch_index = 0;
	this->branch_count = 1;
	this->branch_read = 0;
	this->branch_read_phase = 0;
	this->branches[0].init();

	this->tree_max.x = 1.0;
	this->tree_max.y = 1.0;

	this->phase_l = 0.0;
	this->phase_r = 0.0;

	this->wind_phase = random::uniform() * 10000.0;
	this->wind_force = 0.0;
	this->wind_angle = 0;

	for (i = 0; i < 4096; ++i)
		this->sine[i] = std::sin(((float)i / 4096.0) * 2.0 * M_PI);
}

void Tree::process(const ProcessArgs& args) {
	TreeBranch	*branch;
	int			seq_length;
	int			seq_offset;
	float		seq_wind;
	bool		clock;

	/// [1] HANDLE INPUTS
	clock = false;
	if (this->trigger_tree_reset.process(inputs[INPUT_TREE_RESET].getVoltage())) {
		this->branch_index = 0;
		this->branch_count = 1;
		this->branches[0].init();
	}
	if (this->trigger_seq_reset.process(inputs[INPUT_SEQ_RESET].getVoltage())) {
		this->branch_read_phase = 0;
	}
	if (this->trigger_seq_clock.process(inputs[INPUT_SEQ_CLOCK].getVoltage())) {
		clock = true;
	}

	/// [2] HANDLE PARAMETERS
	seq_length = params[PARAM_SEQ_LENGTH].getValue();
	seq_offset = params[PARAM_SEQ_OFFSET].getValue() * this->branch_count
	/**/ - seq_length;
	if (seq_offset < 0)
		seq_offset = 0;
	seq_wind = params[PARAM_SEQ_WIND_INFLUENCE].getValue();

	/// [3] COMPUTE TREE GROWTH
	//// GROW BRANCH
	this->branches[this->branch_index].grow(this, this->branch_index);
	//// SWITCH TO NEXT BRANCH
	this->branch_index += 1;
	if (this->branch_index >= this->branch_count)
		this->branch_index = 0;

	/// [4] COMPUTE WIND PHASE
	this->wind_angle = M_PI / 2.0;
	this->wind_phase += 0.01 / args.sampleRate;
	this->wind_phase -= (int)this->wind_phase;

	/// [5] COMPUTE SEQUENCE
	if (clock) {
		/// COMPUTE SEQUENCE
		this->branch_read_phase += 1;
		if (this->branch_read_phase >= seq_length)
			this->branch_read_phase = 0;
		this->branch_read = (this->branch_read_phase + seq_offset)
		/**/ % this->branch_count;

		/// OUTPUT SEQUENCE
		// TODO: adding angle_wind_rel tends to increase value
		branch = &(this->branches[this->branch_read]);
		outputs[OUTPUT_X].setVoltage(
		/**/ (branch->value_a) * 10.0 - 5.0
		/**/ + branch->angle_wind_rel * seq_wind * 30.0);
		outputs[OUTPUT_Y].setVoltage(
		/**/ (branch->value_b) * 10.0 - 5.0
		/**/ + branch->angle_wind_rel * seq_wind * 30.0);
	}
}

Model* modelTree = createModel<Tree, TreeWidget>("Biset-Tree");
