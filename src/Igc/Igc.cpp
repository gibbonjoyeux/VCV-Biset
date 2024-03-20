
#include "Igc.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Igc::Igc() {
	int		i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configSwitch(PARAM_MODE_HD, 0, 1, 1, "Mode HD");
	configSwitch(PARAM_MODE_ANTICLICK, 0, 1, 1, "Mode Anti-click");

	configSwitch(PARAM_MODE, 0, 3, 0, "Mode", {
		"Pos relative", "Pos absolute", "Speed", "Grain"
	});

	configParam<ParamQuantityLinearRatio>(PARAM_DELAY_TIME, -9, +9, 0.0, "Delay time", "s");
	configInput(INPUT_DELAY_CLOCK, "Delay clock");
	configInput(INPUT_DELAY_TIME, "Delay time");
	configParam(PARAM_DELAY_TIME_MOD, -1.0, 1.0, 0.0, "Delay time mod", "%", 0, 100);
	configParam(PARAM_DELAY_PPQN, 1.0, 96.0, 1.0, "Delay click ppqn");

	configParam(PARAM_MIX_LVL_IN, 0.0, 2.0, 1.0, "In level", "%", 0, 100);
	configInput(INPUT_MIX_LVL_IN, "In level");
	configParam(PARAM_MIX_LVL_IN_MOD, -1.0, 1.0, 0.0, "In level mod", "%", 0, 100);
	configParam(PARAM_MIX_LVL_OUT, 0.0, 2.0, 1.0, "Out level", "%", 0, 100);
	configInput(INPUT_MIX_LVL_OUT, "Out level");
	configParam(PARAM_MIX_LVL_OUT_MOD, -1.0, 1.0, 0.0, "Out level mod", "%", 0, 100);
	configParam(PARAM_MIX, 0.0, 1.0, 1.0, "Mix", "%", 0, 100);
	configInput(INPUT_MIX, "Mix");
	configParam(PARAM_MIX_MOD, -1.0, 1.0, 0.0, "Mix mod", "%", 0, 100);

	configParam(PARAM_POS, 0.0, 1.0, 0.0, "Playhead position", "");
	configInput(INPUT_POS_1, "Playhead position 1");
	configParam(PARAM_POS_MOD_1, -1.0, 1.0, 0.0, "Playhead position 1 mod", "%", 0, 100);
	configInput(INPUT_POS_2, "Playhead position 2");
	configParam(PARAM_POS_MOD_2, -1.0, 1.0, 0.0, "Playhead position 2 mod", "%", 0, 100);

	configParam(PARAM_SPEED, -4.0, 4.0, 0.0, "Playhead speed", " v/oct");
	configInput(INPUT_SPEED_1, "Playhead speed 1");
	configParam(PARAM_SPEED_MOD_1, -1.0, 1.0, 0.0, "Playhead speed 1 mod", "%", 0, 100);
	configInput(INPUT_SPEED_2, "Playhead speed 2");
	configParam(PARAM_SPEED_MOD_2, -1.0, 1.0, 0.0, "Playhead speed 2 mod", "%", 0, 100);
	configInput(INPUT_SPEED_REV, "Playhead speed reverse");
	configSwitch(PARAM_SPEED_REV, 0, 1, 0, "Speed reverse");
	configSwitch(PARAM_SPEED_ROUND, 0, 3, 0, "Speed round", {
		"None", "Knob", "Knob + 1st input", "All"
	});
	configParam(PARAM_SPEED_SLEW, 0, 1, 0.5, "Speed slew", "%", 0, 100);

	configParam(PARAM_GRAIN, 0.0, 1.0, 0.5, "Grain length", "%", 0, 100);
	configInput(INPUT_GRAIN_1, "Grain length 1");
	configParam(PARAM_GRAIN_MOD_1, -1.0, 1.0, 0.0, "Grain length 1 mod", "%", 0, 100);
	configInput(INPUT_GRAIN_2, "Grain length 2");
	configParam(PARAM_GRAIN_MOD_2, -1.0, 1.0, 0.0, "Grain length 2 mod", "%", 0, 100);

	configParam(PARAM_LVL, 0.0, 1.0, 1.0, "Playhead level", "%", 0, 100);
	configInput(INPUT_LVL_1, "Playhead level 1");
	configParam(PARAM_LVL_MOD_1, -1.0, 1.0, 0.0, "Playhead level 1 mod", "%", 0, 100);
	configInput(INPUT_LVL_2, "Playhead level 2");
	configParam(PARAM_LVL_MOD_2, -1.0, 1.0, 0.0, "Playhead level 2 mod", "%", 0, 100);

	configParam(PARAM_LVL_SHAPE_FORCE, 0.0, 1.0, 0.0, "Global level shape force", "%", 0, 100);
	configParam(PARAM_LVL_SHAPE_FORCE_MOD, -1.0, 1.0, 0.0, "Global level shape force mod", "%", 0, 100);
	configParam(PARAM_LVL_SHAPE_WAVE, 0.0, 1.0, 0.5, "Global level shape wave", "%", 0, 100);
	configParam(PARAM_LVL_SHAPE_WAVE_MOD, -1.0, 1.0, 0.0, "Global level shape wave mod", "%", 0, 100);

	configInput(INPUT_L, "Left / Mono");
	configInput(INPUT_R, "Right");
	configOutput(OUTPUT_L, "Left");
	configOutput(OUTPUT_R, "Right");

	configBypass(INPUT_L, OUTPUT_L);
	configBypass(INPUT_R, OUTPUT_R);

	this->audio_index = 0;
	this->playhead_count = 0;
	this->clock_samples_count = 0;
	this->delay_time = 1.0;
	this->delay_time_aim = 1.0;
	this->abs_phase = 0.0;
	for (i = 0; i < 16; ++i) {
		this->playheads[i].speed = 1.0;
		this->playheads[i].index_rel = 0.0;
		this->playheads[i].grain_phase = 0.0;
		this->playheads[i].grain_time = 0.0;
	}
}

