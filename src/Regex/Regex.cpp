
#include "Regex.hpp"

int	table_pitch_midi[] = {
	9,	// A
	11,	// B
	0,	// C
	2,	// D
	4,	// E
	5,	// F
	7,	// G
};

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Regex::Regex() {
	int						i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);
	/// COMFIG PARAMS
	for (i = 0; i < 8; ++i)
		configSwitch(PARAM_MODE + i, 0, 1, 0, string::f("Mode %d", i + 1), {"Clock", "Pitch"});
	/// CONFIG INPUTS / OUTPUTS
	configInput(INPUT_MASTER, "Master clock");
	for (i = 0; i < 8; ++i) {
		configInput(INPUT_EXP_RESET + i, string::f("Reset %d", i + 1));
		configInput(INPUT_EXP_1 + i, string::f("%d:1", i + 1));
		configInput(INPUT_EXP_2 + i, string::f("%d:2", i + 1));
		configOutput(OUTPUT_EXP_EOC + i, string::f("EOC %d", i + 1));
		configOutput(OUTPUT_EXP + i, string::f("Out %d", i + 1));
		this->sequences[i].in_reset = &(this->inputs[INPUT_EXP_RESET + i]);
		this->sequences[i].in_1 = &(this->inputs[INPUT_EXP_1 + i]);
		this->sequences[i].in_2 = &(this->inputs[INPUT_EXP_2 + i]);
		this->sequences[i].out = &(this->outputs[OUTPUT_EXP + i]);
		this->sequences[i].out_eoc = &(this->outputs[OUTPUT_EXP_EOC + i]);
	}
	/// INIT CLOCK
	this->clock_reset.reset();
	this->clock_master.reset();
	/// INIT THREAD FLAG
	this->thread_flag.clear();
}

void Regex::process(const ProcessArgs& args) {
	bool	clock_master;
	bool	clock_reset;
	int		mode;
	int		i;


	/// [1] CHECK THREAD FLAG
	if (this->thread_flag.test_and_set())
		return;

	/// [2] COMPUTE PROCESS
	clock_master = this->clock_master.process(this->inputs[INPUT_MASTER].getVoltage());
	clock_reset = this->clock_reset.process(this->inputs[INPUT_RESET].getVoltage());
	for (i = 0; i < 8; ++i) {
		/// UPDATE SEQUENCES MODES
		if (args.frame % 64 != 0) {
			mode = this->params[PARAM_MODE + i].getValue();
			if (mode != this->sequences[i].mode) {
				this->sequences[i].mode = mode;
				this->sequences[i].reset(true);
				this->sequences[i].display->check_syntax();
				this->sequences[i].display->active_value = -1;
			}
		}
		/// PROCESS SEQUENCE
		this->sequences[i].process(args.sampleTime, clock_reset, clock_master);
	}

	/// [3] CLEAR THREAD FLAG
	this->thread_flag.clear();
}

Model* modelRegex = createModel<Regex, RegexWidget>("Regex");
