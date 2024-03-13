
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
	configParam(PARAM_DELAY_TIME_MOD, 0.0, 1.0, 1.0, "Delay time mod");
	configSwitch(PARAM_DELAY_PPQN, 0, 9, 0, "Delay clock ppqn", {
		"1", "4", "8", "12", "24", "32", "48", "64", "96"
	});

	configSwitch(PARAM_MODE, 0, 3, 0, "Mode", {
		"Pos relative", "Pos absolute", "Speed", "Grain"
	});

	configParam(PARAM_POS, 0.0, 1.0, 0.0, "Playhead position", "");
	configInput(INPUT_POS_1, "Playhead position 1");
	configParam(PARAM_POS_MOD_1, -1.0, 1.0, 1.0, "Playhead position 1 mod", "");
	configInput(INPUT_POS_2, "Playhead position 2");
	configParam(PARAM_POS_MOD_2, -1.0, 1.0, 1.0, "Playhead position 2 mod", "");

	configParam(PARAM_SPEED, -4.0, 4.0, 0.0, "Playhead speed", "");
	configInput(INPUT_SPEED_1, "Playhead speed 1");
	configParam(PARAM_SPEED_MOD_1, -1.0, 1.0, 1.0, "Playhead speed 1 mod", "");
	configInput(INPUT_SPEED_2, "Playhead speed 2");
	configParam(PARAM_SPEED_MOD_2, -1.0, 1.0, 1.0, "Playhead speed 2 mod", "");
	configInput(INPUT_SPEED_REV, "Playhead speed reverse");
	configSwitch(PARAM_SPEED_REV, 0, 1, 0, "Speed reverse");
	configSwitch(PARAM_SPEED_ROUND, 0, 1, 0, "Speed round");

	configParam(PARAM_GRAIN, 0.0, 1.0, 0.0, "Grain length", "");
	configInput(INPUT_GRAIN_1, "Grain length 1");
	configParam(PARAM_GRAIN_MOD_1, -1.0, 1.0, 1.0, "Grain length 1 mod", "");
	configInput(INPUT_GRAIN_2, "Grain length 2");
	configParam(PARAM_GRAIN_MOD_2, -1.0, 1.0, 1.0, "Grain length 2 mod", "");

	configParam(PARAM_LVL, 0.0, 1.0, 1.0, "Playhead level", "");
	configInput(INPUT_LVL_1, "Playhead level 1");
	configParam(PARAM_LVL_MOD_1, -1.0, 1.0, 1.0, "Playhead level 1 mod", "");
	configInput(INPUT_LVL_2, "Playhead level 2");
	configParam(PARAM_LVL_MOD_2, -1.0, 1.0, 1.0, "Playhead level 2 mod", "");

	configInput(INPUT_L, "Left / Mono");
	configInput(INPUT_R, "Right");
	configOutput(OUTPUT_L, "Left");
	configOutput(OUTPUT_R, "Right");

	configBypass(INPUT_L, OUTPUT_L);
	configBypass(INPUT_R, OUTPUT_R);

	this->audio_index = 0;
	this->playhead_count = 0;
	this->clock_between = 0;
	this->clock_between_count = 0;
	this->delay_time = 1.0;
	this->delay_time_aim = 1.0;
	for (i = 0; i < 16; ++i) {
		this->playheads[i].speed = 1.0;
	}
}

