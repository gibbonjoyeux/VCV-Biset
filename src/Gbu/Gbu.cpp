
#include "Gbu.hpp"

#define GBU_WAVE(f)	this->wave[((((int)(f * GBU_RESOLUTION) % GBU_RESOLUTION) \
						+ GBU_RESOLUTION) % GBU_RESOLUTION)]

//#define GBU_WAVE(f)	(std::sin(2.0 * M_PI * f))

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Gbu::Gbu() {
	int		i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configParam(PARAM_FREQ_GLOBAL, -9, +9, 0, "Freq");//->snapEnabled = true;
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

	configParam(PARAM_NOISE_SPEED, 0, 1, 0.3, "Ugly noise speed");
	configParam(PARAM_NOISE_AMP, 0, 1, 0.5, "Ugly noise amplitude");
	configParam(PARAM_FOLLOW_ATTRACTION, 0, 1, 0.5, "Ugly attraction");
	configParam(PARAM_FOLLOW_FRICTION, 0, 1, 0.5, "Ugly friction");

	this->phase_1 = 0.0;
	this->phase_2 = 0.0;
	this->phase_3 = 0.0;
	this->out_1 = 0.0;
	this->out_2 = 0.0;
	this->out_3 = 0.0;
	this->pitch_3 = 0.0;
	this->pitch_3_acc = 0.0;
	this->pitch_3_noise_phase = 0.0;

	for (i = 0; i < GBU_RESOLUTION; ++i)
		this->wave[i] = std::sin(2.0 * M_PI * (i / (float)GBU_RESOLUTION));
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
	float	noise_speed;
	float	noise_amp;
	float	follow_attraction;
	float	follow_friction;
	float	pitch_global;
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
	pitch_global = this->params[PARAM_FREQ_GLOBAL].getValue();
	pitch_1 = this->inputs[INPUT_PITCH_1].getVoltage();
	if (this->inputs[INPUT_PITCH_2].isConnected())
		pitch_2 = this->inputs[INPUT_PITCH_2].getVoltage();
	else
		pitch_2 = pitch_1;
	if (this->inputs[INPUT_PITCH_3].isConnected())
		pitch_3 = this->inputs[INPUT_PITCH_3].getVoltage();
	else
		pitch_3 = pitch_1;
	pitch_1 += pitch_global + this->params[PARAM_FREQ_1].getValue();
	pitch_2 += pitch_global + this->params[PARAM_FREQ_2].getValue();
	pitch_3 += pitch_global + this->params[PARAM_FREQ_3].getValue();
	//// NOISE
	noise_speed = this->params[PARAM_NOISE_SPEED].getValue();
	noise_amp = this->params[PARAM_NOISE_AMP].getValue();
	//// PITCH FOLLOWING
	follow_attraction = this->params[PARAM_FOLLOW_ATTRACTION].getValue()
	/**/ * (0.01 - 0.0005) + 0.0005;
	follow_friction = (1.0 - this->params[PARAM_FOLLOW_FRICTION].getValue())
	/**/ * (0.9999 - 0.999) + 0.999;

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
	this->out_1 = GBU_WAVE(phase);
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
	this->out_2 = GBU_WAVE(phase);
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
		this->pitch_3_acc -= args.sampleTime * follow_attraction;//0.05
	else
		this->pitch_3_acc += args.sampleTime * follow_attraction;
	this->pitch_3_acc *= follow_friction;//0.999
	this->pitch_3 += this->pitch_3_acc;
	pitch_3 = this->pitch_3;
	//// COMPUTE PITCH RANDOM MOVEMENT
	pitch_3 += noise_amp * 0.3 *
	/**/ ((GBU_WAVE(this->pitch_3_noise_phase)
	/**/ * GBU_WAVE(this->pitch_3_noise_phase * 3.152)
	/**/ * GBU_WAVE(this->pitch_3_noise_phase * 4.936)) * 0.5
	/**/ + GBU_WAVE(this->pitch_3_noise_phase * 24.967 + 0.5) * 0.05);
	this->pitch_3_noise_phase += args.sampleTime * 5.0 * noise_speed;
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
	this->out_3 = GBU_WAVE(phase);
	//// SEND OUTPUT
	//this->outputs[OUTPUT_RIGHT].setVoltage(this->out_3 * 5.0); // TODO: Remove
	this->outputs[OUTPUT_DEBUG].setVoltage(pitch_3);
}

Model* modelGbu = createModel<Gbu, GbuWidget>("Biset-Gbu");
