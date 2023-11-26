
#include "Gbu.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Gbu::Gbu() {
	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configParam(PARAM_FREQ_1, -9, +9, 0, "Freq Good");//->snapEnabled = true;
	configParam(PARAM_FREQ_2, -9, +9, 0, "Freq Bad");//->snapEnabled = true;
	configParam(PARAM_FREQ_3, -9, +9, 0, "Freq Ugly");//->snapEnabled = true;

	configParam(PARAM_FEEDBACK_1, 0, 1, 0, "Feedback Good");
	configParam(PARAM_FEEDBACK_2, 0, 1, 0, "Feedback Bad");
	configParam(PARAM_FEEDBACK_3, 0, 1, 0, "Feedback Ugly");

	configParam(PARAM_FM_1_2, 0, 1, 0, "FM Good -> Bad");
	configParam(PARAM_FM_2_1, 0, 1, 0, "FM Bad -> Good");
	configParam(PARAM_FM_3_1, 0, 1, 0, "FM Ugly -> Good");
	configParam(PARAM_FM_3_2, 0, 1, 0, "FM Ugly -> Bad");
	configParam(PARAM_RM_1_2_1, -1, +1, 0, "RM Good <-> Bad");
	configParam(PARAM_RM_3_1, 0, 1, 0, "RM Ugly -> Good");
	configParam(PARAM_RM_3_2, 0, 1, 0, "RM Ugly -> Bad");

	this->phase_1 = 0.0;
	this->phase_2 = 0.0;
	this->phase_3 = 0.0;
	this->out_1 = 0.0;
	this->out_2 = 0.0;
	this->out_3 = 0.0;
	this->pitch_3 = 0.0;
	this->pitch_3_acc = 0.0;
}

