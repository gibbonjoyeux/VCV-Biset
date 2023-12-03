
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

	configOutput(OUTPUT_LEFT, "Left");
	configOutput(OUTPUT_RIGHT, "Right");

	configParam(PARAM_PITCH, -9, +9, 0, "Pitch");

	for (i = 0; i < 16; ++i) {
		this->phase[i] = 0.0;
		this->out[i] = 0.0;
		for (j = 0; j < 4; ++j)
			for (k = 0; k < 4096; ++k)
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
	float	pitch_main;
	float	p_mod_feedback;
	float	p_mod_feedback_delay;
	float	mod_feedback;
	float	mod_feedback_delay;
	int32_4	mod_feedback_phase;
	float	out;
	int		i;
	int		channel_count;

	p_mod_feedback = params[PARAM_FEEDBACK].getValue();
	p_mod_feedback_delay = params[PARAM_FEEDBACK_DELAY].getValue();

	channel_count = inputs[INPUT_PITCH].getChannels();
	outputs[OUTPUT_LEFT].setChannels(channel_count);
	outputs[OUTPUT_RIGHT].setChannels(channel_count);

	for (i = 0; i < channel_count; ++i) {

		/// COMPUTE PARAMETERS
		mod_feedback = p_mod_feedback
		/**/ * inputs[INPUT_FEEDBACK].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_feedback = mod_feedback * 0.9 + 0.1;
		mod_feedback_delay = p_mod_feedback_delay
		/**/ * inputs[INPUT_FEEDBACK_DELAY].getNormalPolyVoltage(10.0, i) / 10.0;

		//// COMPUTE PITCH
		pitch_main = inputs[INPUT_PITCH].getVoltage(i)
		/**/ + params[PARAM_PITCH].getValue();
		pitch[0] = pitch_main - 0.02;
		pitch[1] = pitch_main - 0.0075;
		pitch[2] = pitch_main + 0.0075;
		pitch[3] = pitch_main + 0.02;

		//// COMPUTE FREQUENCY (Hz)
		freq = dsp::FREQ_C4 * simd::pow(2.f, pitch);

		//// COMPUTE PHASE
		this->phase[i] += freq * args.sampleTime;
		this->phase[i] -= simd::floor(this->phase[i]);

		//// COMPUTE PHASE MODULATION
		mod_feedback_phase = simd::fmod(this->feedback_i
		/**/ - (1 + simd::floor(4096 * mod_feedback_delay * 0.9999)) + 4096,
		/**/ 4096);
		feedback[0] = this->feedback_buffer[i][0][mod_feedback_phase[0]];
		feedback[1] = this->feedback_buffer[i][1][mod_feedback_phase[1]];
		feedback[2] = this->feedback_buffer[i][2][mod_feedback_phase[2]];
		feedback[3] = this->feedback_buffer[i][3][mod_feedback_phase[3]];
		phase = this->phase[i] + (feedback * 0.5 + 0.5) * mod_feedback;

		//// COMPUTE PHASE MODULO
		phase = simd::fmod((simd::fmod(simd::floor(phase * PKM_RESOLUTION),
		/**/ PKM_RESOLUTION) + PKM_RESOLUTION), PKM_RESOLUTION);

		//// COMPUTE WAVEFORM
		this->out[i][0] = this->wave[(int)phase[0]];
		this->out[i][1] = this->wave[(int)phase[1]];
		this->out[i][2] = this->wave[(int)phase[2]];
		this->out[i][3] = this->wave[(int)phase[3]];

		//// COMPUTE OUTPUT
		out = this->out[i][0] + this->out[i][1] + this->out[i][2] + this->out[i][3];
		out *= 0.25;

		//// SEND OUTPUT
		outputs[OUTPUT_LEFT].setVoltage(out * 5.0, i);
		this->feedback_buffer[i][0][this->feedback_i] = this->out[i][0];
		this->feedback_buffer[i][1][this->feedback_i] = this->out[i][1];
		this->feedback_buffer[i][2][this->feedback_i] = this->out[i][2];
		this->feedback_buffer[i][3][this->feedback_i] = this->out[i][3];

	}

	/// [8] UPDATE FEEDBACK DELAY
	this->feedback_i += 1;
		if (this->feedback_i >= 4096)
			this->feedback_i = 0;
}
