
#include "plugin.hpp"

SynthVoice::SynthVoice() {
	this->reset();
}

void SynthVoice::process(
	float					dt_sec,
	float					dt_beat,
	float					*output) {
	float					pitch, mix;
	float					velocity;
	float					vibrato, tremolo;

	/// ON NOTE PLAY
	if (this->active && this->delay <= 0 && this->delay_gate <= 0) {
		/// COMPUTE CV
		//pitch = (float)(this->pitch - 69) / 12.0f;
		velocity = (float)this->velocity / 25.5;
		/// COMPUTE EFFECTS
		//// COMPUTE GLIDE
		if (this->pitch_glide_len > 0) {
			this->pitch_glide_cur += dt_beat;
			/// GLIDE ENDED
			if (this->pitch_glide_cur >= this->pitch_glide_len) {
				this->pitch_glide_len = 0;
				this->pitch_glide_cur = 0;
				this->pitch_from = this->pitch_to;
				pitch = (float)(this->pitch_from - 69.0) / 12.0f;
			/// GLIDE RUNNING
			} else {
				mix = this->pitch_glide_cur / this->pitch_glide_len;
				pitch = ((this->pitch_from * (1.0 - mix)
				/**/ + this->pitch_to * mix) - 69.0) / 12.0f;
			}
		} else {
			pitch = (float)(this->pitch_from - 69) / 12.0f;
		}
		//// COMPUTE VIBRATO
		if (this->vibrato_amp > 0) {
			this->vibrato_phase += this->vibrato_freq * dt_beat;
			vibrato = sinf(this->vibrato_phase) * this->vibrato_amp;
			pitch += vibrato;
		}
		//// COMPUTE TREMOLO
		if (this->tremolo_amp > 0) {
			this->tremolo_phase += this->tremolo_freq * dt_beat;
			tremolo = 0.5f + sinf(this->tremolo_phase) * 0.5f
			/**/ * this->tremolo_amp;
			velocity *= tremolo;
		}
		/// COMPUTE STOPPING DELAY
		if (this->delay_stop > 0) {
			this->delay_stop -= dt_beat;
			if (this->delay_stop < 0)
				this->active = false;
		}
		/// SET OUTPUT (PITCH + GATE + VELOCITY)
		output[this->channel * 3 + 0] = pitch;
		output[this->channel * 3 + 1] = 10.0f;
		output[this->channel * 3 + 2] = velocity;
	/// ON NOTE DELAY
	} else {
		/// COMPUTE NOTE STARTING DELAY
		if (this->delay > 0)
			this->delay -= dt_beat;
		/// COMPUTE INTER NOTES GATE DELAY
		if (this->delay_gate > 0)
			this->delay_gate -= dt_sec;
		/// SET OUTPUT (GATE)
		output[this->channel * 3 + 1] = 0.0f;
	}
}

bool SynthVoice::start(
	PatternNote				*note,
	int						lpb) {
	PatternEffect			*effect;
	int						i;

	delay = (1.0f / (float)lpb) * ((float)note->delay / 256.0f);
	/// START NOTE ON CHANCE
	if (note->chance == 255 || random::uniform() * 255.0 < note->chance) {
		/// SET INTER NOTE GATE DELAY
		this->delay_gate = (this->active) ? 0.001f : 0.0f;
		/// SET MAIN DELAY
		this->delay = delay;
		this->delay_stop = 0;
		/// SET NOTE PROPS
		this->velocity = note->velocity;
		this->pitch = note->pitch;
		this->pitch_from = note->pitch;
		this->pitch_to = note->pitch;
		this->pitch_glide_len = 0;
		this->pitch_glide_cur = 0;
		/// SET EFFECTS
		this->vibrato_amp = 0;
		this->tremolo_amp = 0;
		for (i = 0; i < 8; ++i) {
			effect = &(note->effects[i]);
			switch(effect->type) {
				case PATTERN_EFFECT_NONE:
					break;
				case PATTERN_EFFECT_VIBRATO:
					this->vibrato_freq =
					/**/ (float)(effect->value / 16) * M_PI * 2.0f;
					this->vibrato_amp = (float)(effect->value % 16) / 64.0;
					break;
				case PATTERN_EFFECT_TREMOLO:
					this->tremolo_freq =
					/**/ (float)(effect->value / 16) * M_PI * 2.0f;
					this->tremolo_amp = (float)(effect->value % 16) / 16.0;
					break;
				case PATTERN_EFFECT_FADE_IN:
					break;
				case PATTERN_EFFECT_FADE_OUT:
					break;
				case PATTERN_EFFECT_RACHET:
					break;
			}
		}
		/// ACTIVATE NOTE
		this->active = true;
		return true;
	}
	return false;
}

void SynthVoice::glide(
	PatternNote				*note) {
	float					mix;

	if (this->active) {
		/// GLIDE ALREADY RUNNING
		if (this->pitch_glide_len > 0) {
			/// COMPUTE GLIDING PITCH
			mix = this->pitch_glide_cur / this->pitch_glide_len;
			this->pitch_from = this->pitch_from * (1.0 - mix)
			/**/ + this->pitch_to * mix;
		/// GLIDE OFF
		} else {
		}
		this->pitch_to = note->pitch;
		this->pitch_glide_len = (1.0 - ((float)note->glide / 256.0));
		this->pitch_glide_cur = 0;
	}
}

void SynthVoice::stop(PatternNote *note, int lpb) {
	// TODO: get stopping line to get delay
	// Stop only after certain delay
	if (note) {
		if (note->delay > 0) {
			this->delay_stop =
			/**/ (1.0f / (float)lpb) * ((float)note->delay / 256.0f);
		} else {
			this->active = false;
		}
	} else {
		this->active = false;
	}
}

void SynthVoice::init(int synth, int channel) {
	this->reset();
	this->synth = synth;
	this->channel = channel;
}

void SynthVoice::reset() {
	this->active = false;
	this->pitch = 0;
	this->velocity = 255;
	this->pitch_glide_len = 0;
	this->pitch_glide_cur = 0.0;
	this->pitch_from = 0.0;
	this->pitch_to = 0.0;
	this->delay = 0;
	this->vibrato_amp = 0;
	this->vibrato_freq = 0;
	this->vibrato_phase = 0.0;
	this->tremolo_amp = 0;
	this->tremolo_freq = 0;
	this->tremolo_phase = 0.0;
}