void Gbu::process(const ProcessArgs& args) {
	float	mod_fm_1_2;
	float	mod_fm_2_1;
	float	mod_fm_3_1;
	float	mod_fm_3_2;
	float	mod_rm;
	float	mod_rm_1_2;
	float	mod_rm_2_1;
	float	mod_rm_3_1;
	float	mod_rm_3_2;
	float	mod_feedback_1;
	float	mod_feedback_2;
	float	mod_feedback_3;
	float	pitch_1;
	float	pitch_2;
	float	pitch_3;
	float	freq;
	float	phase;

	/// [1] GET PARAMETERS
	//// MODULATION RING
	mod_rm = this->params[PARAM_RM_1_2_1].getValue();
	if (mod_rm >= 0) {
		mod_rm_2_1 = mod_rm;
		mod_rm_1_2 = 0.0;
	} else {
		mod_rm_2_1 = 0.0;
		mod_rm_1_2 = -mod_rm;
	}
	mod_rm_3_1 = this->params[PARAM_RM_3_1].getValue();
	mod_rm_3_2 = this->params[PARAM_RM_3_2].getValue();
	//// MODULATION FREQUENCY
	mod_fm_2_1 = this->params[PARAM_FM_2_1].getValue();
	mod_fm_3_1 = this->params[PARAM_FM_3_1].getValue();
	mod_fm_1_2 = this->params[PARAM_FM_1_2].getValue();
	mod_fm_3_2 = this->params[PARAM_FM_3_2].getValue();
	//// MODULATION FEEDBACK
	mod_feedback_1 = this->params[PARAM_FEEDBACK_1].getValue();
	mod_feedback_2 = this->params[PARAM_FEEDBACK_2].getValue();
	mod_feedback_3 = this->params[PARAM_FEEDBACK_3].getValue();
	//// PITCH
	pitch_1 = this->inputs[INPUT_PITCH_1].getVoltage();
	if (this->inputs[INPUT_PITCH_2].isConnected())
		pitch_2 = this->inputs[INPUT_PITCH_2].getVoltage();
	else
		pitch_2 = pitch_1;
	if (this->inputs[INPUT_PITCH_3].isConnected())
		pitch_3 = this->inputs[INPUT_PITCH_3].getVoltage();
	else
		pitch_3 = pitch_1;
	pitch_1 += this->params[PARAM_FREQ_1].getValue();
	pitch_2 += this->params[PARAM_FREQ_2].getValue();
	pitch_3 += this->params[PARAM_FREQ_3].getValue();

	/// [2] COMPUTE GOOD
	//// COMPUTE FREQUENCY (Hz)
	freq = dsp::FREQ_C4 * std::pow(2.f, pitch_1);
	//// COMPUTE PHASE
	this->phase_1 += freq * args.sampleTime;
	while (this->phase_1 >= 1.0)
		this->phase_1 -= 1.0;
	//// COMPUTE PHASE MODULATION
	phase = this->phase_1
	///// MODULATION FREQUENCY (FROM BAD)
	/**/ + (this->out_2 * 0.5 + 0.5) * mod_fm_2_1 * 5.0
	///// MODULATION FREQUENCY (FROM UGLY)
	/**/ + (this->out_3 * 0.5 + 0.5) * mod_fm_3_1 * 5.0
	///// MODULATION FEEDBACK
	/**/ + (this->out_1 * 0.5 + 0.5) * mod_feedback_1;
	//// COMPUTE OUTPUT
	this->out_1 = std::sin(2.0 * M_PI * phase);
	//// COMPUTE AMPLITUDE MODULATION
	this->out_1 *=
	///// MODULATION RING (FROM BAD)
	/**/ ((1.0 - mod_rm_2_1) + (this->out_2 * mod_rm_2_1))
	///// MODULATION RING (FROM UGLY)
	/**/ * ((1.0 - mod_rm_3_1) + (this->out_3 * mod_rm_3_1));
	//// SEND OUTPUT
	this->outputs[OUTPUT_LEFT].setVoltage(this->out_1 * 5.0);

	/// [3] COMPUTE BAD
	//// COMPUTE FREQUENCY (Hz)
	freq = dsp::FREQ_C4 * std::pow(2.f, pitch_2);
	//// COMPUTE PHASE
	this->phase_2 += freq * args.sampleTime;
	while (this->phase_2 >= 1.0)
		this->phase_2 -= 1.0;
	//// COMPUTE PHASE MODULATION
	phase = this->phase_2
	///// MODULATION FREQUENCY (FROM GOOD)
	/**/ + (this->out_1 * 0.5 + 0.5) * mod_fm_1_2 * 5.0
	///// MODULATION FREQUENCY (FROM UGLY)
	/**/ + (this->out_3 * 0.5 + 0.5) * mod_fm_3_2 * 5.0
	///// MODULATION FEEDBACK
	/**/ + (this->out_2 * 0.5 + 0.5) * mod_feedback_2;
	//// COMPUTE OUTPUT
	this->out_2 = std::sin(2.0 * M_PI * phase);
	//// COMPUTE AMPLITUDE MODULATION
	this->out_2 *=
	///// MODULATION RING (FROM BAD)
	/**/ ((1.0 - mod_rm_1_2) + (this->out_1 * mod_rm_1_2))
	///// MODULATION RING (FROM UGLY)
	/**/ * ((1.0 - mod_rm_3_2) + (this->out_3 * mod_rm_3_2));
	//// SEND OUTPUT
	this->outputs[OUTPUT_RIGHT].setVoltage(this->out_2 * 5.0);

	/// [4] COMPUTE UGLY
	//// COMPUTE PITCH MOVEMENT
	if (this->pitch_3 > pitch_3)
		this->pitch_3_acc -= args.sampleTime * 0.05;
	else
		this->pitch_3_acc += args.sampleTime * 0.05;
	this->pitch_3_acc *= 0.9990;
	this->pitch_3 += this->pitch_3_acc;
	pitch_3 = this->pitch_3;
	//// COMPUTE FREQUENCY (Hz)
	freq = dsp::FREQ_C4 * std::pow(2.f, pitch_3);
	//// COMPUTE PHASE
	this->phase_3 += freq * args.sampleTime;
	while (this->phase_3 >= 1.0)
		this->phase_3 -= 1.0;
	//// COMPUTE PHASE MODULATION
	phase = this->phase_3
	///// MODULATION FEEDBACK
	/**/ + (this->out_3 * 0.5 + 0.5) * mod_feedback_3;
	//// COMPUTE OUTPUT
	this->out_3 = std::sin(2.0 * M_PI * phase);
	//// SEND OUTPUT
	this->outputs[OUTPUT_RIGHT].setVoltage(this->out_3 * 5.0); // TODO: Remove
	this->outputs[OUTPUT_DEBUG].setVoltage(pitch_3);
}

Model* modelGbu = createModel<Gbu, GbuWidget>("Biset-Gbu");
