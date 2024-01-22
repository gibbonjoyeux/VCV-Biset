
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

	configSwitch(PARAM_ALGO_SWITCH, 0, 3, 0, "Algorithm",
	/**/ {"Simple", "Double", "Entangled", "Echo"});

	for (i = 0; i < 16; ++i) {
		this->phase_1[i] = 0.0;
		this->phase_2[i] = 0.0;
		this->out_1[i] = 0.0;
		this->out_2[i] = 0.0;
		for (j = 0; j < 4; ++j) {
			for (k = 0; k < PKM_FEEDBACK; ++k) {
				this->feedback_buffer_1[i][j][k] = 0.0;
				this->feedback_buffer_2[i][j][k] = 0.0;
			}
		}
	}
	this->feedback_i = 0;

	for (i = 0; i < PKM_RESOLUTION; ++i)
		this->wave[i] = std::sin(2.0 * M_PI * (i / (float)PKM_RESOLUTION));
}

void Pkm::process(const ProcessArgs& args) {
	float_4	pitch_1, pitch_2;
	float_4	freq_1, freq_2;
	float_4	phase_1, phase_2;
	float_4	feedback_1, feedback_2;
	float_4	pan_1, pan_2;
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
	int		algo;
	int		i;

	//////////////////////////////	
	/// [1] GET ALGORITHM
	//////////////////////////////	
	algo = params[PARAM_ALGO_SWITCH].getValue();
	if (args.frame % 32 == 0) {
		lights[LIGHT_SIMPLE].setBrightness(algo == PKM_ALGO_SIMPLE);
		lights[LIGHT_DOUBLE].setBrightness(algo == PKM_ALGO_DOUBLE);
		lights[LIGHT_ENTANGLED].setBrightness(algo == PKM_ALGO_ENTANGLED);
		lights[LIGHT_ECHO].setBrightness(algo == PKM_ALGO_ECHO);
	}

	//////////////////////////////	
	/// [2] GET PARAMETERS
	//////////////////////////////	
	p_mod_feedback = params[PARAM_FEEDBACK].getValue();
	p_mod_feedback_mod = params[PARAM_FEEDBACK_MOD].getValue();
	p_mod_feedback_delay = params[PARAM_FEEDBACK_DELAY].getValue();
	p_mod_feedback_delay_mod = params[PARAM_FEEDBACK_DELAY_MOD].getValue();
	p_detune = params[PARAM_DETUNE].getValue();
	p_width = params[PARAM_WIDTH].getValue();

	channel_count = inputs[INPUT_PITCH].getChannels();
	if (channel_count == 0)
		channel_count = 1;
	outputs[OUTPUT_LEFT].setChannels(channel_count);
	outputs[OUTPUT_RIGHT].setChannels(channel_count);

	//////////////////////////////	
	/// [3] COMPUTE OUTPUT
	//////////////////////////////	

	/// FOR EACH CHANNEL
	for (i = 0; i < channel_count; ++i) {

		//////////////////////////////	
		/// [A] COMPUTE PARAMETERS
		//////////////////////////////	
		mod_feedback = p_mod_feedback + p_mod_feedback_mod
		/**/ * inputs[INPUT_FEEDBACK].getPolyVoltage(i) / 5.0;
		mod_feedback = mod_feedback * 0.9 + 0.1;
		mod_feedback_delay = p_mod_feedback_delay + p_mod_feedback_delay_mod
		/**/ * inputs[INPUT_FEEDBACK_DELAY].getPolyVoltage(i) / 5.0;
		if (mod_feedback_delay < 0.0)
			mod_feedback_delay = 0.0;
		if (mod_feedback_delay > 1.0)
			mod_feedback_delay = 1.0;

		//////////////////////////////	
		/// [B] COMPUTE OSCILLATORS
		//////////////////////////////	

		//// COMPUTE PITCH
		pitch_main = inputs[INPUT_PITCH].getVoltage(i)
		/**/ + params[PARAM_PITCH].getValue();
		if (algo == PKM_ALGO_DOUBLE) {
			pitch_1[0] = pitch_main - 0.1000 * p_detune;	// - (7 / 7)
			pitch_1[1] = pitch_main - 0.0714 * p_detune;	// - (5 / 7)
			pitch_1[2] = pitch_main - 0.0428 * p_detune;	// - (3 / 7)
			pitch_1[3] = pitch_main - 0.0142 * p_detune;	// - (1 / 7)
			pitch_2[0] = pitch_main + 0.0142 * p_detune;	// + (1 / 7)
			pitch_2[1] = pitch_main + 0.0428 * p_detune;	// + (3 / 7)
			pitch_2[2] = pitch_main + 0.0714 * p_detune;	// + (5 / 7)
			pitch_2[3] = pitch_main + 0.1000 * p_detune;	// + (7 / 7)
		} else {
			pitch_1[0] = pitch_main - 0.1000 * p_detune;	// - (3 / 3)
			pitch_1[1] = pitch_main - 0.0333 * p_detune;	// - (1 / 3)
			pitch_1[2] = pitch_main + 0.0333 * p_detune;	// + (1 / 3)
			pitch_1[3] = pitch_main + 0.1000 * p_detune;	// + (3 / 3)
		}

		//// COMPUTE FREQUENCY (Hz)
		if (algo == PKM_ALGO_DOUBLE)
			freq_2 = dsp::FREQ_C4 * simd::pow(2.f, pitch_2);
		freq_1 = dsp::FREQ_C4 * simd::pow(2.f, pitch_1);

		//// COMPUTE PHASE
		if (algo == PKM_ALGO_DOUBLE) {
			this->phase_2[i] += freq_2 * args.sampleTime;
			this->phase_2[i] -= simd::floor(this->phase_2[i]);
		}
		this->phase_1[i] += freq_1 * args.sampleTime;
		this->phase_1[i] -= simd::floor(this->phase_1[i]);

		//// COMPUTE PHASE MODULATION
		mod_feedback_phase = (this->feedback_i
		/**/ - (1 + (int)(PKM_FEEDBACK * mod_feedback_delay * 0.9999))
		/**/ + PKM_FEEDBACK) % PKM_FEEDBACK;
		if (algo == PKM_ALGO_DOUBLE) {
			feedback_2[0] = this->feedback_buffer_2[i][0][mod_feedback_phase];
			feedback_2[1] = this->feedback_buffer_2[i][1][mod_feedback_phase];
			feedback_2[2] = this->feedback_buffer_2[i][2][mod_feedback_phase];
			feedback_2[3] = this->feedback_buffer_2[i][3][mod_feedback_phase];
			phase_2 = this->phase_2[i] + (feedback_2 * 0.5 + 0.5) * mod_feedback;
		}
		feedback_1[0] = this->feedback_buffer_1[i][0][mod_feedback_phase];
		feedback_1[1] = this->feedback_buffer_1[i][1][mod_feedback_phase];
		feedback_1[2] = this->feedback_buffer_1[i][2][mod_feedback_phase];
		feedback_1[3] = this->feedback_buffer_1[i][3][mod_feedback_phase];
		phase_1 = this->phase_1[i] + (feedback_1 * 0.5 + 0.5) * mod_feedback;

		//// COMPUTE PHASE MODULO
		if (algo == PKM_ALGO_DOUBLE) {
			phase_2 = simd::fmod((simd::fmod(simd::floor(phase_2 * PKM_RESOLUTION),
			/**/ PKM_RESOLUTION) + PKM_RESOLUTION), PKM_RESOLUTION);
		}
		phase_1 = simd::fmod((simd::fmod(simd::floor(phase_1 * PKM_RESOLUTION),
		/**/ PKM_RESOLUTION) + PKM_RESOLUTION), PKM_RESOLUTION);

		//// COMPUTE WAVEFORM
		if (algo == PKM_ALGO_DOUBLE) {
			this->out_2[i][0] = this->wave[(int)phase_2[0]];
			this->out_2[i][1] = this->wave[(int)phase_2[1]];
			this->out_2[i][2] = this->wave[(int)phase_2[2]];
			this->out_2[i][3] = this->wave[(int)phase_2[3]];
		}
		this->out_1[i][0] = this->wave[(int)phase_1[0]];
		this->out_1[i][1] = this->wave[(int)phase_1[1]];
		this->out_1[i][2] = this->wave[(int)phase_1[2]];
		this->out_1[i][3] = this->wave[(int)phase_1[3]];

		//// COMPUTE FEEDBACK OUTPUT
		if (algo == PKM_ALGO_DOUBLE) {
			this->feedback_buffer_2[i][0][this->feedback_i] = this->out_2[i][0];
			this->feedback_buffer_2[i][1][this->feedback_i] = this->out_2[i][1];
			this->feedback_buffer_2[i][2][this->feedback_i] = this->out_2[i][2];
			this->feedback_buffer_2[i][3][this->feedback_i] = this->out_2[i][3];
		} else if (algo == PKM_ALGO_SIMPLE) {
			this->feedback_buffer_1[i][0][this->feedback_i] = this->out_1[i][0];
			this->feedback_buffer_1[i][1][this->feedback_i] = this->out_1[i][1];
			this->feedback_buffer_1[i][2][this->feedback_i] = this->out_1[i][2];
			this->feedback_buffer_1[i][3][this->feedback_i] = this->out_1[i][3];
		} else if (algo == PKM_ALGO_ECHO) {
			this->feedback_buffer_1[i][0][this->feedback_i] =
			/**/ this->feedback_buffer_1[i][0][this->feedback_i] * 0.5
			/**/ + this->out_1[i][0];
			this->feedback_buffer_1[i][1][this->feedback_i] =
			/**/ this->feedback_buffer_1[i][1][this->feedback_i] * 0.5
			/**/ + this->out_1[i][1];
			this->feedback_buffer_1[i][2][this->feedback_i] =
			/**/ this->feedback_buffer_1[i][2][this->feedback_i] * 0.5
			/**/ + this->out_1[i][2];
			this->feedback_buffer_1[i][3][this->feedback_i] =
			/**/ this->feedback_buffer_1[i][3][this->feedback_i] * 0.5
			/**/ + this->out_1[i][3];
		} else if (algo == PKM_ALGO_ENTANGLED) {
			this->feedback_buffer_1[i][0][this->feedback_i] = this->out_1[i][3];
			this->feedback_buffer_1[i][1][this->feedback_i] = this->out_1[i][2];
			this->feedback_buffer_1[i][2][this->feedback_i] = this->out_1[i][1];
			this->feedback_buffer_1[i][3][this->feedback_i] = this->out_1[i][0];
		}
	
		//// COMPUTE STEREO OUTPUT PANNING
		if (algo == PKM_ALGO_DOUBLE) {
			pan_1[0] = 0.5 - (p_width * 0.50);
			pan_1[1] = 0.5 - (p_width * 0.35);
			pan_1[2] = 0.5 - (p_width * 0.20);
			pan_1[3] = 0.5 - (p_width * 0.05);
			pan_2[0] = 0.5 + (p_width * 0.05);
			pan_2[1] = 0.5 + (p_width * 0.20);
			pan_2[2] = 0.5 + (p_width * 0.35);
			pan_2[3] = 0.5 + (p_width * 0.50);
		} else {
			pan_1[0] = 0.5 - (p_width * 0.50);
			pan_1[1] = 0.5 + (p_width * 0.20);
			pan_1[2] = 0.5 - (p_width * 0.20);
			pan_1[3] = 0.5 + (p_width * 0.50);
		}
		if (algo == PKM_ALGO_DOUBLE) {
			out_l = this->out_1[i] * pan_1 + this->out_2[i] * pan_2;
			out_r = this->out_1[i] * (1.0 - pan_1) + this->out_2[i] * (1.0 - pan_2);
		} else {
			out_l = this->out_1[i] * pan_1;
			out_r = this->out_1[i] * (1.0 - pan_1);
		}

		//// SEND STEREO OUTPUT
		if (algo == PKM_ALGO_DOUBLE) {
			out_l *= 0.125;
			out_r *= 0.125;
		} else {
			out_l *= 0.25;
			out_r *= 0.25;
		}
		outputs[OUTPUT_LEFT].setVoltage(
		/**/ (out_l[0] + out_l[1] + out_l[2] + out_l[3]) * 5.0, i);
		outputs[OUTPUT_RIGHT].setVoltage(
		/**/ (out_r[0] + out_r[1] + out_r[2] + out_r[3]) * 5.0, i);

	}

	//////////////////////////////	
	/// [4] COMPUTE OUTPUT
	//////////////////////////////	

	this->feedback_i += 1;
		if (this->feedback_i >= PKM_FEEDBACK)
			this->feedback_i = 0;
}

Model* modelPkm = createModel<Pkm, PkmWidget>("Biset-Pkm");
