
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

	configInput(INPUT_PLAYHEAD, "Playhead position");

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
	int		buffer_index_a, buffer_index_b;
	int		channels;
	int		i;

	delay_time = this->params[PARAM_DELAY_TIME].getValue();

	/// RECORD AUDIO BUFFER
	in_l = this->inputs[INPUT_L].getVoltageSum();
	if (this->inputs[INPUT_R].isConnected())
		in_r = this->inputs[INPUT_R].getVoltageSum();
	else
		in_r = in_l;
	this->audio[0][this->audio_index] = in_l;
	this->audio[1][this->audio_index] = in_r;

	/// READ AUDIO BUFFER
	out_l = 0.0;
	out_r = 0.0;
	channels = this->inputs[INPUT_PLAYHEAD].getChannels();
	this->playhead_count = channels;
	for (i = 0; i < channels; ++i) {
		/// COMPUTE PHASE
		phase = this->inputs[INPUT_PLAYHEAD].getVoltage(i) * 0.1;
		this->playheads[i].phase = fmod(fmod(phase, 1.0) + 1.0, 1.0);

		/// COMPUTE REAL INDEX
		index = (float)this->audio_index
		/**/ - (phase * (float)IGC_BUFFER * delay_time);
		index = fmod(fmod(index, IGC_BUFFER) + IGC_BUFFER, IGC_BUFFER);

		/// COMPUTE BUFFER INDEX
		buffer_index_a = (int)index;
		buffer_index_b = buffer_index_a + 1;
		if (buffer_index_b >= IGC_BUFFER)
			buffer_index_b = 0;
		index_inter = index - (float)buffer_index_a;

		/// COMPUTE INTERPOLATED VALUE
		out_l += this->audio[0][buffer_index_a] * (1.0 - index_inter)
		/**/ + this->audio[0][buffer_index_b] * index_inter;
		out_r += this->audio[1][buffer_index_a] * (1.0 - index_inter)
		/**/ + this->audio[1][buffer_index_b] * index_inter;
	}
	this->outputs[OUTPUT_L].setVoltage(out_l);
	this->outputs[OUTPUT_R].setVoltage(out_r);

	/// LOOP AUDIO BUFFER
	this->audio_index += 1;
	if (this->audio_index >= IGC_BUFFER)
		this->audio_index = 0;
}

Model* modelIgc = createModel<Igc, IgcWidget>("Biset-Igc");
