
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
	int		i, j;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configInput(INPUT_PITCH_1, "V/Oct Good");
	configInput(INPUT_PITCH_2, "V/Oct Bad");
	configInput(INPUT_PITCH_3, "V/Oct Ugly");
	configInput(INPUT_LEVEL_1, "Level Good");
	configInput(INPUT_LEVEL_2, "Level Bad");
	configInput(INPUT_LEVEL_3, "Level Ugly");
	configInput(INPUT_FEEDBACK_1, "Feedback Good");
	configInput(INPUT_FEEDBACK_2, "Feedback Bad");
	configInput(INPUT_FEEDBACK_3, "Feedback Ugly");
	configInput(INPUT_PM_1_2, "PM Good → Bad");
	configInput(INPUT_PM_2_1, "PM Good ← Bad");
	configInput(INPUT_PM_3_1, "PM Good ← Ugly");
	configInput(INPUT_PM_3_2, "PM Ugly → Bad");
	configInput(INPUT_RM_1_2_1, "AM Good ↔ Bad");
	configInput(INPUT_RM_3_1, "AM Good ← Ugly");
	configInput(INPUT_RM_3_2, "AM Ugly → Bad");
	configInput(INPUT_RM_MODE, "RM / AM");
	configInput(INPUT_FEEDBACK_DELAY, "Feedback delay");

	configOutput(OUTPUT_1, "Good");
	configOutput(OUTPUT_2, "Bad");
	configOutput(OUTPUT_3, "Ugly");
	configOutput(OUTPUT_MIX, "Mix");
	configOutput(OUTPUT_EXTRA, "Extra");

	configSwitch(PARAM_ALGO_SWITCH, 0, GBU_ALGO_MAX, 0, "Algorithm",
	/**/ {"Ugly", "Weird", "Queen"});

	configParam(PARAM_PITCH_GLOBAL, -9, +9, 0, "Pitch");
	configParam(PARAM_PITCH_1, -9, +9, 0, "Pitch Good");
	configParam(PARAM_PITCH_2, -9, +9, 0, "Pitch Bad");
	configParam(PARAM_PITCH_3, -9, +9, 0, "Pitch Ugly");

	configParam(PARAM_LEVEL_1, 0, 1, 1, "Level Good");
	configParam(PARAM_LEVEL_2, 0, 1, 0, "Level Bad");
	configParam(PARAM_LEVEL_3, 0, 1, 0, "Level Ugly");

	configParam(PARAM_FEEDBACK_1, 0, 1, 0, "Feedback Good");
	configParam(PARAM_FEEDBACK_2, 0, 1, 0, "Feedback Bad");
	configParam(PARAM_FEEDBACK_3, 0, 1, 0, "Feedback Ugly");

	configParam(PARAM_RM_MODE, 0, 1, 1, "RM / AM");
	configParam(PARAM_PM_1_2, 0, 1, 0, "PM Good → Bad");
	configParam(PARAM_PM_2_1, 0, 1, 0, "PM Good ← Bad");
	configParam(PARAM_PM_3_1, 0, 1, 0, "PM Good ← Ugly");
	configParam(PARAM_PM_3_2, 0, 1, 0, "PM Ugly → Bad");
	configParam(PARAM_RM_1_2_1, -1, +1, 0, "RM Good ↔ Bad");
	configParam(PARAM_RM_3_1, 0, 1, 0, "RM Good ← Ugly");
	configParam(PARAM_RM_3_2, 0, 1, 0, "RM Ugly → Bad");

	configParam(PARAM_FEEDBACK_DELAY, 0, 1, 0, "Feedback delay");

	configParam(PARAM_NOISE_SPEED, 0, 1, 0.3, "Ugly noise speed");
	configParam(PARAM_NOISE_AMP, 0, 1, 0.5, "Ugly noise amplitude");
	configParam(PARAM_FOLLOW_ATTRACTION, 0, 1, 0.5, "Ugly attraction");
	configParam(PARAM_FOLLOW_FRICTION, 0, 1, 0.5, "Ugly friction");

	this->algo = GBU_ALGO_UGLY;	// TODO: should be saved (as a parameter ?)

	for (i = 0; i < 16; ++i) {
		this->phase_1[i] = 0.0;
		this->phase_2[i] = 0.0;
		this->phase_3[i] = 0.0;
		this->phase_4[i] = 0.0;
		this->out_1[i] = 0.0;
		this->out_2[i] = 0.0;
		this->out_3[i] = 0.0;
		this->out_4[i] = 0.0;
		this->pitch_3[i] = 0.0;
		this->pitch_3_acc[i] = 0.0;
		this->pitch_3_noise_phase[i] = (float)i * 12.345;
		this->pitch_4[i] = 0.0;
		this->pitch_4_acc[i] = 0.0;
		this->pitch_4_noise_phase[i] = (float)(i + 16) * 12.345;

		for (j = 0; j < 4096; ++j) {
			this->feedback_buffer_1[i][j] = 0.0;
			this->feedback_buffer_2[i][j] = 0.0;
			this->feedback_buffer_3[i][j] = 0.0;
			this->feedback_buffer_4[i][j] = 0.0;
		}
	}
	this->feedback_i = 0;

	for (i = 0; i < GBU_RESOLUTION; ++i)
		this->wave[i] = std::sin(2.0 * M_PI * (i / (float)GBU_RESOLUTION));
}