void Igc::process(const ProcessArgs& args) {
	IgcPlayhead	*playhead;
	float		in_l, in_r;
	float		out_l, out_r;
	float		voice_l, voice_r;
	float		t;
	float		dist, dist_1, dist_2;
	float		phase;
	float		index;
	float		index_inter;
	float		index_rel;
	float		level;
	float		speed;
	float		shape;
	float		length;
	float		knob_delay;
	float		knob_pos;
	float		knob_lvl;
	float		knob_speed;
	float		knob_speed_rev;
	float		knob_speed_slew;
	float		knob_shape_force, knob_shape_wave;
	float		knob_grain;
	float		knob_mix_in, knob_mix_out, knob_mix;
	float		mode_round;
	float		mod_pos_1, mod_pos_2;
	float		mod_lvl_1, mod_lvl_2;
	float		mod_speed_1, mod_speed_2;
	float		mod_grain_1, mod_grain_2;
	int			buffer_index_a, buffer_index_b;
	int			buffer_length;
	int			channels;
	int			mode;
	int			i;
	float		param_ppqn;
	bool		param_hd;
	bool		param_anticlick;

	//////////////////////////////	
	/// [1] GET ALGORITHM
	//////////////////////////////	
	mode = params[PARAM_MODE].getValue();
	mode_round = this->params[PARAM_SPEED_ROUND].getValue();
	if (args.frame % 32 == 0) {
		/// LIGHTS MODE
		lights[LIGHT_MODE_POS_REL].setBrightness(mode == IGC_MODE_POS_REL);
		lights[LIGHT_MODE_POS_ABS].setBrightness(mode == IGC_MODE_POS_ABS);
		lights[LIGHT_MODE_SPEED].setBrightness(mode == IGC_MODE_SPEED);
		lights[LIGHT_MODE_GRAIN].setBrightness(mode == IGC_MODE_GRAIN);
		/// LIGHTS ROUND
		lights[LIGHT_ROUND_KNOB].setBrightness(mode_round > IGC_ROUND_NONE);
		lights[LIGHT_ROUND_INPUT_1].setBrightness(mode_round > IGC_ROUND_KNOB);
		lights[LIGHT_ROUND_INPUT_2].setBrightness(mode_round == IGC_ROUND_ALL);
	}

	//////////////////////////////	
	/// [2] GET PARAMETERS
	//////////////////////////////	
	param_hd = this->params[PARAM_MODE_HD].getValue();
	param_anticlick = this->params[PARAM_MODE_ANTICLICK].getValue();
	param_ppqn = this->params[PARAM_DELAY_PPQN].getValue();
	knob_pos = this->params[PARAM_POS].getValue();
	mod_pos_1 = this->params[PARAM_POS_MOD_1].getValue();
	mod_pos_2 = this->params[PARAM_POS_MOD_2].getValue();
	knob_lvl = this->params[PARAM_LVL].getValue();
	mod_lvl_1 = this->params[PARAM_LVL_MOD_1].getValue();
	mod_lvl_2 = this->params[PARAM_LVL_MOD_2].getValue();
	knob_speed = this->params[PARAM_SPEED].getValue();
	mod_speed_1 = this->params[PARAM_SPEED_MOD_1].getValue();
	mod_speed_2 = this->params[PARAM_SPEED_MOD_2].getValue();
	knob_speed_rev = this->params[PARAM_SPEED_REV].getValue();
	knob_speed_slew = this->params[PARAM_SPEED_SLEW].getValue();
	knob_grain = this->params[PARAM_GRAIN].getValue();
	mod_grain_1 = this->params[PARAM_GRAIN_MOD_1].getValue();
	mod_grain_2 = this->params[PARAM_GRAIN_MOD_2].getValue();

	knob_delay = this->params[PARAM_DELAY_TIME].getValue()
	/**/ + this->params[PARAM_DELAY_TIME_MOD].getValue()
	/**/ * this->inputs[INPUT_DELAY_TIME].getVoltage() * 1.8;	// -5:+5 -9:+9
	if (knob_delay < -9.0)
		knob_delay = -9.0;
	if (knob_delay > 9.0)
		knob_delay = 9.0;
	knob_shape_force = this->params[PARAM_LVL_SHAPE_FORCE].getValue()
	/**/ + this->params[PARAM_LVL_SHAPE_FORCE_MOD].getValue()
	/**/ * this->inputs[INPUT_LVL_SHAPE_FORCE].getVoltage() * 0.1;
	knob_shape_wave = this->params[PARAM_LVL_SHAPE_WAVE].getValue()
	/**/ + this->params[PARAM_LVL_SHAPE_WAVE_MOD].getValue()
	/**/ * this->inputs[INPUT_LVL_SHAPE_WAVE].getVoltage() * 0.1;
	knob_speed_slew = (1.0 - knob_speed_slew);
	knob_speed_slew = knob_speed_slew * knob_speed_slew * knob_speed_slew;
	knob_speed_slew = 0.995 + (1.0 - knob_speed_slew) * 0.0049;
	knob_mix_in = this->params[PARAM_MIX_LVL_IN].getValue()
	/**/ + this->params[PARAM_MIX_LVL_IN_MOD].getValue()
	/**/ * this->inputs[INPUT_MIX_LVL_IN].getVoltage() * 0.1;
	if (knob_mix_in < 0.0)
		knob_mix_in = 0.0;
	if (knob_mix_in > 2.0)
		knob_mix_in = 2.0;
	knob_mix_out = this->params[PARAM_MIX_LVL_OUT].getValue()
	/**/ + this->params[PARAM_MIX_LVL_OUT_MOD].getValue()
	/**/ * this->inputs[INPUT_MIX_LVL_OUT].getVoltage() * 0.1;
	if (knob_mix_out < 0.0)
		knob_mix_out = 0.0;
	if (knob_mix_out > 2.0)
		knob_mix_out = 2.0;
	knob_mix = this->params[PARAM_MIX].getValue()
	/**/ + this->params[PARAM_MIX_MOD].getValue()
	/**/ * this->inputs[INPUT_MIX].getVoltage() * 0.1;
	if (knob_mix < 0.0)
		knob_mix = 0.0;
	if (knob_mix > 1.0)
		knob_mix = 1.0;

	//////////////////////////////	
	/// [3] COMPUTE DELAY TIME
	//////////////////////////////	
	/// CLOCK MODE
	if (this->inputs[INPUT_DELAY_CLOCK].isConnected()) {
		this->clock_samples_count += 1;
		/// ON TRIGGER
		if (this->trigger_clock.process(inputs[INPUT_DELAY_CLOCK].getVoltage())) {
			/// COMPUTE DELAY TIME
			this->delay_time_aim = ((float)this->clock_samples_count
			/**/ / args.sampleRate) * param_ppqn;
			this->clock_samples_count = 0;
			// TODO: Count ppqn for pos abs mode (reset phase)
			/// COMPUTE DELAY TIME MULTIPLICATION
			knob_delay = (int)knob_delay;
			if (knob_delay >= 0.0)
				knob_delay = 1.0 + knob_delay;
			else
				knob_delay = 1.0 / (1.0 + -knob_delay);
			this->delay_time_aim *= knob_delay;
			if (this->delay_time_aim > 10.0)
				this->delay_time_aim = 10.0;
		}
		/// GO TOWARD AIM DELAY TIME
		this->delay_time = this->delay_time * 0.9999
		/**/ + this->delay_time_aim * 0.0001;
	/// TIME MODE
	} else {
		if (knob_delay >= 0.0)
			knob_delay = 1.0 + knob_delay;
		else
			knob_delay = 1.0 / (1.0 + -knob_delay);
		this->delay_time = knob_delay;
	}


	//////////////////////////////	
	/// [3] RECORD AUDIO
	//////////////////////////////	
	buffer_length = (float)IGC_BUFFER * (this->delay_time * 0.1);
	in_l = this->inputs[INPUT_L].getVoltageSum();
	if (this->inputs[INPUT_R].isConnected())
		in_r = this->inputs[INPUT_R].getVoltageSum();
	else
		in_r = in_l;
	in_l *= knob_mix_in;
	in_r *= knob_mix_in;
	this->audio[0][this->audio_index] = in_l;
	this->audio[1][this->audio_index] = in_r;

	//////////////////////////////	
	/// [4] READ AUDIO
	//////////////////////////////	
	if (mode == IGC_MODE_POS_ABS) {
		this->abs_phase += 1.0 / (float)buffer_length;
		this->abs_phase -= (int)this->abs_phase;
	}
	out_l = 0.0;
	out_r = 0.0;
	channels = std::max(this->inputs[INPUT_POS_1].getChannels(),
	/**/ this->inputs[INPUT_POS_2].getChannels());
	this->playhead_count = channels;
	for (i = 0; i < channels; ++i) {

		playhead = &(this->playheads[i]);

		/// MODE POS REL
		if (mode == IGC_MODE_POS_REL) {

			/// COMPUTE PHASE
			phase = knob_pos
			/**/ + this->inputs[INPUT_POS_1].getPolyVoltage(i) * 0.1 * mod_pos_1
			/**/ + this->inputs[INPUT_POS_2].getPolyVoltage(i) * 0.1 * mod_pos_2;
			phase = fmod(fmod(phase, 1.0) + 1.0, 1.0);
			playhead->phase = phase;

			/// COMPUTE REAL INDEX
			index = (float)this->audio_index
			/**/ - (phase * (float)buffer_length);
			if (index < 0)
				index += IGC_BUFFER;
			//index = fmod(fmod(index, IGC_BUFFER) + IGC_BUFFER, IGC_BUFFER);

		/// MODE POS ABSOLUTE
		} else if (mode == IGC_MODE_POS_ABS) {

			/// COMPUTE PHASE
			phase = this->abs_phase - (knob_pos
			/**/ + this->inputs[INPUT_POS_1].getPolyVoltage(i) * 0.1 * mod_pos_1
			/**/ + this->inputs[INPUT_POS_2].getPolyVoltage(i) * 0.1 * mod_pos_2);
			phase = fmod(fmod(phase, 1.0) + 1.0, 1.0);
			playhead->phase = phase;

			/// COMPUTE REAL INDEX
			index = (float)this->audio_index
			/**/ - (phase * (float)buffer_length);
			if (index < 0)
				index += IGC_BUFFER;

		/// MODE SPEED / GRAIN
		} else {

			/// COMPUTE SPEED
			if (mode_round == IGC_ROUND_NONE) {
				speed = knob_speed
				/**/ + this->inputs[INPUT_SPEED_1].getPolyVoltage(i) * mod_speed_1
				/**/ + this->inputs[INPUT_SPEED_2].getPolyVoltage(i) * mod_speed_2;
			} else if (mode_round == IGC_ROUND_KNOB) {
				speed = std::round(knob_speed)
				/**/ + this->inputs[INPUT_SPEED_1].getPolyVoltage(i) * mod_speed_1
				/**/ + this->inputs[INPUT_SPEED_2].getPolyVoltage(i) * mod_speed_2;
			} else if (mode_round == IGC_ROUND_KNOB_INPUT) {
				speed = std::round(knob_speed
				/**/ + this->inputs[INPUT_SPEED_1].getPolyVoltage(i) * mod_speed_1)
				/**/ + this->inputs[INPUT_SPEED_2].getPolyVoltage(i) * mod_speed_2;
			} else {
				speed = std::round(knob_speed
				/**/ + this->inputs[INPUT_SPEED_1].getPolyVoltage(i) * mod_speed_1
				/**/ + this->inputs[INPUT_SPEED_2].getPolyVoltage(i) * mod_speed_2);
			}
			speed = std::pow(2.f, speed);	// BOTTLENECK -> LOOKUP TABLE ? SIMD ?
			if (knob_speed_rev > 0)
				speed = -speed;
			if (this->inputs[INPUT_SPEED_REV].getPolyVoltage(i) > 0.0)
				speed = -speed;

			/// MODE SPEED
			if (mode == IGC_MODE_SPEED) {

				/// EASE SPEED
				speed = speed * (1.0 - knob_speed_slew)
				/**/ + playhead->speed * knob_speed_slew;
				playhead->speed = speed;

				/// COMPUTE PHASE / RELATIVE INDEX
				index_rel = playhead->index_rel;
				index_rel += 1.0 - speed;
				index_rel = fmod(fmod(index_rel, buffer_length)
				/**/ + buffer_length, buffer_length);
				playhead->index_rel = index_rel;

				/// COMPUTE REAL INDEX
				index = (float)this->audio_index - index_rel;
				while (index < 0)
					index += IGC_BUFFER;

				/// COMPUTE RELATIVE PHASE
				if (this->audio_index > index)
					phase = (float)this->audio_index - index;
				else
					phase = (float)this->audio_index + ((float)IGC_BUFFER - index);
				phase = fmod(fmod(phase / (float)buffer_length, 1.0) + 1.0, 1.0);
				playhead->phase = phase;

			/// MODE GRAIN
			} else {

				/// RESTART GRAIN
				if (playhead->grain_time >= playhead->grain_length) {
					/// RESET GRAIN PHASE
					phase = knob_pos
					/**/ + this->inputs[INPUT_POS_1].getPolyVoltage(i)
					/**/ * 0.1 * mod_pos_1
					/**/ + this->inputs[INPUT_POS_2].getPolyVoltage(i)
					/**/ * 0.1 * mod_pos_2;

					/// GET GRAIN LENGTH
					length = knob_grain
					/**/ + this->inputs[INPUT_GRAIN_1].getPolyVoltage(i)
					/**/ * mod_grain_1 * 0.1
					/**/ + this->inputs[INPUT_GRAIN_2].getPolyVoltage(i)
					/**/ * mod_grain_2 * 0.1;
					if (length < 0.0)
						length = 0.0;
					if (length > 1.0)
						length = 1.0;
					/// GET GRAIN REMAPPED LENGTH [0.1:2.0]
					length = 0.1 + length * 1.9;
					/// INIT GRAIN
					playhead->grain_length = length;
					playhead->grain_time = 0.0;
					playhead->speed = speed;
				} else {
					phase = playhead->phase;
					playhead->grain_time += args.sampleTime;
				}

				/// COMPUTE PHASE
				phase += 1.0 / (float)buffer_length;
				phase -= (1.0 / (float)buffer_length) * playhead->speed;
				phase = fmod(fmod(phase, 1.0) + 1.0, 1.0);
				playhead->phase = phase;

				/// COMPUTE REAL INDEX
				index = (float)this->audio_index
				/**/ - (phase * (float)buffer_length);
				if (index < 0)
					index += IGC_BUFFER;

			}

		}

		/// COMPUTE LEVEL
		level = knob_lvl
		/**/ + this->inputs[INPUT_LVL_1].getPolyVoltage(i) * 0.1
		/**/ * mod_lvl_1
		/**/ + this->inputs[INPUT_LVL_2].getPolyVoltage(i) * 0.1
		/**/ * mod_lvl_2;
		if (level > 1.0)
			level = 1.0;
		if (level < 0.0)
			level = 0.0;
		if (knob_shape_force > 0.0) {
			if (knob_shape_force > 1.0)
				knob_shape_force = 1.0;
			if (knob_shape_wave < 0.01)
				knob_shape_wave = 0.01;
			if (knob_shape_wave > 0.99)
				knob_shape_wave = 0.99;
			/// WAVE UP
			if (phase < knob_shape_wave) {
				level *= 1.0 - (knob_shape_force
				/**/ * (1.0 - (phase / knob_shape_wave)));
			/// WAVE DOWN
			} else {
				level *= 1.0 - knob_shape_force * ((phase - knob_shape_wave)
				/**/ / (1.0 - knob_shape_wave));
			}
		}
		level = playhead->level * 0.99 + level * 0.01;
		playhead->level = level;

		if (mode == IGC_MODE_GRAIN) {
			if (playhead->grain_time < (playhead->grain_length / 2.0)) {
				shape = (playhead->grain_time / (playhead->grain_length / 2.0));
			} else {
				shape = 1.0
				/**/ - ((playhead->grain_time - (playhead->grain_length / 2.0))
				/**/ / (playhead->grain_length / 2.0));
			}
			shape = (1.0 - shape);
			shape = shape * shape * shape;
			shape = (1.0 - shape);
			level *= shape;
			playhead->level = level;
		}

		/// COMPUTE PLAYHEAD OUTPUT
		if (param_hd) {
			/// COMPUTE INTERPOLATED INDEXES
			buffer_index_a = (int)index;
			buffer_index_b = buffer_index_a + 1;
			if (buffer_index_b >= IGC_BUFFER)
				buffer_index_b = 0;
			index_inter = index - (float)buffer_index_a;
			/// COMPUTE INTERPOLATED VALUE
			voice_l = (this->audio[0][buffer_index_a] * (1.0 - index_inter)
			/**/ + this->audio[0][buffer_index_b] * index_inter);
			voice_r = (this->audio[1][buffer_index_a] * (1.0 - index_inter)
			/**/ + this->audio[1][buffer_index_b] * index_inter);
		} else {
			buffer_index_a = (int)index;
			voice_l = this->audio[0][buffer_index_a];
			voice_r = this->audio[1][buffer_index_a];
		}

		/// COMPUTE ANTI-CLICK FILTER
		if (param_anticlick) {

			/// ALGO 1
			/// -> When close to delay buffer ending point, progressively ease
			///    into delay buffer starting point.
			///
			if (this->audio_index > index) {
				dist = (float)buffer_length
				/**/ - ((float)this->audio_index - index);
			} else {
				dist = (float)buffer_length
				/**/ - ((float)this->audio_index + ((float)IGC_BUFFER - index));
			}
			if (dist < IGC_CLICK_SAFE_LENGTH) {
				t = dist / (float)IGC_CLICK_SAFE_LENGTH;
				voice_l = voice_l * t + in_l * (1.0 - t);
				voice_r = voice_r * t + in_r * (1.0 - t);
			}

			/// ALGO 2
			/// -> "Paralize" signal on big jumps in the buffer just before
			///    click and slowly come back to signal.
			///
			//// COMPUTE JUMP DISTANCE
			///// JUMP DIRECT
			dist_1 = index - playhead->index;
			if (dist_1 < 0)
				dist_1 = -dist_1;
			///// JUMP CIRCULAR
			if (index < playhead->index)
				dist_2 = index + ((float)IGC_BUFFER - playhead->index);
			else
				dist_2 = playhead->index + ((float)IGC_BUFFER - index);
			///// JUMP SMALLEST
			dist = (dist_1 < dist_2) ? dist_1 : dist_2;

			//// FIRE ANTI-CLICK
			if (dist > IGC_CLICK_DIST_THRESHOLD)
				playhead->click_remaining = IGC_CLICK_SAFE_LENGTH;
			//// COMPUTE ANTI-CLICK
			if (playhead->click_remaining > 0) {
				t = playhead->click_remaining / (float)IGC_CLICK_SAFE_LENGTH;
				voice_l = voice_l * (1.0 - t) + playhead->click_prev_l * t;
				voice_r = voice_r * (1.0 - t) + playhead->click_prev_r * t;
				playhead->click_remaining -= 1.0;
			//// PREPARE ANTI-CLICK (SAVE PREVIOUS VALUE)
			} else {
				playhead->click_prev_l = voice_l;
				playhead->click_prev_r = voice_r;
			}
		}

		playhead->index = index;

		/// ADD VOICE TO AUDIO
		out_l += voice_l * level;
		out_r += voice_r * level;
	}
	this->outputs[OUTPUT_L].setVoltage(
	/**/ in_l * (1.0 - knob_mix) + (out_l * knob_mix_out) * knob_mix);
	this->outputs[OUTPUT_R].setVoltage(
	/**/ in_r * (1.0 - knob_mix) + (out_r * knob_mix_out) * knob_mix);

	//////////////////////////////	
	/// [5] LOOP AUDIO
	//////////////////////////////	
	this->audio_index += 1;
	if (this->audio_index >= IGC_BUFFER)
		this->audio_index = 0;
}

Model* modelIgc = createModel<Igc, IgcWidget>("Biset-Igc");
