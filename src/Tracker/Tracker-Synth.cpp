
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Synth::Synth() {
	this->active = false;
	this->index = 0;
	this->channel_cur = 0;
	this->channel_count = 6;
}

void Synth::init(int synth_index, int channel_count) {
	int			i, j;

	this->active = true;
	/// INIT VOICES
	this->index = synth_index;
	for (i = 0; i < 16; ++i) {
		this->voices[i].init(this->index, i);
	}
	/// INIT OUTPUTS
	for (i = 0; i < 16; ++i)
		for (j = 0; j < 4; ++j)
			this->out_synth[i * 4 + j] = 0.0;
	for (i = 0; i < 8; ++i)
			this->out_cv[i] = 0.0;
	/// SET CHANNEL COUNT
	this->channel_count = channel_count;
}

void Synth::process(float dt_sec, float dt_beat) {
	int			i;

	if (this->active == false)
		return;
	/// COMPUTE VOICES
	for (i = 0; i < 16; ++i)
		this->voices[i].process(dt_sec, dt_beat, this->out_synth);
}

SynthVoice* Synth::add(PatternNoteRow *row, PatternNote *note, int lpb) {
	SynthVoice*				voice;

	/// MODE DRUM
	if (row->mode == PATTERN_NOTE_MODE_DRUM) {
		voice = &(this->voices[note->pitch % 12]);
		if (voice->start(row, note, lpb) == true)
			return voice;
	/// MODE GATE + TRIGGER
	} else {
		voice = &(this->voices[this->channel_cur]);
		if (voice->start(row, note, lpb) == true) {
			this->channel_cur = (this->channel_cur + 1) % channel_count;
			return voice;
		}
	}
	return NULL;
}
