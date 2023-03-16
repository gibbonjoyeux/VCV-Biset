
#include "plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

SynthVoice::SynthVoice() {
	this->reset();
}

void SynthVoice::process(
	float					dt_sec,
	float					dt_beat,
	float					*output) {
	float					pitch, mix;
	float					velocity;
	float					panning;
	float					vibrato, tremolo;

	/// ON NOTE PLAY
	if (this->active && this->delay <= 0 && this->delay_gate <= 0) {
		/// COMPUTE CV
		//pitch = (float)(this->pitch - 69) / 12.0f;
		velocity = (float)this->velocity / 255.0 * 10.0;
		panning = (float)this->panning / 255.0 * 10.0;
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
		output[this->channel * 4 + 0] = pitch;
		output[this->channel * 4 + 1] = 10.0f;
		output[this->channel * 4 + 2] = velocity;
		output[this->channel * 4 + 3] = panning;
	/// ON NOTE DELAY
	} else {
		/// COMPUTE NOTE STARTING DELAY
		if (this->delay > 0)
			this->delay -= dt_beat;
		/// COMPUTE INTER NOTES GATE DELAY
		if (this->delay_gate > 0)
			this->delay_gate -= dt_sec;
		/// SET OUTPUT (GATE)
		output[this->channel * 4 + 1] = 0.0f;
	}
}

bool SynthVoice::start(
	PatternNote				*note,
	int						lpb) {
	PatternEffect			*effect;
	int						i;
	int						x, y;
	int						int_1, int_2;
	float					float_1;

	delay = (1.0f / (float)lpb) * ((float)note->delay / 256.0f);
	/// SET INTER NOTE GATE DELAY
	this->delay_gate = (this->active) ? 0.001f : 0.0f;
	/// SET MAIN DELAY
	this->delay = delay;
	this->delay_stop = 0;
	/// SET NOTE PROPS
	this->velocity = note->velocity;
	this->panning = note->panning;
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
			case PATTERN_EFFECT_RAND_AMP:		// Axx
				float_1 = random::uniform() * (effect->value / 255.0);
				this->velocity *= 1.0 - float_1;
				break;
			case PATTERN_EFFECT_RAND_PAN:		// Pxx
				break;
			case PATTERN_EFFECT_RAND_OCT:		// Oxy
				x = effect->value / 16;
				y = effect->value % 16;
				if (y > 0) {
					if (x == 0)					// > -+
						int_1 = (random::u32() % (2 * y) - y) * 12;
					else if (x == 1)			// > +
						int_1 = (random::u32() % (y + 1)) * 12;
					else						// > -
						int_1 = -(random::u32() % (y + 1)) * 12;
					this->pitch += int_1;
					this->pitch_from += int_1;
					this->pitch_to += int_1;
				}
				break;
			case PATTERN_EFFECT_RAND_PITCH:		// Mxy
				x = effect->value / 16;
				y = effect->value % 16;
				int_1 = random::u32() % 3;
				if (int_1 == 0)
					int_2 = this->pitch;
				else if (int_1 == 1)
					int_2 = this->pitch + x;
				else
					int_2 = this->pitch + y;
				this->pitch = int_2;
				this->pitch_from = int_2;
				this->pitch_to = int_2;
				break;
			case PATTERN_EFFECT_RAND_SCALE:		// Sxy
				break;
			case PATTERN_EFFECT_VIBRATO:		// Vxy
				this->vibrato_freq =
				/**/ (float)(effect->value / 16) * M_PI * 2.0f;
				this->vibrato_amp = (float)(effect->value % 16) / 64.0;
				break;
			case PATTERN_EFFECT_TREMOLO:		// Txy
				this->tremolo_freq =
				/**/ (float)(effect->value / 16) * M_PI * 2.0f;
				this->tremolo_amp = (float)(effect->value % 16) / 32.0;
				break;
			case PATTERN_EFFECT_FADE_IN:		// Fxx
				break;
			case PATTERN_EFFECT_FADE_OUT:		// fxx
				break;
			case PATTERN_EFFECT_CHANCE:			// Cxx
				if (random::uniform() * 255.0 > effect->value)
					return false;
				break;
			case PATTERN_EFFECT_CHANCE_STOP:	// cxx
				break;
			case PATTERN_EFFECT_RACHET:
				break;
		}
	}
	/// ACTIVATE NOTE
	this->active = true;
	return true;
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
