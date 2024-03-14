
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

	configParam<ParamQuantityLinearRatio>(PARAM_DELAY_TIME, -9, +9, 0.0, "Delay time", "s");
	configInput(INPUT_DELAY_CLOCK, "Delay clock");
	configInput(INPUT_DELAY_TIME, "Delay time");
	configParam(PARAM_DELAY_TIME_MOD, 0.0, 1.0, 0.0, "Delay time mod", "%", 0, 100);
	configSwitch(PARAM_DELAY_PPQN, 0, 9, 0, "Delay clock ppqn", {
		"1", "4", "8", "12", "24", "32", "48", "64", "96"
	});

	configSwitch(PARAM_MODE, 0, 3, 0, "Mode", {
		"Pos relative", "Pos absolute", "Speed", "Grain"
	});

	configParam(PARAM_POS, 0.0, 1.0, 0.0, "Playhead position", "");
	configInput(INPUT_POS_1, "Playhead position 1");
	configParam(PARAM_POS_MOD_1, -1.0, 1.0, 0.0, "Playhead position 1 mod", "%", 0, 100);
	configInput(INPUT_POS_2, "Playhead position 2");
	configParam(PARAM_POS_MOD_2, -1.0, 1.0, 0.0, "Playhead position 2 mod", "%", 0, 100);

	configParam(PARAM_SPEED, -4.0, 4.0, 0.0, "Playhead speed", "");
	configInput(INPUT_SPEED_1, "Playhead speed 1");
	configParam(PARAM_SPEED_MOD_1, -1.0, 1.0, 0.0, "Playhead speed 1 mod", "%", 0, 100);
	configInput(INPUT_SPEED_2, "Playhead speed 2");
	configParam(PARAM_SPEED_MOD_2, -1.0, 1.0, 0.0, "Playhead speed 2 mod", "%", 0, 100);
	configInput(INPUT_SPEED_REV, "Playhead speed reverse");
	configSwitch(PARAM_SPEED_REV, 0, 1, 0, "Speed reverse");
	configSwitch(PARAM_SPEED_ROUND, 0, 3, 0, "Speed round", {
		"None", "Knob", "Knob + 1st input", "All"
	});

	configParam(PARAM_GRAIN, 0.0, 1.0, 0.0, "Grain length", "");
	configInput(INPUT_GRAIN_1, "Grain length 1");
	configParam(PARAM_GRAIN_MOD_1, -1.0, 1.0, 0.0, "Grain length 1 mod", "%", 0, 100);
	configInput(INPUT_GRAIN_2, "Grain length 2");
	configParam(PARAM_GRAIN_MOD_2, -1.0, 1.0, 0.0, "Grain length 2 mod", "%", 0, 100);

	configParam(PARAM_LVL, 0.0, 1.0, 1.0, "Playhead level", "");
	configInput(INPUT_LVL_1, "Playhead level 1");
	configParam(PARAM_LVL_MOD_1, -1.0, 1.0, 0.0, "Playhead level 1 mod", "%", 0, 100);
	configInput(INPUT_LVL_2, "Playhead level 2");
	configParam(PARAM_LVL_MOD_2, -1.0, 1.0, 0.0, "Playhead level 2 mod", "%", 0, 100);

	configParam(PARAM_LVL_SHAPE_FORCE, 0.0, 1.0, 0.0, "Global level shape force", "%", 0, 100);
	configParam(PARAM_LVL_SHAPE_WAVE, 0.0, 1.0, 0.5, "Global level shape wave", "%", 0, 100);

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
	float		knob_delay;
	float		knob_pos;
	float		knob_lvl;
	float		knob_speed;
	float		knob_speed_rev;
	float		knob_shape_force, knob_shape_wave;
	float		mode_round;
	float		mod_pos_1, mod_pos_2;
	float		mod_lvl_1, mod_lvl_2;
	float		mod_speed_1, mod_speed_2;
	int			buffer_index_a, buffer_index_b;
	int			buffer_length;
	int			channels;
	int			mode;
	int			ppqn;
	int			i;
	int			param_ppqn;
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
	knob_delay = this->params[PARAM_DELAY_TIME].getValue();
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

	knob_shape_force = this->params[PARAM_LVL_SHAPE_FORCE].getValue()
	/**/ + this->params[PARAM_LVL_SHAPE_FORCE_MOD].getValue()
	/**/ * this->inputs[INPUT_LVL_SHAPE_FORCE].getVoltage() * 0.1;
	knob_shape_wave = this->params[PARAM_LVL_SHAPE_WAVE].getValue()
	/**/ + this->params[PARAM_LVL_SHAPE_WAVE_MOD].getValue()
	/**/ * this->inputs[INPUT_LVL_SHAPE_WAVE].getVoltage() * 0.1;

	//////////////////////////////	
	/// [3] COMPUTE DELAY TIME
	//////////////////////////////	
	/// CLOCK MODE
	if (this->inputs[INPUT_DELAY_CLOCK].isConnected()) {
		if (param_ppqn == 0)
			ppqn = 1;
		else if (param_ppqn == 1)
			ppqn = 4;
		else if (param_ppqn == 2)
			ppqn = 8;
		else if (param_ppqn == 3)
			ppqn = 12;
		else if (param_ppqn == 4)
			ppqn = 24;
		else if (param_ppqn == 5)
			ppqn = 32;
		else if (param_ppqn == 6)
			ppqn = 48;
		else if (param_ppqn == 7)
			ppqn = 64;
		else
			ppqn = 96;
		this->clock_samples_count += 1;
		/// ON TRIGGER
		if (this->trigger_clock.process(inputs[INPUT_DELAY_CLOCK].getVoltage())) {
			/// COMPUTE DELAY TIME
			this->delay_time_aim = ((float)this->clock_samples_count
			/**/ / args.sampleRate) * (float)ppqn;
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

		/// MODE SPEED
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
			speed = std::pow(2.f, speed);
			if (knob_speed_rev > 0)
				speed = -speed;
			if (this->inputs[INPUT_SPEED_REV].getPolyVoltage(i) > 0.0)
				speed = -speed;

			/// EASE SPEED
			// TODO: use parameter (context menu ?)
			speed = speed * 0.0005 + playhead->speed * 0.9995;
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
			///// JUMP FOREWARD
			dist_1 = index - playhead->index; if (dist_1 < 0)
				dist_1 = -dist_1;
			///// JUMP BACKWARD
			dist_2 = index + ((float)IGC_BUFFER - playhead->index);
			if (dist_2 < 0)
				dist_2 = -dist_2;
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
			//// SAVE PREVIOUS VALUE
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
	this->outputs[OUTPUT_L].setVoltage(out_l);
	this->outputs[OUTPUT_R].setVoltage(out_r);

	//////////////////////////////	
	/// [5] LOOP AUDIO
	//////////////////////////////	
	this->audio_index += 1;
	if (this->audio_index >= IGC_BUFFER)
		this->audio_index = 0;
}

Model* modelIgc = createModel<Igc, IgcWidget>("Biset-Igc");
