
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

PatternInstance::PatternInstance() {
	this->reset();
}

void PatternInstance::reset() {
	int						i;

	for (i = 0; i < 32; ++i) {
		this->voices[i] = NULL;
		this->notes[i] = NULL;
	}
}

void PatternInstance::process(
	Synth					*synths,
	PatternSource			*pattern,
	Clock					clock,
	int						*debug,
	int						*debug_2,
	char					*debug_str) {
	int						line, row;
	Synth					*synth;
	PatternCVRow			*col_cv;
	PatternCV				*cv_line, *cv_from, *cv_to;
	PatternNoteRow			*row_note;
	PatternNote				*note;
	SynthVoice				*voice;
	int						line_from, line_to;
	float					phase, phase_from, phase_to;
	float					cv_phase;
	float					cv_value;
	float					delay;

	line = clock.beat * pattern->lpb + clock.phase * pattern->lpb;
	phase = clock.phase * pattern->lpb;
	phase -= (int)phase;
	pattern->line_play = line;

	*debug = line;
	*debug_2 = 0;
	//sprintf(debug_str, "%d x %d (%d:%d) %d/%d", clock.beat, line,
	///**/ note->mode, note->synth,
	///**/ voice->channel, module->outputs[1].getChannels());

	/// [1] COMPUTE PATTERN NOTE ROWS
	for (row = 0; row < pattern->note_count; ++row) {
		row_note = pattern->notes[row];
		note = &(row_note->lines[line]);
		/// CELL CHANGE
		if (note != this->notes[row]) {
			/// NOTE KEEP
			if (note->mode == PATTERN_NOTE_KEEP) {
				this->notes[row] = note;
			/// NOTE ACTION
			} else {
				/// HANDLE DELAY
				delay = (float)note->delay / 100.0f;
				if (phase > delay) {
					voice = this->voices[row];
					/// NOTE CHANGE
					if (note->mode == PATTERN_NOTE_NEW) {
						/// CLOSE ACTIVE NOTE
						if (voice) {
							voice->stop();
							this->voices[row] = NULL;
						}
						/// ADD NEW NOTE
						if (note->synth < 64) {
							voice = synths[note->synth]
							/**/ .add(row_note, note, pattern->lpb);
							this->voices[row] = voice;
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
							this->voices[row] = NULL;
						}
					}
					this->notes[row] = note;
				}
			}
		}
	}
	/// [2] COMPUTE PATTERN CV ROWS
	for (row = 0; row < pattern->cv_count; ++row) {
		col_cv = pattern->cvs[row];
		/// [A] COMPUTE KEY CV INTERPOLATION LINES
		cv_line = &(col_cv->lines[line]);
		cv_from = NULL;
		cv_to = NULL;
		line_from = 0;
		line_to = 0;
		//// ON ACTIVE LINE
		if (cv_line->mode == PATTERN_CV_SET) {
			/// AFTER DELAY
			if (phase * 99.0 >= cv_line->delay) {
				cv_from = cv_line;
				line_from = line;
			/// BEFORE DELAY
			} else {
				cv_to = cv_line;
				line_to = line;
			}
		//// ON PASSIVE LINE
		} else {
		}
		//// FIND LINE FROM
		if (cv_from == NULL) {
			line_from = line - 1;
			while (line_from >= 0
			&& pattern->cvs[row]->lines[line_from].mode != PATTERN_CV_SET)
				line_from -= 1;
			if (line_from >= 0)
				cv_from = &(pattern->cvs[row]->lines[line_from]);
		}
		//// FIND LINE TO
		if (cv_to == NULL) {
			line_to = line + 1;
			while (line_to < pattern->line_count
			&& pattern->cvs[row]->lines[line_to].mode != PATTERN_CV_SET)
				line_to += 1;
			if (line_to < pattern->line_count)
				cv_to = &(pattern->cvs[row]->lines[line_to]);
		}
		/// [B] COMPUTE CV
		//// WITH BOTH INTERPOLATION LINES
		if (cv_from && cv_to) {
			/// COMPUTE CV PHASE
			if (line_from == line_to) {
				cv_phase = 0;
			} else {
				phase_from = (float)line_from + (float)cv_from->delay / 99.0;
				phase_to = (float)line_to + (float)cv_to->delay / 99.0;
				cv_phase = (((float)line + phase) - phase_from)
				/**/ / (phase_to - phase_from);
			}
			/// COMPUTE CV VALUE
			cv_value = (float)cv_from->value +
			/**/ ((float)cv_to->value - (float)cv_from->value)
			/**/ * cv_phase;
		//// WITHOUT BOTH INTERPOLATION LINES
		} else {
			if (cv_from) {
				cv_value = cv_from->value;
			} else if (cv_to) {
				cv_value = cv_to->value;
			} else {
				if (col_cv->mode == PATTERN_CV_MODE_CV)
					cv_value = 500.0;
				else if (col_cv->mode == PATTERN_CV_MODE_BPM)
					cv_value = 120.0;
			}
		}
		/// [D] OUTPUT CV
		if (col_cv->mode == PATTERN_CV_MODE_CV) {
			/// REMAP CV FROM [0:999] TO [0:10]
			cv_value /= 100.0;
			/// OUTPUT CV
			synth = &(synths[pattern->cvs[row]->synth]);
			synth->out_cv[pattern->cvs[row]->channel] = cv_value;
		} else if (col_cv->mode == PATTERN_CV_MODE_BPM) {
			/// CLAMP CV ON [30:300]
			if (cv_value < 30)
				cv_value = 30;
			if (cv_value > 300)
				cv_value = 300;
			/// OUTPUT CV
			g_editor.module->params[Tracker::PARAM_BPM].setValue(cv_value);
		}
	}
}

void PatternInstance::stop() {
	int						row;

	for (row = 0; row < 32; ++row) {
		if (this->voices[row])
			this->voices[row]->stop();
		this->voices[row] = NULL;
		this->notes[row] = NULL;
	}
}
