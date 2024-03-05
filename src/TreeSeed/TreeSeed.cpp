
#include "TreeSeed.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TreeSeed::TreeSeed() {
	int		i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configParam(PARAM_THRESHOLD, 0, 1, 0.5, "Threshold", "%", 0, 100);
	configParam(PARAM_THRESHOLD_MOD, -1, 1, 0, "Threshold attenuverter", "%", 0, 100);
	configSwitch(PARAM_RANGE, 0, 10, 1, "Range",
	/**/ {"+/-10v", "+/-5V", "+/-3V", "+/-2V", "+/-1V",
	/**/ "+10v", "+5V", "+3V", "+2V", "+1V"});
	configSwitch(PARAM_RANGE, 0, 1, 0, "Gate", {"Trigger", "Gate"});
	configParam(PARAM_POLYPHONY, 1, 16, 4, "Polyphony")->snapEnabled = true;

	configInput(INPUT_THRESHOLD, "Threshold");
	configOutput(OUTPUT_GATE, "Gate");
	configOutput(OUTPUT_PITCH, "Pitch");
	for (i = 0; i < 3; ++i)
		configOutput(OUTPUT_CV + i, rack::string::f("CV %d", i + 1));

	this->trigger_in_seq.reset();
	for (i = 0; i < 16; ++i) {
		this->trigger_out[i].reset();
		this->gate_out[i].off();
	}
	this->poly_index = 0;
	this->fire = false;
}

void TreeSeed::process(const ProcessArgs& args) {
	Tree		*tree;
	Module		*exp;
	int			gate_mode;
	int			out_gate;
	int			out_trigger;
	float		tree_gate;
	float		tree_pitch;
	float		threshold;
	float		threshold_mod;
	float		threshold_in;
	int			range;
	int			channels;
	int			i;

	gate_mode = this->params[PARAM_GATE_MODE].getValue();
	channels = this->params[PARAM_POLYPHONY].getValue();

	this->outputs[OUTPUT_GATE].setChannels(channels);
	this->outputs[OUTPUT_PITCH].setChannels(channels);
	this->outputs[OUTPUT_CV + 0].setChannels(channels);
	this->outputs[OUTPUT_CV + 1].setChannels(channels);
	this->outputs[OUTPUT_CV + 2].setChannels(channels);

	/// [1] OUTPUT GATE
	for (i = 0; i < 16; ++i) {
		out_trigger = this->trigger_out[i].process(args.sampleTime);
		out_gate = this->gate_out[i].process(args.sampleTime);
		if (i < channels) {
			if (gate_mode == TREE_SEED_GATE_TRIGGER)
				this->outputs[OUTPUT_GATE].setVoltage(out_trigger * 10.0, i);
			else
				this->outputs[OUTPUT_GATE].setVoltage(out_gate * 10.0, i);
		}
	}

	/// [2] GET TREE MODULE
	exp = this->leftExpander.module;
	if (exp == NULL)
		return;
	if (exp->model->slug != "Biset-Tree")
		return;
	tree = dynamic_cast<Tree*>(exp);
	if (tree == NULL)
		return;

	/// [3] COMPUTE SEQUENCE
	if (this->fire) {
		threshold = this->params[PARAM_THRESHOLD].getValue();
		threshold_mod = this->params[PARAM_THRESHOLD_MOD].getValue();
		threshold_in = this->inputs[INPUT_THRESHOLD].getVoltage();
		threshold = (threshold * 10.0) - 5.0 + threshold_in * threshold_mod;
		range = (int)this->params[PARAM_RANGE].getValue();

		tree_gate = tree->outputs[Tree::OUTPUT + 0].getVoltage();
		if (tree_gate >= threshold) {
			/// TRIGGER NOTE
			this->trigger_out[this->poly_index].trigger();
			this->gate_out[this->poly_index].on();
			/// COMPUTE PITCH
			tree_pitch = tree->outputs[Tree::OUTPUT + 1].getVoltage();
			switch (range) {
				case TREE_SEED_RANGE_UNI_10:
					tree_pitch = tree_pitch + 5.0;
					break;
				case TREE_SEED_RANGE_UNI_5:
					tree_pitch = (tree_pitch + 5.0) * 0.5;
					break;
				case TREE_SEED_RANGE_UNI_3:
					tree_pitch = (tree_pitch + 5.0) * 0.3;
					break;
				case TREE_SEED_RANGE_UNI_2:
					tree_pitch = (tree_pitch + 5.0) * 0.2;
					break;
				case TREE_SEED_RANGE_UNI_1:
					tree_pitch = (tree_pitch + 5.0) * 0.1;
					break;
				case TREE_SEED_RANGE_BI_10:
					tree_pitch = tree_pitch * 2.0;
					break;
				case TREE_SEED_RANGE_BI_5:
					break;
				case TREE_SEED_RANGE_BI_3:
					tree_pitch = tree_pitch * 0.6;	// 3 / 5
					break;
				case TREE_SEED_RANGE_BI_2:
					tree_pitch = tree_pitch * 0.4;	// 2 / 5
					break;
				case TREE_SEED_RANGE_BI_1:
					tree_pitch = tree_pitch * 0.2;	// 1 / 5
					break;
			};
			/// OUTPUT SEQUENCE
			this->outputs[OUTPUT_PITCH].setVoltage(tree_pitch, this->poly_index);
			this->outputs[OUTPUT_CV + 0].setVoltage(
			/**/ tree->outputs[Tree::OUTPUT + 2].getVoltage(), this->poly_index);
			this->outputs[OUTPUT_CV + 1].setVoltage(
			/**/ tree->outputs[Tree::OUTPUT + 3].getVoltage(), this->poly_index);
			this->outputs[OUTPUT_CV + 2].setVoltage(
			/**/ tree->outputs[Tree::OUTPUT + 4].getVoltage(), this->poly_index);
			/// LOOP POLYPHONIC CHANNELS
			this->poly_index += 1;
			if (this->poly_index >= channels)
				this->poly_index = 0;
		}
		this->fire = false;
	}

	/// [4] FIRE SEQUENCE (NEXT FRAME)
	if (this->trigger_in_seq.process(
	tree->inputs[Tree::INPUT_SEQ_CLOCK].getVoltage())) {
		this->fire = true;
	}
}

Model* modelTreeSeed = createModel<TreeSeed, TreeSeedWidget>("Biset-Tree-Seed");
