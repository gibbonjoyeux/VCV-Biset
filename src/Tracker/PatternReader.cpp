
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

PatternReader::PatternReader() {
	this->reset();
}

void PatternReader::reset() {
	int						i;

	for (i = 0; i < 32; ++i) {
		this->voices[i] = NULL;
		this->notes[i] = NULL;
	}
}

void PatternReader::process(
	Synth					*synths,
	PatternSource			*pattern,
	Clock					clock) {
	int						line, col;
	Synth					*synth;
	PatternCVCol			*col_cv;
	PatternCV				*cv_line, *cv_prev, *cv_next;
	PatternNoteCol			*col_note;
	PatternNote				*note;
	SynthVoice				*voice;
	SynthVoice				*voice_new;
	int						line_prev, line_next;
	float					phase, phase_prev, phase_next;
	float					cv_phase;
	float					cv_value;
	float					delay;
	int						state;

	line = clock.beat * pattern->lpb + clock.phase * pattern->lpb;
	phase = clock.phase * pattern->lpb;
	phase -= (int)phase;
	pattern->line_play = line;
	pattern->line_phase = phase;

	/// [1] COMPUTE PATTERN NOTE COLS
	for (col = 0; col < pattern->note_count; ++col) {
		col_note = pattern->notes[col];
		note = &(col_note->lines[line]);
		/// CELL CHANGE
		if (note != this->notes[col]) {
			/// NOTE KEEP
			if (note->mode == PATTERN_NOTE_KEEP) {
				this->notes[col] = note;
			/// NOTE ACTION
			} else {
				/// HANDLE DELAY
				delay = (float)note->delay / 100.0f;
				if (phase > delay) {
					voice = this->voices[col];
					/// NOTE NEW
					if (note->mode == PATTERN_NOTE_NEW) {
						/// ADD NEW NOTE
						voice_new = synths[note->synth]
						/**/ .add(col_note, note, pattern->lpb, &state);
						if (state == VOICE_ADD_ADD
						|| state == VOICE_ADD_STOP) {
							/// CLOSE ACTIVE NOTE
							if (voice && voice != voice_new)
								voice->stop();
							this->voices[col] = voice_new;
						}
					/// NOTE GLIDE
					} else if (note->mode == PATTERN_NOTE_GLIDE) {
						if (voice) {
							voice->glide(note);
						}
					/// NOTE STOP
					} else if (note->mode == PATTERN_NOTE_STOP) {
						/// CLOSE ACTIVE NOTE
						if (voice) {
							voice->stop();
							this->voices[col] = NULL;
						}
					}
					this->notes[col] = note;
				}
			}
		}
	}
	/// [2] COMPUTE PATTERN CV COLS
	for (col = 0; col < pattern->cv_count; ++col) {
		col_cv = pattern->cvs[col];
		/// [A] COMPUTE KEY CV INTERPOLATION LINES
		cv_line = &(col_cv->lines[line]);
		cv_prev = NULL;
		cv_next = NULL;
		line_prev = 0;
		line_next = 0;
		//// HANDLE LINE CURRENT
		if (cv_line->mode == PATTERN_CV_SET) {
			/// AFTER DELAY
			if (phase * 99.0 >= cv_line->delay) {
				cv_prev = cv_line;
				line_prev = line;
			/// BEFORE DELAY
			} else {
				cv_next = cv_line;
				line_next = line;
			}
		}
		//// HANDLE LINE PREV
		if (cv_prev == NULL) {
			line_prev = pattern->cv_prev(col, line);
			if (line_prev >= 0)
				cv_prev = &(col_cv->lines[line_prev]);
		}
		//// HANDLE LINE NEXT
		if (cv_next == NULL) {
			line_next = pattern->cv_next(col, line);
			if (line_next >= 0)
				cv_next = &(col_cv->lines[line_next]);
		}
		/// [B] COMPUTE CV
		//// WITH BOTH INTERPOLATION LINES
		if (col_cv->mode == PATTERN_CV_MODE_CV
		|| col_cv->mode == PATTERN_CV_MODE_BPM) {
			if (cv_prev && cv_next) {
				/// COMPUTE CV PHASE
				if (line_prev == line_next) {
					cv_phase = 0;
				} else {
					phase_prev = (float)line_prev
					/**/ + (float)cv_prev->delay / 99.0;
					phase_next = (float)line_next
					/**/ + (float)cv_next->delay / 99.0;
					cv_phase = (((float)line + phase) - phase_prev)
					/**/ / (phase_next - phase_prev);
				}
				/// COMPUTE CV VALUE
				cv_value = (float)cv_prev->value +
				/**/ ((float)cv_next->value - (float)cv_prev->value)
				/**/ * cv_phase;
			//// WITHOUT BOTH INTERPOLATION LINES
			} else {
				if (cv_prev) {
					cv_value = cv_prev->value;
				} else if (cv_next) {
					cv_value = cv_next->value;
				} else {
					if (col_cv->mode == PATTERN_CV_MODE_CV)
						cv_value = 500.0;
					else if (col_cv->mode == PATTERN_CV_MODE_BPM)
						cv_value = 120.0;
				}
			}
		} else if (col_cv->mode == PATTERN_CV_MODE_GATE) {
			if (cv_prev)
				cv_value = cv_prev->value;
			else if (cv_next)
				cv_value = cv_next->value;
			else
				cv_value = 0.0;
		}
		/// [D] OUTPUT CV
		//// MODE CV
		if (col_cv->mode == PATTERN_CV_MODE_CV) {
			/// REMAP CV PREV [0:999] TO [0:1]
			cv_value /= 1000.0;
			/// OUTPUT CV
			synth = &(synths[pattern->cvs[col]->synth]);
			synth->out_cv[pattern->cvs[col]->channel] = cv_value;
		//// MODE GATE
		} else if (col_cv->mode == PATTERN_CV_MODE_GATE) {
			cv_value = (cv_value < 1.0) ? 0.0 : 1.0;
			/// OUTPUT CV
			synth = &(synths[pattern->cvs[col]->synth]);
			synth->out_cv[pattern->cvs[col]->channel] = cv_value;
		//// MODE BPM
		} else if (col_cv->mode == PATTERN_CV_MODE_BPM) {
			/// CLAMP CV ON [30:300]
			if (cv_value < 30)
				cv_value = 30;
			if (cv_value > 300)
				cv_value = 300;
			/// OUTPUT CV
			g_module->params[Tracker::PARAM_BPM].setValue(cv_value);
		}
	}
}

void PatternReader::stop() {
	int						col;

	for (col = 0; col < 32; ++col) {
		if (this->voices[col])
			this->voices[col]->stop();
		this->voices[col] = NULL;
		this->notes[col] = NULL;
	}
}
