
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

Regex::Regex(bool condensed) {
	int						i;

	this->widget = NULL;
	if (condensed)
		this->exp_count = 12;
	else
		this->exp_count = 8;
	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	/// COMFIG PARAMS
	configParam(PARAM_BIAS, -1, 1, 0, "Bias", "%", 0, 100);
	for (i = 0; i < this->exp_count; ++i)
		configSwitch(PARAM_MODE + i, 0, 1, 0, rack::string::f("Mode %d", i + 1), {"Clock", "Pitch"});
	configSwitch(PARAM_RUN_START, 0, 1, 0, "Run", {"Off", "On"});

	/// CONFIG INPUTS / OUTPUTS
	configInput(INPUT_BIAS, "Orientation");
	configInput(INPUT_RESET, "Master reset");
	configInput(INPUT_MASTER, "Master clock");
	for (i = 0; i < this->exp_count; ++i) {
		configInput(INPUT_EXP_RESET + i, rack::string::f("Reset %d", i + 1));
		configInput(INPUT_EXP_1 + i, rack::string::f("%d:1", i + 1));
		configInput(INPUT_EXP_2 + i, rack::string::f("%d:2", i + 1));
		configOutput(OUTPUT_EXP_EOC + i, rack::string::f("EOC %d", i + 1));
		configOutput(OUTPUT_EXP + i, rack::string::f("Out %d", i + 1));
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
	float	bias;
	int		mode;
	int		i;

	if (args.frame % REGEX_FRAME_DIVIDER != 0)
		return;

	/// [1] CHECK THREAD FLAG
	if (this->thread_flag.test_and_set())
		return;

	/// [2] COMPUTE BIAS
	if (this->inputs[INPUT_BIAS].isConnected()) {
		bias = (this->inputs[INPUT_BIAS].getVoltage() / 5.0)
		/**/ * this->params[PARAM_BIAS].getValue();
		if (bias > 1.0)
			bias = 1.0;
		if (bias < -1.0)
			bias = -1.0;
	} else {
		bias = this->params[PARAM_BIAS].getValue();
	}
	/// [3] COMPUTE PROCESS
	clock_master = this->clock_master.process(this->inputs[INPUT_MASTER].getVoltage());
	clock_reset = this->clock_reset.process(this->inputs[INPUT_RESET].getVoltage());
	for (i = 0; i < this->exp_count; ++i) {
		/// UPDATE SEQUENCES MODES
		mode = this->params[PARAM_MODE + i].getValue();
		if (mode != this->sequences[i].mode) {
			this->sequences[i].mode = mode;
			this->sequences[i].reset(true);
			this->sequences[i].string_active_value = -1;
		}
		/// PROCESS SEQUENCE
		this->sequences[i].process(args.sampleTime * REGEX_FRAME_DIVIDER,
		/**/ clock_reset, clock_master, bias);
	}

	/// [4] CLEAR THREAD FLAG
	this->thread_flag.clear();
}

void Regex::onReset(const ResetEvent &e) {
	int		i;

	Module::onReset(e);
	for (i = 0; i < this->exp_count; ++i) {
		if (this->widget)
			this->widget->display[i]->setText("");
		this->sequences[i].reset(true);
		this->sequences[i].string_edit = "";
		this->sequences[i].string_run = "";
		this->sequences[i].string_run_next = "";
	}
}

Model* modelRegex = createModel<Regex, RegexWidget>("Biset-Regex");
Model* modelRegexCondensed = createModel<RegexCondensed, RegexCondensedWidget>("Biset-Regex-Condensed");