void Gbu::process(const ProcessArgs& args) {
	// p_x	value on knob
	// x	value on knob * input
	float	p_mod_pm_1_2;
	float	p_mod_pm_2_1;
	float	p_mod_pm_3_1;
	float	p_mod_pm_3_2;
	float	mod_pm_1_2;
	float	mod_pm_2_1;
	float	mod_pm_3_1;
	float	mod_pm_3_2;
	float	mod_rm;
	float	p_mod_rm_mode;
	float	mod_rm_mode;
	float	mod_rm_mul;
	float	mod_rm_add;
	float	p_mod_rm_1_2;
	float	p_mod_rm_2_1;
	float	p_mod_rm_3_1;
	float	p_mod_rm_3_2;
	float	mod_rm_1_2;
	float	mod_rm_2_1;
	float	mod_rm_3_1;
	float	mod_rm_3_2;
	float	p_mod_feedback_1;
	float	p_mod_feedback_2;
	float	p_mod_feedback_3;
	float	p_mod_feedback_delay;
	float	mod_feedback_1;
	float	mod_feedback_2;
	float	mod_feedback_3;
	float	mod_feedback_delay;
	int		mod_feedback_phase;
	float	noise_speed;
	float	noise_amp;
	float	follow_attraction;
	float	follow_friction;
	float	follow_attraction_4;
	float	follow_friction_4;
	float	pitch_global;
	float	pitch_main;
	float	p_pitch_1;
	float	p_pitch_2;
	float	p_pitch_3;
	float	pitch_1;
	float	pitch_2;
	float	pitch_3;
	float	pitch_4;
	float	pitch_3_aim;
	float	p_level_1;
	float	p_level_2;
	float	p_level_3;
	float	level_1;
	float	level_2;
	float	level_3;
	float	freq;
	float	phase;
	float	out;
	float	out_level;
	int		channel_count;
	int		i;

	/// [1] CHECK ALGORITHM CHANGE
	this->algo = params[PARAM_ALGO_SWITCH].getValue();
	if (args.frame % 32 == 0) {
		lights[LIGHT_UGLY].setBrightness(this->algo == GBU_ALGO_UGLY);
		lights[LIGHT_WEIRD].setBrightness(this->algo == GBU_ALGO_WEIRD);
		lights[LIGHT_QUEEN].setBrightness(this->algo == GBU_ALGO_QUEEN);
	}

	/// [2] GET PARAMETERS
	//// MODULATION RING
	mod_rm = params[PARAM_RM_1_2_1].getValue();
	if (mod_rm >= 0) {
		p_mod_rm_1_2 = mod_rm;
		p_mod_rm_2_1 = 0.0;
	} else {
		p_mod_rm_1_2 = 0.0;
		p_mod_rm_2_1 = -mod_rm;
	}
	p_mod_rm_3_1 = params[PARAM_RM_3_1].getValue();
	p_mod_rm_3_2 = params[PARAM_RM_3_2].getValue();
	p_mod_rm_mode = params[PARAM_RM_MODE].getValue();
	//// MODULATION FREQUENCY
	p_mod_pm_2_1 = params[PARAM_PM_2_1].getValue();
	p_mod_pm_3_1 = params[PARAM_PM_3_1].getValue();
	p_mod_pm_1_2 = params[PARAM_PM_1_2].getValue();
	p_mod_pm_3_2 = params[PARAM_PM_3_2].getValue();
	//// MODULATION FEEDBACK
	p_mod_feedback_delay = params[PARAM_FEEDBACK_DELAY].getValue();
	p_mod_feedback_1 = params[PARAM_FEEDBACK_1].getValue();
	p_mod_feedback_2 = params[PARAM_FEEDBACK_2].getValue();
	p_mod_feedback_3 = params[PARAM_FEEDBACK_3].getValue();
	//// LEVEL
	p_level_1 = params[PARAM_LEVEL_1].getValue();
	p_level_2 = params[PARAM_LEVEL_2].getValue();
	p_level_3 = params[PARAM_LEVEL_3].getValue();
	//// PITCH
	pitch_global = params[PARAM_PITCH_GLOBAL].getValue();
	p_pitch_1 = pitch_global + params[PARAM_PITCH_1].getValue();
	p_pitch_2 = pitch_global + params[PARAM_PITCH_2].getValue();
	p_pitch_3 = pitch_global + params[PARAM_PITCH_3].getValue();
	//// NOISE
	noise_speed = params[PARAM_NOISE_SPEED].getValue();
	noise_amp = params[PARAM_NOISE_AMP].getValue();
	//// PITCH FOLLOWING
	follow_attraction = params[PARAM_FOLLOW_ATTRACTION].getValue()
	/**/ * (0.01 - 0.0005) + 0.0005;
	follow_friction = (1.0 - params[PARAM_FOLLOW_FRICTION].getValue())
	/**/ * (0.9999 - 0.999) + 0.999;
	follow_attraction_4 = follow_attraction * 0.5;
	follow_friction_4 = 1.0 - (1.0 - follow_friction) * 0.5;

	channel_count = inputs[INPUT_PITCH_1].getChannels();
	if (channel_count == 0)
		channel_count = 1;
	outputs[OUTPUT_1].setChannels(channel_count);
	outputs[OUTPUT_2].setChannels(channel_count);
	outputs[OUTPUT_3].setChannels(channel_count);
	outputs[OUTPUT_MIX].setChannels(channel_count);
	outputs[OUTPUT_EXTRA].setChannels(channel_count);
	out_level = p_level_1 + p_level_2 + p_level_3;

	/// FOR EACH CHANNEL
	for (i = 0; i < channel_count; ++i) {

		out = 0.0;

		/// [3] COMPUTE VOICE MODULATION
		//// PITCH
		pitch_main = inputs[INPUT_PITCH_1].getVoltage(i);
		pitch_1 = p_pitch_1 + pitch_main;
		pitch_2 = p_pitch_2
		/**/ + inputs[INPUT_PITCH_2].getNormalPolyVoltage(pitch_main, i);
		pitch_3 = p_pitch_3
		/**/ + inputs[INPUT_PITCH_3].getNormalPolyVoltage(pitch_main, i);
		//// MODULATION RING
		mod_rm = inputs[INPUT_RM_1_2_1].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_rm_1_2 = p_mod_rm_1_2 * mod_rm;
		mod_rm_2_1 = p_mod_rm_2_1 * mod_rm;
		mod_rm_3_1 = p_mod_rm_3_1
		/**/ * inputs[INPUT_RM_3_1].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_rm_3_2 = p_mod_rm_3_2
		/**/ * inputs[INPUT_RM_3_2].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_rm_mode = p_mod_rm_mode
		/**/ * inputs[INPUT_RM_MODE].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_rm_mul = (1.0 - mod_rm_mode * 0.5);
		mod_rm_add = mod_rm_mode * 0.5;
		//// MODULATION PHASE
		mod_pm_1_2 = p_mod_pm_1_2
		/**/ * inputs[INPUT_PM_1_2].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_pm_2_1 = p_mod_pm_2_1
		/**/ * inputs[INPUT_PM_2_1].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_pm_3_1 = p_mod_pm_3_1
		/**/ * inputs[INPUT_PM_3_1].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_pm_3_2 = p_mod_pm_3_2
		/**/ * inputs[INPUT_PM_3_2].getNormalPolyVoltage(10.0, i) / 10.0;
		//// MODULATION FEEDBACK
		mod_feedback_1 = p_mod_feedback_1
		/**/ * inputs[INPUT_FEEDBACK_1].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_feedback_2 = p_mod_feedback_2
		/**/ * inputs[INPUT_FEEDBACK_2].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_feedback_3 = p_mod_feedback_3
		/**/ * inputs[INPUT_FEEDBACK_3].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_feedback_delay = p_mod_feedback_delay
		/**/ * inputs[INPUT_FEEDBACK_DELAY].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_feedback_phase = (this->feedback_i
		/**/ - (1 + (int)(4096 * mod_feedback_delay * 0.9999)) + 4096) % 4096;
		//// MODULATION LEVEL
		level_1 = p_level_1
		/**/ * inputs[INPUT_LEVEL_1].getNormalPolyVoltage(10.0, i) / 10.0;
		level_2 = p_level_2
		/**/ * inputs[INPUT_LEVEL_2].getNormalPolyVoltage(10.0, i) / 10.0;
		level_3 = p_level_3
		/**/ * inputs[INPUT_LEVEL_3].getNormalPolyVoltage(10.0, i) / 10.0;

		//////////////////////////////	
		/// [4] COMPUTE OSC GOOD
		//////////////////////////////	

		//// COMPUTE FREQUENCY (Hz)
		freq = dsp::FREQ_C4 * std::pow(2.f, pitch_1);
		//// COMPUTE PHASE
		this->phase_1[i] += freq * args.sampleTime;
		while (this->phase_1[i] >= 1.0)
			this->phase_1[i] -= 1.0;
		//// COMPUTE PHASE MODULATION
		phase = this->phase_1[i]
		///// MODULATION FREQUENCY (FROM BAD)
		/**/ + (this->out_2[i] * 0.5 + 0.5) * mod_pm_2_1 * 5.0
		///// MODULATION FREQUENCY (FROM UGLY)
		/**/ + (this->out_3[i] * 0.5 + 0.5) * mod_pm_3_1 * 5.0
		///// MODULATION FEEDBACK
		///**/ + (this->out_1[i] * 0.5 + 0.5) * mod_feedback_1;
		/**/ + (this->feedback_buffer_1[i][mod_feedback_phase] * 0.5 + 0.5)
		/**/ * mod_feedback_1;
		//// COMPUTE OUTPUT
		this->out_1[i] = GBU_WAVE(phase);
		//// COMPUTE AMPLITUDE MODULATION
		this->out_1[i] *=
		///// MODULATION AMPLITUDE / RING (FROM BAD)
		/**/ ((1.0 - mod_rm_2_1)
		/**/ + ((this->out_2[i] * mod_rm_mul + mod_rm_add) * mod_rm_2_1))
		///// MODULATION AMPLITUDE / RING (FROM UGLY)
		/**/ * ((1.0 - mod_rm_3_1)
		/**/ + ((this->out_3[i] * mod_rm_mul + mod_rm_add) * mod_rm_3_1));
		//// SEND OUTPUT
		outputs[OUTPUT_1].setVoltage(this->out_1[i], i);
		out += this->out_1[i] * level_1;
		this->feedback_buffer_1[i][this->feedback_i] = this->out_1[i];

		//////////////////////////////	
		/// [5] COMPUTE OSC BAD
		//////////////////////////////	

		//// COMPUTE FREQUENCY (Hz)
		freq = dsp::FREQ_C4 * std::pow(2.f, pitch_2);
		//// COMPUTE PHASE
		this->phase_2[i] += freq * args.sampleTime;
		while (this->phase_2[i] >= 1.0)
			this->phase_2[i] -= 1.0;
		//// COMPUTE PHASE MODULATION
		phase = this->phase_2[i]
		///// MODULATION FREQUENCY (FROM GOOD)
		/**/ + (this->out_1[i] * 0.5 + 0.5) * mod_pm_1_2 * 5.0
		///// MODULATION FREQUENCY (FROM UGLY)
		/**/ + (this->out_3[i] * 0.5 + 0.5) * mod_pm_3_2 * 5.0
		///// MODULATION FEEDBACK
		///**/ + (this->out_2[i] * 0.5 + 0.5) * mod_feedback_2;
		/**/ + (this->feedback_buffer_2[i][mod_feedback_phase] * 0.5 + 0.5)
		/**/ * mod_feedback_2;
		//// COMPUTE OUTPUT
		this->out_2[i] = GBU_WAVE(phase);
		//// COMPUTE AMPLITUDE MODULATION
		this->out_2[i] *=
		///// MODULATION RING (FROM BAD)
		/**/ ((1.0 - mod_rm_1_2)
		/**/ + ((this->out_1[i] * mod_rm_mul + mod_rm_add) * mod_rm_1_2))
		///// MODULATION RING (FROM UGLY)
		/**/ * ((1.0 - mod_rm_3_2)
		/**/ + ((this->out_3[i] * mod_rm_mul + mod_rm_add) * mod_rm_3_2));
		//// SEND OUTPUT
		outputs[OUTPUT_2].setVoltage(this->out_2[i], i);
		out += this->out_2[i] * level_2;
		this->feedback_buffer_2[i][this->feedback_i] = this->out_2[i];

		//////////////////////////////	
		/// [6] COMPUTE OSC UGLY
		//////////////////////////////	

		////////////////////	
		/// [A] ALGO UGLY
		////////////////////	
		if (this->algo == GBU_ALGO_UGLY) {

			//// COMPUTE PITCH MOVEMENT
			pitch_3_aim = pitch_3;
			if (this->pitch_3[i] > pitch_3)
				this->pitch_3_acc[i] -= args.sampleTime * follow_attraction;
			else
				this->pitch_3_acc[i] += args.sampleTime * follow_attraction;
			this->pitch_3_acc[i] *= follow_friction;
			this->pitch_3[i] += this->pitch_3_acc[i];
			pitch_3 = this->pitch_3[i];
			//// COMPUTE PITCH RANDOM MOVEMENT
			pitch_3 += noise_amp * 0.3 *
			/**/ ((GBU_WAVE(this->pitch_3_noise_phase[i])
			/**/ * GBU_WAVE(this->pitch_3_noise_phase[i] * 3.152)
			/**/ * GBU_WAVE(this->pitch_3_noise_phase[i] * 4.936)) * 0.5
			/**/ + GBU_WAVE(this->pitch_3_noise_phase[i] * 24.967 + 0.5) * 0.05);
			this->pitch_3_noise_phase[i] += args.sampleTime * 5.0 * noise_speed;
			//// COMPUTE FREQUENCY (Hz)
			freq = dsp::FREQ_C4 * std::pow(2.f, pitch_3);
			//// COMPUTE PHASE
			this->phase_3[i] += freq * args.sampleTime;
			while (this->phase_3[i] >= 1.0)
				this->phase_3[i] -= 1.0;
			//// COMPUTE PHASE MODULATION
			phase = this->phase_3[i]
			///// MODULATION FEEDBACK
			///**/ + (this->out_3[i] * 0.5 + 0.5) * mod_feedback_3;
			/**/ + (this->feedback_buffer_3[i][mod_feedback_phase] * 0.5 + 0.5)
			/**/ * mod_feedback_3;
			//// COMPUTE OUTPUT
			this->out_3[i] = GBU_WAVE(phase);
			//// SEND OUTPUT
			outputs[OUTPUT_EXTRA].setVoltage(pitch_3_aim - pitch_3, i);
			this->feedback_buffer_3[i][this->feedback_i] = this->out_3[i];

		////////////////////	
		/// [B] ALGO WEIRD
		////////////////////	
		} else if (this->algo == GBU_ALGO_WEIRD) {

			/// COMPUTE UGLY
			//// COMPUTE PITCH MOVEMENT
			pitch_3_aim = pitch_3;
			if (this->pitch_3[i] > pitch_3)
				this->pitch_3_acc[i] -= args.sampleTime * follow_attraction;
			else
				this->pitch_3_acc[i] += args.sampleTime * follow_attraction;
			this->pitch_3_acc[i] *= follow_friction;
			//this->pitch_3_acc[i] *= follow_friction;
			this->pitch_3[i] += this->pitch_3_acc[i];
			pitch_3 = this->pitch_3[i];
			//// COMPUTE PITCH RANDOM MOVEMENT
			pitch_3 += noise_amp * 0.3 *
			/**/ ((GBU_WAVE(this->pitch_3_noise_phase[i])
			/**/ * GBU_WAVE(this->pitch_3_noise_phase[i] * 3.152)
			/**/ * GBU_WAVE(this->pitch_3_noise_phase[i] * 4.936)) * 0.5
			/**/ + GBU_WAVE(this->pitch_3_noise_phase[i] * 24.967 + 0.5) * 0.05);
			this->pitch_3_noise_phase[i] += args.sampleTime * 5.0 * noise_speed;
			//// COMPUTE FREQUENCY (Hz)
			freq = dsp::FREQ_C4 * std::pow(2.f, pitch_3);
			//// COMPUTE PHASE
			this->phase_3[i] += freq * args.sampleTime;
			while (this->phase_3[i] >= 1.0)
				this->phase_3[i] -= 1.0;
			//// COMPUTE PHASE MODULATION
			phase = this->phase_3[i]
			///// MODULATION FREQUENCY (FROM UGLIEST)
			/**/ + (this->out_4[i] * 0.5 + 0.5) * mod_pm_3_1 * 5.0
			///// MODULATION FEEDBACK
			///**/ + (this->out_3[i] * 0.5 + 0.5) * mod_feedback_3;
			/**/ + (this->feedback_buffer_3[i][mod_feedback_phase] * 0.5 + 0.5)
			/**/ * mod_feedback_3;
			//// COMPUTE OUTPUT
			this->out_3[i] = GBU_WAVE(phase);
			//// COMPUTE AMPLITUDE MODULATION
			this->out_3[i] *=
			///// MODULATION RING (FROM UGLIEST)
			/**/ ((1.0 - mod_rm_3_1)
			/**/ + ((this->out_4[i] * mod_rm_mul + mod_rm_add) * mod_rm_3_1));
			//// SEND OUTPUT
			outputs[OUTPUT_EXTRA].setVoltage(pitch_3_aim - pitch_3, i);
			this->feedback_buffer_3[i][this->feedback_i] = this->out_3[i];

			/// COMPUTE UGLIEST
			//// COMPUTE PITCH MOVEMENT
			if (this->pitch_4[i] > pitch_3)
				this->pitch_4_acc[i] -= args.sampleTime * follow_attraction_4;
			else
				this->pitch_4_acc[i] += args.sampleTime * follow_attraction_4;
			this->pitch_4_acc[i] *= follow_friction_4;
			this->pitch_4[i] += this->pitch_4_acc[i];
			pitch_4 = this->pitch_4[i];
			//// COMPUTE PITCH RANDOM MOVEMENT
			pitch_4 += noise_amp * 0.3 *
			/**/ ((GBU_WAVE(this->pitch_4_noise_phase[i])
			/**/ * GBU_WAVE(this->pitch_4_noise_phase[i] * 3.152)
			/**/ * GBU_WAVE(this->pitch_4_noise_phase[i] * 4.936)) * 0.5
			/**/ + GBU_WAVE(this->pitch_4_noise_phase[i] * 24.967 + 0.5) * 0.05);
			this->pitch_4_noise_phase[i] += args.sampleTime * 5.0 * noise_speed;
			//// COMPUTE FREQUENCY (Hz)
			freq = dsp::FREQ_C4 * std::pow(2.f, pitch_4);
			//// COMPUTE PHASE
			this->phase_4[i] += freq * args.sampleTime;
			while (this->phase_4[i] >= 1.0)
				this->phase_4[i] -= 1.0;
			//// COMPUTE PHASE MODULATION
			phase = this->phase_4[i]
			///// MODULATION FEEDBACK
			///**/ + (this->out_4[i] * 0.5 + 0.5) * mod_feedback_3;
			/**/ + (this->feedback_buffer_4[i][mod_feedback_phase] * 0.5 + 0.5)
			/**/ * mod_feedback_3;
			//// COMPUTE OUTPUT
			this->out_4[i] = GBU_WAVE(phase);
			//// SEND OUTPUT
			this->feedback_buffer_4[i][this->feedback_i] = this->out_4[i];

		////////////////////	
		/// [C] ALGO QUEEN
		////////////////////	
		} else if (this->algo == GBU_ALGO_QUEEN) {

			pitch_3_aim = pitch_3;

			//// COMPUTE PITCH MOVEMENT
			if (this->pitch_3[i] > pitch_3_aim - 0.02)
				this->pitch_3_acc[i] -= args.sampleTime * follow_attraction;
			else
				this->pitch_3_acc[i] += args.sampleTime * follow_attraction;
			this->pitch_3_acc[i] *= follow_friction;
			this->pitch_3[i] += this->pitch_3_acc[i];
			pitch_3 = this->pitch_3[i];
			//// COMPUTE PITCH RANDOM MOVEMENT
			pitch_3 += noise_amp * 0.3 *
			/**/ ((GBU_WAVE(this->pitch_3_noise_phase[i])
			/**/ * GBU_WAVE(this->pitch_3_noise_phase[i] * 3.152)
			/**/ * GBU_WAVE(this->pitch_3_noise_phase[i] * 4.936)) * 0.5
			/**/ + GBU_WAVE(this->pitch_3_noise_phase[i] * 24.967 + 0.5) * 0.05);
			this->pitch_3_noise_phase[i] += args.sampleTime * 5.0 * noise_speed;
			//// COMPUTE FREQUENCY (Hz)
			freq = dsp::FREQ_C4 * std::pow(2.f, pitch_3);
			//// COMPUTE PHASE
			this->phase_3[i] += freq * args.sampleTime;
			while (this->phase_3[i] >= 1.0)
				this->phase_3[i] -= 1.0;
			//// COMPUTE PHASE MODULATION
			phase = this->phase_3[i]
			///// MODULATION FEEDBACK
			///**/ + (this->out_3[i] * 0.5 + 0.5) * mod_feedback_3;
			/**/ + (this->feedback_buffer_3[i][mod_feedback_phase] * 0.5 + 0.5)
			/**/ * mod_feedback_3;
			//// COMPUTE OUTPUT
			this->out_3[i] = GBU_WAVE(phase);
			//// SEND OUTPUT
			outputs[OUTPUT_EXTRA].setVoltage(pitch_3_aim - pitch_3, i);
			this->feedback_buffer_3[i][this->feedback_i] = this->out_3[i];

			//// COMPUTE PITCH MOVEMENT
			if (this->pitch_4[i] > pitch_3_aim + 0.02)
				this->pitch_4_acc[i] -= args.sampleTime * follow_attraction;
			else
				this->pitch_4_acc[i] += args.sampleTime * follow_attraction;
			this->pitch_4_acc[i] *= follow_friction;
			this->pitch_4[i] += this->pitch_4_acc[i];
			pitch_4 = this->pitch_4[i];
			//// COMPUTE PITCH RANDOM MOVEMENT
			pitch_4 += noise_amp * 0.3 *
			/**/ ((GBU_WAVE(this->pitch_4_noise_phase[i])
			/**/ * GBU_WAVE(this->pitch_4_noise_phase[i] * 3.152)
			/**/ * GBU_WAVE(this->pitch_4_noise_phase[i] * 4.936)) * 0.5
			/**/ + GBU_WAVE(this->pitch_4_noise_phase[i] * 24.967 + 0.5) * 0.05);
			this->pitch_4_noise_phase[i] += args.sampleTime * 5.0 * noise_speed;
			//// COMPUTE FREQUENCY (Hz)
			freq = dsp::FREQ_C4 * std::pow(2.f, pitch_4);
			//// COMPUTE PHASE
			this->phase_4[i] += freq * args.sampleTime;
			while (this->phase_4[i] >= 1.0)
				this->phase_4[i] -= 1.0;
			//// COMPUTE PHASE MODULATION
			phase = this->phase_4[i]
			///// MODULATION FEEDBACK
			///**/ + (this->out_4[i] * 0.5 + 0.5) * mod_feedback_3;
			/**/ + (this->feedback_buffer_4[i][mod_feedback_phase] * 0.5 + 0.5)
			/**/ * mod_feedback_3;
			//// COMPUTE OUTPUT
			this->out_3[i] += GBU_WAVE(phase);
			//// SEND OUTPUT
			this->feedback_buffer_4[i][this->feedback_i] = this->out_4[i];

		}
		outputs[OUTPUT_3].setVoltage(this->out_3[i], i);
		out += this->out_3[i] * level_3;

		/// [7] COMPUTE OUTPUT
		if (out_level > 1.0)
			out /= out_level;
		outputs[OUTPUT_MIX].setVoltage(out * 5.0, i);
	}

	/// [8] UPDATE FEEDBACK DELAY
	this->feedback_i += 1;
		if (this->feedback_i >= 4096)
			this->feedback_i = 0;
}

Model* modelGbu = createModel<Gbu, GbuWidget>("Biset-Gbu");
