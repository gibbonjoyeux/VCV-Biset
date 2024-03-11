
#include "Igc.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Igc::Igc() {

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configParam(PARAM_DELAY_TIME, 0.002, 1.0, 0.1, "Delay time", "s", 0, 10.0);

	configSwitch(PARAM_MODE, 0, 3, 0, "Mode", {
		"Pos relative",
		"Pos absolute",
		"Speed",
		"Grain"
	});

	configParam(PARAM_POS, 0.0, 1.0, 0.0, "Playhead position", "");
	configInput(INPUT_POS_1, "Playhead position 1");
	configParam(PARAM_POS_MOD_1, -1.0, 1.0, 1.0, "Playhead position 1 mod", "");
	configInput(INPUT_POS_2, "Playhead position 2");
	configParam(PARAM_POS_MOD_2, -1.0, 1.0, 1.0, "Playhead position 2 mod", "");

	configParam(PARAM_SPEED, 0.0, 1.0, 0.0, "Playhead speed", "");
	configInput(INPUT_SPEED_1, "Playhead speed 1");
	configParam(PARAM_SPEED_MOD_1, -1.0, 1.0, 1.0, "Playhead speed 1 mod", "");
	configInput(INPUT_SPEED_2, "Playhead speed 2");
	configParam(PARAM_SPEED_MOD_2, -1.0, 1.0, 1.0, "Playhead speed 2 mod", "");

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
}

void Igc::process(const ProcessArgs& args) {
	float	in_l, in_r;
	float	out_l, out_r;
	float	phase;
	float	index;
	float	index_inter;
	float	delay_time;
	float	level;
	float	mod_pos_1, mod_pos_2;
	float	mod_lvl_1, mod_lvl_2;
	int		buffer_index_a, buffer_index_b;
	int		buffer_length;
	int		channels;
	int		mode;
	int		i;

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
	delay_time = this->params[PARAM_DELAY_TIME].getValue();
	mod_pos_1 = this->params[PARAM_POS_MOD_1].getValue();
	mod_pos_2 = this->params[PARAM_POS_MOD_2].getValue();
	mod_lvl_1 = this->params[PARAM_LVL_MOD_1].getValue();
	mod_lvl_2 = this->params[PARAM_LVL_MOD_2].getValue();

	//////////////////////////////	
	/// [3] RECORD AUDIO
	//////////////////////////////	
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
	buffer_length = (float)IGC_BUFFER * delay_time;
	channels = std::max(this->inputs[INPUT_POS_1].getChannels(),
	/**/ this->inputs[INPUT_POS_2].getChannels());
	this->playhead_count = channels;
	for (i = 0; i < channels; ++i) {

		/// COMPUTE LEVEL
		level = this->params[PARAM_LVL].getValue()
		/**/ + this->inputs[INPUT_LVL_1].getPolyVoltage(i) * 0.1
		/**/ * mod_lvl_1
		/**/ + this->inputs[INPUT_LVL_2].getPolyVoltage(i) * 0.1
		/**/ * mod_lvl_2;
		if (level > 1.0)
			level = 1.0;
		if (level < 0.0)
			level = 0.0;
		this->playheads[i].level = level;

		/// COMPUTE PHASE
		phase = this->params[PARAM_POS].getValue()
		/**/ + this->inputs[INPUT_POS_1].getPolyVoltage(i) * 0.1 * mod_pos_1
		/**/ + this->inputs[INPUT_POS_2].getPolyVoltage(i) * 0.1 * mod_pos_2;
		phase = fmod(fmod(phase, 1.0) + 1.0, 1.0);
		this->playheads[i].phase = phase;

		/// COMPUTE REAL INDEX
		index = (float)this->audio_index
		/**/ - (phase * (float)buffer_length);
		if (index < 0)
			index += buffer_length;
		//index = fmod(fmod(index, IGC_BUFFER) + IGC_BUFFER, IGC_BUFFER);

		/// COMPUTE BUFFER INDEX
		buffer_index_a = (int)index;
		buffer_index_b = buffer_index_a + 1;
		if (buffer_index_b >= buffer_length)
			buffer_index_b = 0;
		//if (buffer_index_b >= IGC_BUFFER)
		//	buffer_index_b = 0;
		index_inter = index - (float)buffer_index_a;

		/// COMPUTE INTERPOLATED VALUE
		out_l += (this->audio[0][buffer_index_a] * (1.0 - index_inter)
		/**/ + this->audio[0][buffer_index_b] * index_inter) * level;
		out_r += (this->audio[1][buffer_index_a] * (1.0 - index_inter)
		/**/ + this->audio[1][buffer_index_b] * index_inter) * level;
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
