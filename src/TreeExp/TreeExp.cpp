
#include "TreeExp.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TreeExp::TreeExp() {
	int		i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configParam(PARAM_THRESHOLD, 0, 1, 0.5, "Fire threshold", "%", 0, 100);

	configOutput(OUTPUT_GATE, "Gate");
	configOutput(OUTPUT_PITCH, "Pitch");
	for (i = 0; i < 3; ++i)
		configOutput(OUTPUT_CV + i, rack::string::f("CV %d", i + 1));

	this->trigger_in_seq.reset();
	this->trigger_out.reset();
	this->fire = false;
}

void TreeExp::process(const ProcessArgs& args) {
	Tree		*tree;
	Module		*exp;
	float		out_gate;
	float		out_pitch;
	float		threshold;
	int			range;

	/// OUTPUT TRIGGER
	this->outputs[OUTPUT_GATE]
	/**/ .setVoltage(this->trigger_out.process(args.sampleTime) * 10.0);

	/// CHECK EXPANDER CONNECTION
	exp = this->leftExpander.module;
	if (exp == NULL)
		return;
	if (exp->model->slug == "Biset-Tree") {

		tree = dynamic_cast<Tree*>(exp);

		/// COMPUTE SEQUENCER PROCESS
		if (this->fire) {
			threshold = this->params[PARAM_THRESHOLD].getValue() * 10.0 - 5.0;
			range = (int)this->params[PARAM_RANGE].getValue();

			out_gate = tree->outputs[Tree::OUTPUT + 0].getVoltage();
			if (out_gate >= threshold) {
				/// TRIGGER NOTE
				this->trigger_out.trigger();
				/// COMPUTE PITCH
				out_pitch = tree->outputs[Tree::OUTPUT + 1].getVoltage();
				switch (range) {
					case TREE_EXP_RANGE_UNI_10:
						out_pitch = out_pitch + 5.0;
						break;
					case TREE_EXP_RANGE_UNI_5:
						out_pitch = (out_pitch + 5.0) * 0.5;
						break;
					case TREE_EXP_RANGE_UNI_3:
						out_pitch = (out_pitch + 5.0) * 0.3;
						break;
					case TREE_EXP_RANGE_UNI_2:
						out_pitch = (out_pitch + 5.0) * 0.2;
						break;
					case TREE_EXP_RANGE_UNI_1:
						out_pitch = (out_pitch + 5.0) * 0.1;
						break;
					case TREE_EXP_RANGE_BI_10:
						out_pitch = out_pitch * 2.0;
						break;
					case TREE_EXP_RANGE_BI_5:
						break;
					case TREE_EXP_RANGE_BI_3:
						out_pitch = out_pitch * 0.6;	// 3 / 5
						break;
					case TREE_EXP_RANGE_BI_2:
						out_pitch = out_pitch * 0.4;	// 2 / 5
						break;
					case TREE_EXP_RANGE_BI_1:
						out_pitch = out_pitch * 0.2;	// 1 / 5
						break;
				};
				/// OUTPUT SEQUENCE
				this->outputs[OUTPUT_PITCH].setVoltage(out_pitch);
				this->outputs[OUTPUT_CV + 0].setVoltage(
				/**/ tree->outputs[Tree::OUTPUT + 2].getVoltage());
				this->outputs[OUTPUT_CV + 1].setVoltage(
				/**/ tree->outputs[Tree::OUTPUT + 3].getVoltage());
				this->outputs[OUTPUT_CV + 2].setVoltage(
				/**/ tree->outputs[Tree::OUTPUT + 4].getVoltage());
			}
			this->fire = false;
		}

		/// FIRE SEQUENCER PROCESS (NEXT FRAME)
		if (this->trigger_in_seq.process(
		tree->inputs[Tree::INPUT_SEQ_CLOCK].getVoltage())) {
			this->fire = true;
		}
	}
}

Model* modelTreeExp = createModel<TreeExp, TreeExpWidget>("Biset-Tree-Exp");
