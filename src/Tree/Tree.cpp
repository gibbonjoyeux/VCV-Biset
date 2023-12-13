
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

	configParam(PARAM_BRANCH_ANGLE_VARIATION, 0, 1, 1, "Angle variation", "%", 0, 100);
	configParam(PARAM_BRANCH_ANGLE_DIVISION, 0, 1, 1, "Branching variation", "%", 0, 100);
	configParam(PARAM_BRANCH_ANGLE_SUN_FORCE, 0, 1, 0.2, "Sun force", "%", 0, 100);
	configParam(PARAM_BRANCH_DIVISION, 1, 4, 2, "Branching", "")->snapEnabled = true;

	configParam(PARAM_SEQ_LENGTH, 1, 64, 8, "Sequence length", "")->snapEnabled = true;
	configParam(PARAM_SEQ_LENGTH_MOD, 0, 1, 0, "Sequence length mod", "%", 0, 100);
	configParam(PARAM_SEQ_OFFSET, 0, 1, 0, "Sequence offset", "%", 0, 100);
	configParam(PARAM_SEQ_OFFSET_MOD, 0, 1, 0, "Sequence offset mod", "%", 0, 100);
	configParam(PARAM_SEQ_WIND_INFLUENCE, 0, 1, 0, "Sequence wind influence", "%", 0, 100);
	configParam(PARAM_SEQ_WIND_INFLUENCE_MOD, 0, 1, 0, "Sequence wind influence mod", "%", 0, 100);

	configInput(INPUT_TREE_RESET, "Reset tree");
	configInput(INPUT_SEQ_RESET, "Reset sequence");
	configInput(INPUT_SEQ_CLOCK, "Clock sequence");

	configInput(INPUT_SEQ_LENGTH, "Sequence length");
	configInput(INPUT_SEQ_OFFSET, "Sequence offset");
	configInput(INPUT_SEQ_WIND_INFLUENCE, "Sequence wind influence");

	for (i = 0; i < 5; ++i)
		configOutput(OUTPUT + i, rack::string::f("%d", i + 1));

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
	float		wind_force;
	bool		clock;
	int			i;

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
	seq_length = params[PARAM_SEQ_LENGTH].getValue()
	/**/ + params[PARAM_SEQ_LENGTH_MOD].getValue()
	/**/ * (inputs[INPUT_SEQ_LENGTH].getVoltage() / 10.0) * 64.0;
	if (seq_length < 1)
		seq_length = 1;
	if (seq_length > 64)
		seq_length = 64;
	seq_offset = (params[PARAM_SEQ_OFFSET].getValue()
	/**/ + params[PARAM_SEQ_OFFSET_MOD].getValue()
	/**/ * (inputs[INPUT_SEQ_OFFSET].getVoltage() / 10.0))
	/**/ * this->branch_count - seq_length;
	if (seq_offset < 0)
		seq_offset = 0;
	seq_wind = params[PARAM_SEQ_WIND_INFLUENCE].getValue()
	/**/ + params[PARAM_SEQ_WIND_INFLUENCE_MOD].getValue()
	/**/ * (inputs[INPUT_SEQ_WIND_INFLUENCE].getVoltage() / 10.0);
	if (seq_wind < 0.0)
		seq_wind = 0.0;
	if (seq_wind > 1.0)
		seq_wind = 1.0;

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
		branch = &(this->branches[this->branch_read]);
		wind_force = branch->angle_wind_rel * seq_wind * 30.0;
		for (i = 0; i < 5; ++i) {
			outputs[OUTPUT + i].setVoltage(
			/**/ (branch->value[i]) * 10.0 - 5.0 + wind_force);
		}
	}
}

Model* modelTree = createModel<Tree, TreeWidget>("Biset-Tree");
