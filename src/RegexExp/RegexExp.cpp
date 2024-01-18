
#include "RegexExp.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

RegexExp::RegexExp(void) {
	int						i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	for (i = 0; i < 12; ++i)
		configOutput(OUTPUT_EXP + i, rack::string::f("Gate %d", i + 1));
}

void RegexExp::process(const ProcessArgs& args) {
	Regex		*regex;
	Module		*exp;
	RegexSeq	*seq;
	int			i;

	exp = this->leftExpander.module;
	if (exp == NULL)
		return;
	/// EXPANDER REGULAR
	if (exp->model->slug == "Biset-Regex"
	|| exp->model->slug == "Biset-Regex-Condensed") {
		regex = dynamic_cast<Regex*>(exp);
		for (i = 0; i < regex->exp_count; ++i) {
			seq = &(regex->sequences[i]);
			/// MODE CLOCK
			if (seq->mode == REGEX_MODE_CLOCK) {
				if (seq->clock_out_gate.state)
					this->outputs[RegexExp::OUTPUT_EXP + i].setVoltage(10.0);
				else
					this->outputs[RegexExp::OUTPUT_EXP + i].setVoltage(0.0);
			/// MODE PITCH
			} else {
				this->outputs[RegexExp::OUTPUT_EXP + i].setVoltage(
				/**/ - exp->outputs[Regex::OUTPUT_EXP + i].getVoltage());
			}
		}
	}
}

Model* modelRegexExp = createModel<RegexExp, RegexExpWidget>("Biset-Regex-Exp");
