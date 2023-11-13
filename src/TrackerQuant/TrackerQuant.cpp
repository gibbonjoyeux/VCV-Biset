
#include "TrackerQuant.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static inline float quantize_index_down(float pitch_note) {
	return g_timeline->pitch_scale[(int)pitch_note] / 12.0;
}

static inline float quantize_index_up(float pitch_note) {
	int		index;

	index = ceil(pitch_note);
	if (index < 12)
		return g_timeline->pitch_scale[index] / 12.0;
	return g_timeline->pitch_scale[0] / 12.0 + 1.0;
}

static inline float quantize_index_round(float pitch_note) {
	int		index;

	index = (int)(pitch_note + 0.5);
	if (index < 12)
		return g_timeline->pitch_scale[index] / 12.0;
	return g_timeline->pitch_scale[0] / 12.0 + 1.0;
}

static inline float quantize_nearest(float pitch_note) {
	float	pitch;
	float	weight;
	float	nearest_pitch;
	float	nearest_weight;
	int		i;

	nearest_pitch = 0.0;
	nearest_weight = INFINITY;
	for (i = 0; i < 12; ++i) {
		pitch = g_timeline->pitch_scale[i];
		weight = abs(pitch - pitch_note);
		if (weight < nearest_weight) {
			nearest_pitch = pitch;
			nearest_weight = weight;
		}
	}
	return nearest_pitch / 12.0 + g_timeline->pitch_base_offset;
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerQuant::TrackerQuant() {
	int		i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	for (i = 0; i < 4; ++i) {
		configParam(PARAM_OCTAVE + i, -4, +4, 0, rack::string::f("Octave %d", i + 1))->snapEnabled = true;
		configInput(INPUT_QUANT + i, rack::string::f("%d", i + 1));
		configOutput(OUTPUT_QUANT + i, rack::string::f("%d", i + 1));
		configParam(PARAM_MODE + i, 0, 3, 0, "Mode")->snapEnabled = true;
	}
}

void TrackerQuant::process(const ProcessArgs& args) {
	float	pitch;
	float	pitch_oct;
	float	pitch_note;
	float	pitch_quant;
	int		channels;
	int		mode;
	int		i, j;

	if (g_module == NULL || g_timeline == NULL)
		return;

	/// FOR EACH INPUT
	for (i = 0; i < 4; ++i) {
		mode = this->params[PARAM_MODE + i].getValue();
		channels = std::max(this->inputs[INPUT_QUANT + i].getChannels(), 1);
		this->outputs[OUTPUT_QUANT + i].setChannels(channels);
		/// FOR EACH CHANNEL
		for (j = 0; j < channels; ++j) {
			/// HANDLE INPUT PITCH
			pitch = this->inputs[INPUT_QUANT + i].getVoltage(j);
			pitch_note = modff(pitch, &pitch_oct) * 12.0;
			if (pitch_oct < 0.f || pitch_note < 0.f) {
				if (abs(pitch_note) < 1e-7) {
					pitch_note = 0.f;
				} else {
					pitch_note += 12.f;
					pitch_oct -= 1.f;
				}
			}
			/// HANDLE QUANTIZED PITCH
			if (mode == TQUANT_MODE_INDEX_DOWN)
				pitch_quant = quantize_index_down(pitch_note);
			else if (mode == TQUANT_MODE_INDEX_UP)
				pitch_quant = quantize_index_up(pitch_note);
			else if (mode == TQUANT_MODE_INDEX_ROUND)
				pitch_quant = quantize_index_round(pitch_note);
			else
				pitch_quant = quantize_nearest(pitch_note);
			/// HANDLE OUTPUT PITCH
			pitch = pitch_oct + pitch_quant + g_timeline->pitch_base_offset;
			pitch += this->params[PARAM_OCTAVE + i].getValue();
			this->outputs[OUTPUT_QUANT + i].setVoltage(pitch, j);
		}
	}
}

Model* modelTrackerQuant = createModel<TrackerQuant, TrackerQuantWidget>("Biset-Tracker-Quant");
