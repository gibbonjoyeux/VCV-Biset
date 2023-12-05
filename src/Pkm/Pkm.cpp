
#include "Pkm.hpp"

#define PKM_WAVE(f)	this->wave[((((int)(f * PKM_RESOLUTION) % PKM_RESOLUTION) \
						+ PKM_RESOLUTION) % PKM_RESOLUTION)]

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// PLEASE KILL ME

Pkm::Pkm() {
	int		i, j, k;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configInput(INPUT_PITCH, "V/Oct");
	configInput(INPUT_FEEDBACK, "Feedback");
	configInput(INPUT_FEEDBACK_DELAY, "Delay");

	configOutput(OUTPUT_LEFT, "Left");
	configOutput(OUTPUT_RIGHT, "Right");

	configParam(PARAM_PITCH, -9, +9, 0, "Pitch");
	configParam(PARAM_DETUNE, 0, 1, 0.2, "Detune");
	configParam(PARAM_WIDTH, 0, 1, 1.0, "Width");
	configParam(PARAM_FEEDBACK, 0, 1, 0, "Feedback");
	configParam(PARAM_FEEDBACK_DELAY, 0, 1, 0.1, "Delay");
	configParam(PARAM_FEEDBACK_MOD, -1, +1, 0, "Feedback modulation");
	configParam(PARAM_FEEDBACK_DELAY_MOD, -1, +1, 0, "Delay modulation");

	for (i = 0; i < 16; ++i) {
		this->phase[i] = 0.0;
		this->out[i] = 0.0;
		for (j = 0; j < 4; ++j)
			for (k = 0; k < PKM_FEEDBACK; ++k)
			this->feedback_buffer[i][j][k] = 0.0;
	}
	this->feedback_i = 0;

	for (i = 0; i < PKM_RESOLUTION; ++i)
		this->wave[i] = std::sin(2.0 * M_PI * (i / (float)PKM_RESOLUTION));
}

void Pkm::process(const ProcessArgs& args) {
	float_4	pitch;
	float_4	freq;
	float_4	phase;
	float_4	feedback;
	float_4	pan;
	float_4	out_l;
	float_4	out_r;
	float	pitch_main;
	float	p_detune;
	float	p_width;
	float	p_mod_feedback;
	float	p_mod_feedback_mod;
	float	p_mod_feedback_delay;
	float	p_mod_feedback_delay_mod;
	float	mod_feedback;
	float	mod_feedback_delay;
	int		mod_feedback_phase;
	int		channel_count;
	int		i;

	p_mod_feedback = params[PARAM_FEEDBACK].getValue();
	p_mod_feedback_mod = params[PARAM_FEEDBACK_MOD].getValue();
	p_mod_feedback_delay = params[PARAM_FEEDBACK_DELAY].getValue();
	p_mod_feedback_delay_mod = params[PARAM_FEEDBACK_DELAY_MOD].getValue();
	p_detune = params[PARAM_DETUNE].getValue();
	p_width = params[PARAM_WIDTH].getValue();

	channel_count = inputs[INPUT_PITCH].getChannels();
	outputs[OUTPUT_LEFT].setChannels(channel_count);
	outputs[OUTPUT_RIGHT].setChannels(channel_count);

	for (i = 0; i < channel_count; ++i) {

		/// COMPUTE PARAMETERS
		mod_feedback = p_mod_feedback + p_mod_feedback_mod
		/**/ * inputs[INPUT_FEEDBACK].getPolyVoltage(i) / 5.0;
		mod_feedback = mod_feedback * 0.9 + 0.1;
		mod_feedback_delay = p_mod_feedback_delay + p_mod_feedback_delay_mod
		/**/ * inputs[INPUT_FEEDBACK_DELAY].getPolyVoltage(i) / 5.0;
		if (mod_feedback_delay < 0.0)
			mod_feedback_delay = 0.0;
		if (mod_feedback_delay > 1.0)
			mod_feedback_delay = 1.0;

		//// COMPUTE PITCH
		pitch_main = inputs[INPUT_PITCH].getVoltage(i)
		/**/ + params[PARAM_PITCH].getValue();
		pitch[0] = pitch_main - 0.1 * p_detune;
		pitch[1] = pitch_main - 0.0333 * p_detune;
		pitch[2] = pitch_main + 0.0333 * p_detune;
		pitch[3] = pitch_main + 0.1 * p_detune;

		//// COMPUTE FREQUENCY (Hz)
		freq = dsp::FREQ_C4 * simd::pow(2.f, pitch);

		//// COMPUTE PHASE
		this->phase[i] += freq * args.sampleTime;
		this->phase[i] -= simd::floor(this->phase[i]);

		//// COMPUTE PHASE MODULATION
		mod_feedback_phase = (this->feedback_i
		/**/ - (1 + (int)(PKM_FEEDBACK * mod_feedback_delay * 0.9999))
		/**/ + PKM_FEEDBACK) % PKM_FEEDBACK;
		feedback[0] = this->feedback_buffer[i][0][mod_feedback_phase];
		feedback[1] = this->feedback_buffer[i][1][mod_feedback_phase];
		feedback[2] = this->feedback_buffer[i][2][mod_feedback_phase];
		feedback[3] = this->feedback_buffer[i][3][mod_feedback_phase];
		phase = this->phase[i] + (feedback * 0.5 + 0.5) * mod_feedback;

		//// COMPUTE PHASE MODULO
		phase = simd::fmod((simd::fmod(simd::floor(phase * PKM_RESOLUTION),
		/**/ PKM_RESOLUTION) + PKM_RESOLUTION), PKM_RESOLUTION);

		//// COMPUTE WAVEFORM
		this->out[i][0] = this->wave[(int)phase[0]];
		this->out[i][1] = this->wave[(int)phase[1]];
		this->out[i][2] = this->wave[(int)phase[2]];
		this->out[i][3] = this->wave[(int)phase[3]];

		//// COMPUTE FEEDBACK OUTPUT
		this->feedback_buffer[i][0][this->feedback_i] = this->out[i][0];
		this->feedback_buffer[i][1][this->feedback_i] = this->out[i][1];
		this->feedback_buffer[i][2][this->feedback_i] = this->out[i][2];
		this->feedback_buffer[i][3][this->feedback_i] = this->out[i][3];
	
		//// COMPUTE STEREO OUTPUT PANNING
		pan[0] = 0.5 - (p_width * 0.5);
		pan[1] = 0.5 + (p_width * 0.2);
		pan[2] = 0.5 - (p_width * 0.2);
		pan[3] = 0.5 + (p_width * 0.5);
		out_l = this->out[i] * pan;
		out_r = this->out[i] * (1.0 - pan);

		//// SEND STEREO OUTPUT
		outputs[OUTPUT_LEFT].setVoltage(
		/**/ (out_l[0] + out_l[1] + out_l[2] + out_l[3]) * 0.25 * 5.0, i);
		outputs[OUTPUT_RIGHT].setVoltage(
		/**/ (out_r[0] + out_r[1] + out_r[2] + out_r[3]) * 0.25 * 5.0, i);

	}

	/// [8] UPDATE FEEDBACK DELAY
	this->feedback_i += 1;
		if (this->feedback_i >= PKM_FEEDBACK)
			this->feedback_i = 0;
}

Model* modelPkm = createModel<Pkm, PkmWidget>("Biset-Pkm");