void Igc::process(const ProcessArgs& args) {
	float	in_l, in_r;
	float	out_l, out_r;
	float	voice_l, voice_r;
	float	t;
	float	dist;
	float	phase;
	float	index;
	float	index_inter;
	float	level;
	float	speed;
	float	knob_delay;
	float	knob_pos;
	float	knob_lvl;
	float	knob_speed;
	float	knob_speed_rev;
	float	knob_speed_round;
	float	mod_pos_1, mod_pos_2;
	float	mod_lvl_1, mod_lvl_2;
	float	mod_speed_1, mod_speed_2;
	int		buffer_index_a, buffer_index_b;
	int		buffer_length;
	int		channels;
	int		mode;
	int		ppqn;
	int		i;
	int		param_ppqn;
	bool	param_hd;
	bool	param_anticlick;

	//////////////////////////////	
	/// [1] GET ALGORITHM
	//////////////////////////////	
	mode = params[PARAM_MODE].getValue();
	if (args.frame % 32 == 0) {
		lights[LIGHT_MODE_POS_REL].setBrightness(mode == IGC_MODE_POS_REL);
		lights[LIGHT_MODE_POS_ABS].setBrightness(mode == IGC_MODE_POS_ABS);
		lights[LIGHT_MODE_SPEED].setBrightness(mode == IGC_MODE_SPEED);
		lights[LIGHT_MODE_GRAIN].setBrightness(mode == IGC_MODE_GRAIN);
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
	knob_speed_round = this->params[PARAM_SPEED_ROUND].getValue();

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
		this->clock_between_count += 1;
		/// ON TRIGGER
		if (this->trigger_clock.process(inputs[INPUT_DELAY_CLOCK].getVoltage())) {
			/// COMPUTE CLOCK RATE
			this->clock_between = this->clock_between_count;
			this->clock_between_count = 0;
			/// COMPUTE DELAY TIME
			this->delay_time_aim = ((float)this->clock_between / args.sampleRate)
			/**/ * (float)ppqn;
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
	out_l = 0.0;
	out_r = 0.0;
	channels = std::max(this->inputs[INPUT_POS_1].getChannels(),
	/**/ this->inputs[INPUT_POS_2].getChannels());
	this->playhead_count = channels;
	for (i = 0; i < channels; ++i) {

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
		this->playheads[i].level = level;

		/// MODE PLAYHEAD POS REL
		if (mode == IGC_MODE_POS_REL) {
			/// COMPUTE PHASE
			phase = knob_pos
			/**/ + this->inputs[INPUT_POS_1].getPolyVoltage(i) * 0.1 * mod_pos_1
			/**/ + this->inputs[INPUT_POS_2].getPolyVoltage(i) * 0.1 * mod_pos_2;
			phase = fmod(fmod(phase, 1.0) + 1.0, 1.0);
			this->playheads[i].phase = phase;

			/// COMPUTE REAL INDEX
			index = (float)this->audio_index
			/**/ - (phase * (float)buffer_length);
			if (index < 0)
				index += IGC_BUFFER;
			//index = fmod(fmod(index, IGC_BUFFER) + IGC_BUFFER, IGC_BUFFER);
		/// MODE SPEED
		} else {
			/// COMPUTE SPEED
			speed = knob_speed
			/**/ + this->inputs[INPUT_SPEED_1].getPolyVoltage(i) * mod_speed_1
			/**/ + this->inputs[INPUT_SPEED_2].getPolyVoltage(i) * mod_speed_2;
			if (knob_speed_round)
				speed = std::round(speed);
			speed = std::pow(2.f, speed);
			if (knob_speed_rev > 0)
				speed = -speed;
			if (this->inputs[INPUT_SPEED_REV].getPolyVoltage(i) > 0.0)
				speed = -speed;

			/// EASE SPEED
			// TODO: use parameter (context menu ?)
			speed = speed * 0.0005 + this->playheads[i].speed * 0.9995;
			this->playheads[i].speed = speed;

			/// COMPUTE PHASE / RELATIVE INDEX
			index = this->playheads[i].index;
			index += 1.0 - speed;
			index = fmod(fmod(index, buffer_length) + buffer_length, buffer_length);
			this->playheads[i].index = index;

			/// COMPUTE REAL INDEX
			index = (float)this->audio_index - index;
			while (index < 0)
				index += IGC_BUFFER;

			/// COMPUTE RELATIVE PHASE
			if (this->audio_index > index)
				phase = (float)this->audio_index - index;
			else
				phase = (float)this->audio_index + ((float)IGC_BUFFER - index);
			phase = fmod(fmod(phase / (float)buffer_length, 1.0) + 1.0, 1.0);
			this->playheads[i].phase = phase;
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
			// Ease toward just written audio when near buffer end
			if (this->audio_index > index) {
				dist = (float)buffer_length
				/**/ - ((float)this->audio_index - index);
			} else {
				dist = (float)buffer_length
				/**/ - ((float)this->audio_index + ((float)IGC_BUFFER - index));
			}
			if (dist < IGC_BUFFER_SAFE) {
				t = dist / (float)IGC_BUFFER_SAFE;
				voice_l = voice_l * t + in_l * (1.0 - t);
				voice_r = voice_r * t + in_r * (1.0 - t);
			}
		}

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
