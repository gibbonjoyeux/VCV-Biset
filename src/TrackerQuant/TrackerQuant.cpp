
#include "TrackerQuant.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerQuant::TrackerQuant() {
	int		i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	for (i = 0; i < 4; ++i) {
		configParam(PARAM_OCTAVE + i, -4, +4, 0, string::f("Octave %d", i + 1))->snapEnabled = true;
		configInput(INPUT_QUANT + i, string::f("%d", i + 1));
		configOutput(OUTPUT_QUANT + i, string::f("%d", i + 1));
	}
}

void TrackerQuant::process(const ProcessArgs& args) {
	float	value;
	float	pitch;
	float	pitch_oct;
	float	pitch_note;
	float	pitch_quant;
	int		channels;
	int		i, j;

	if (g_module == NULL)
		return;
	/// FOR EACH INPUT
	for (i = 0; i < 4; ++i) {
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
			pitch_quant = g_timeline->pitch_scale[(int)pitch_note] / 12.0
			/**/ + g_timeline->pitch_base_offset;
			/// HANDLE OUTPUT PITCH
			pitch = (float)pitch_oct + (float)pitch_quant;
			pitch += this->params[PARAM_OCTAVE + i].getValue();
			this->outputs[OUTPUT_QUANT + i].setVoltage(pitch, j);
		}
	}
}

Model* modelTrackerQuant = createModel<TrackerQuant, TrackerQuantWidget>("Biset-Tracker-Quant");
