
#include "Tracker.hpp"

/*

- Patterns
	- Name
	- Color
	- Swing	ratio							NOT HANDLED YET
	- Swing	signature						NOT HANDLED YET
	- Beat count
	- Note count
	- CV count
	- lpb (lines per beat)
	- Note columns
		- Column muted						NOT HANDLED YET
		- Column general effects
			- Fx velocity					NOT HANDLED YET
			- Fx panning					NOT HANDLED YET
			- Fx delay						NOT HANDLED YET
			- Fx chance						NOT HANDLED YET
			- Fx chance mode				NOT HANDLED YET
			- Fx octave						NOT HANDLED YET
			- Fx octave mode				NOT HANDLED YET
			- Fx pitch						NOT HANDLED YET
		- Column effect count
		- Lines (only set lines)
			- Line number
			- Note mode
			- ? Mode NEW
				- Note pitch
				- Note velocity
				- Note panning
				- Note synth
				- Note delay
				- Note glide
				- Note effects
					- Effect type
					- Effect value
			- ? Mode STOP
				- Note delay
			- ? Mode GLIDE
				- Note pitch
				- Note velocity
				- Note panning
				- Note glide
				- Note effects
					- Effect type
					- Effect value
	- CV columns
		- Column mode
		- Column synth
		- Column channel
		- Lines (only set lines)
			- Line number
			- CV mode
			- CV value
			- CV delay
			- CV curve

- Synths
	- Synth name
	- Synth color
	- Synth mode
	- Synth channel count

- Instances (Timeline)
	- Instance row
	- Instance beat
	- Pattern id
	- Instance beat start
	- Instance beat length
	- Muted

- Matrix									NOT HANDLED YET
	- Cells
		- Cell column
		- Cell line
		- Cell pattern id
		- Cell mode
		- ? Mode LOOP
		- ? Mode NEXT
		- ? Mode NEXT CIRCULAR
		- ? Mode PREV
		- ? Mode PREV CIRCULAR
		- ? Mode RAND
		- ? Mode XRAND
		- ? Mode GOTO
			- Cell line
		- ? Mode RAND AFTER
			- Cell line
		- ? Mode RAND BEFORE
			- Cell line

*/

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static void save_track(json_t *root) {
	json_t							*json_patterns;
	json_t							*json_pattern;
	json_t							*json_cols_note;
	json_t							*json_col_note;
	json_t							*json_cols_cv;
	json_t							*json_col_cv;
	json_t							*json_notes;
	json_t							*json_note;
	json_t							*json_fxs;
	json_t							*json_fx;
	json_t							*json_cvs;
	json_t							*json_cv;
	json_t							*json_synths;
	json_t							*json_synth;
	json_t							*json_instances;
	json_t							*json_instance;
	list<PatternInstance>::iterator	it, it_end;
	PatternSource					*pattern;
	PatternNoteCol					*note_col;
	PatternNote						*note;
	PatternCVCol					*cv_col;
	PatternCV						*cv;
	int								pattern_source;
	int								i, j, k, l;

	/// [1] ADD PATTERNS
	json_patterns = json_array();
	json_object_set_new(root, "patterns", json_patterns);

	for (i = 0; i < g_timeline->pattern_count; ++i) {
		pattern = &(g_timeline->patterns[i]);

		json_pattern = json_object();
		json_array_append_new(json_patterns, json_pattern);

		json_object_set_new(json_pattern, "name",			// Name
		/**/ json_string(pattern->name));
		json_object_set_new(json_pattern, "color",			// color
		/**/ json_integer(pattern->color));
		json_object_set_new(json_pattern, "beat_count",		// Beat count
		/**/ json_integer(pattern->beat_count));
		json_object_set_new(json_pattern, "note_count",		// Note count
		/**/ json_integer(pattern->note_count));
		json_object_set_new(json_pattern, "cv_count",		// CV count
		/**/ json_integer(pattern->cv_count));
		json_object_set_new(json_pattern, "lpb",			// lpb (lines per beat)
		/**/ json_integer(pattern->lpb));
		// TODO - Swing
		// TODO - Swing signature

		json_cols_note = json_array();
		json_object_set_new(json_pattern, "notes", json_cols_note);

		/// PATTERN NOTE COLUMNS
		for (j = 0; j < pattern->note_count; ++j) {
			note_col = pattern->notes[j];

			json_col_note = json_object();
			json_array_append_new(json_cols_note, json_col_note);

			// TODO - Muted
			// TODO - Fx velocity
			// TODO - Fx velocity
			// TODO - Fx panning
			// TODO - Fx delay
			// TODO - Fx chance
			// TODO - Fx chance mode
			// TODO - Fx octave
			// TODO - Fx octave mode
			// TODO - Fx pitch

			json_object_set_new(json_col_note, "effect_count",	// Effect count
			/**/ json_integer(note_col->fx_count));

			json_notes = json_array();
			json_object_set_new(json_col_note, "lines", json_notes);

			for (k = 0; k < pattern->line_count; ++k) {
				note = &(note_col->lines[k]);
				if (note->mode == PATTERN_NOTE_NEW) {
					json_note = json_object();
					json_array_append_new(json_notes, json_note);

					json_object_set_new(json_note, "line",		// Line number
					/**/ json_integer(k));
					json_object_set_new(json_note, "mode",		// Note mode
					/**/ json_integer(note->mode));
					json_object_set_new(json_note, "pitch",		// Note pitch
					/**/ json_integer(note->pitch));
					json_object_set_new(json_note, "velocity",	// Note velocity
					/**/ json_integer(note->velocity));
					json_object_set_new(json_note, "panning",	// Note panning
					/**/ json_integer(note->panning));
					json_object_set_new(json_note, "synth",		// Note synth
					/**/ json_integer(note->synth));
					json_object_set_new(json_note, "delay",		// Note delay
					/**/ json_integer(note->delay));
					json_object_set_new(json_note, "glide",		// Note glide
					/**/ json_integer(note->glide));

					json_fxs = json_array();
					json_object_set_new(json_note, "fxs", json_fxs);

					for (l = 0; l < note_col->fx_count; ++l) {
						if (note->effects[l].type != 0) {
							json_fx = json_object();
							json_array_append_new(json_fxs, json_fx);

							json_object_set_new(json_fx, "type",	// Fx type
							/**/ json_integer(note->effects[i].type));
							json_object_set_new(json_fx, "value",	// Fx value
							/**/ json_integer(note->effects[i].value));
						}
					}
				} else if (note->mode == PATTERN_NOTE_STOP) {
					json_note = json_object();
					json_array_append_new(json_notes, json_note);

					json_object_set_new(json_note, "line",		// Line number
					/**/ json_integer(k));
					json_object_set_new(json_note, "mode",		// Note mode
					/**/ json_integer(note->mode));
					json_object_set_new(json_note, "delay",		// Note delay
					/**/ json_integer(note->delay));
				} else if (note->mode == PATTERN_NOTE_GLIDE) {
					json_note = json_object();
					json_array_append_new(json_notes, json_note);

					json_object_set_new(json_note, "line",		// Line number
					/**/ json_integer(k));
					json_object_set_new(json_note, "mode",		// Note mode
					/**/ json_integer(note->mode));
					json_object_set_new(json_note, "pitch",		// Note pitch
					/**/ json_integer(note->pitch));
					json_object_set_new(json_note, "velocity",	// Note velocity
					/**/ json_integer(note->velocity));
					json_object_set_new(json_note, "panning",	// Note panning
					/**/ json_integer(note->panning));
					json_object_set_new(json_note, "glide",		// Note glide
					/**/ json_integer(note->glide));

					json_fxs = json_array();
					json_object_set_new(json_note, "fxs", json_fxs);

					for (l = 0; l < note_col->fx_count; ++l) {
						if (note->effects[l].type != 0) {
							json_fx = json_object();
							json_array_append_new(json_fxs, json_fx);

							json_object_set_new(json_fx, "type",	// Fx type
							/**/ json_integer(note->effects[l].type));
							json_object_set_new(json_fx, "value",	// Fx value
							/**/ json_integer(note->effects[l].value));
						}
					}
				}
			}
		}

		/// PATTERN CV COLUMNS
		json_cols_cv = json_array();
		json_object_set_new(json_pattern, "cvs", json_cols_cv);

		for (j = 0; j < pattern->cv_count; ++j) {
			cv_col = pattern->cvs[j];

			json_col_cv = json_object();
			json_array_append_new(json_cols_cv, json_col_cv);

			json_object_set_new(json_col_cv, "mode",		// Column mode (cv / bpm)
			/**/ json_integer(cv_col->mode));
			json_object_set_new(json_col_cv, "synth",		// Column synth (cv / bpm)
			/**/ json_integer(cv_col->synth));
			json_object_set_new(json_col_cv, "channel",		// Column channel (cv / bpm)
			/**/ json_integer(cv_col->channel));

			json_cvs = json_array();
			json_object_set_new(json_col_cv, "lines", json_cvs);

			for (k = 0; k < pattern->line_count; ++k) {
				cv = &(cv_col->lines[k]);
				if (cv->mode == PATTERN_CV_SET) {
					json_cv = json_object();
					json_array_append_new(json_cvs, json_cv);

					json_object_set_new(json_cv, "line",		// Line number
					/**/ json_integer(k));
					json_object_set_new(json_cv, "mode",		// CV mode
					/**/ json_integer(cv->mode));
					json_object_set_new(json_cv, "value",		// CV value
					/**/ json_integer(cv->value));
					json_object_set_new(json_cv, "delay",		// CV delay
					/**/ json_integer(cv->delay));
					json_object_set_new(json_cv, "curve",		// CV curve
					/**/ json_integer(cv->curve));
				}
			}
		}
	}

	/// [2] ADD SYNTHS
	json_synths = json_array();
	json_object_set_new(root, "synths", json_synths);

	for (i = 0; i < g_timeline->synth_count; ++i) {
		json_synth = json_object();
		json_array_append_new(json_synths, json_synth);

		json_object_set_new(json_synth, "name",				// Synth name
		/**/ json_string(g_timeline->synths[i].name + 5));
		json_object_set_new(json_synth, "color",			// Synth color
		/**/ json_integer(g_timeline->synths[i].color));
		json_object_set_new(json_synth, "mode",				// Synth mode (gate / trigger / drum)
		/**/ json_integer(g_timeline->synths[i].mode));
		json_object_set_new(json_synth, "polyphony",		// Synth channel count
		/**/ json_integer(g_timeline->synths[i].channel_count));
	}

	/// [3] ADD TIMELINE
	json_instances = json_array();
	json_object_set_new(root, "instances", json_instances);

	for (i = 0; i < 32; ++i) {
		it = g_timeline->timeline[i].begin();
		it_end = g_timeline->timeline[i].end();
		while (it != it_end) {

			pattern_source = ((intptr_t)it->source
			/**/ - (intptr_t)g_timeline->patterns) / sizeof(PatternSource);

			json_instance = json_object();
			json_array_append_new(json_instances, json_instance);

			json_object_set_new(json_instance, "row",		// Instance row
			/**/ json_integer(it->row));
			json_object_set_new(json_instance, "beat",		// Instance beat
			/**/ json_integer(it->beat));
			json_object_set_new(json_instance, "pattern",	// Instance pattern source index
			/**/ json_integer(pattern_source));
			json_object_set_new(json_instance, "start",		// Instance beat start
			/**/ json_integer(it->beat_start));
			json_object_set_new(json_instance, "length",	// Instance beat length
			/**/ json_integer(it->beat_length));
			json_object_set_new(json_instance, "muted",		// Instance muted
			/**/ json_integer(it->muted));

			it = std::next(it);
		}
	}

	/// [4] ADD MATRIX
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

json_t *Tracker::dataToJson(void) {
	json_t		*root;

	if (g_module != this)
		return NULL;

	root = json_object();

	/// [1] SAVE MIDI (DRIVER + DEVICE + CHANNEL)
	json_object_set_new(root, "midi", this->midi_input.toJson());
	/// SAVE EDITOR (JUMP + OCTAVE)
	json_object_set_new(root,
	/**/ "editor_jump", json_integer(g_editor->pattern_jump));
	json_object_set_new(root,
	/**/ "editor_octave", json_integer(g_editor->pattern_octave));


	/// [2] WAIT FOR THREAD FLAG
	while (g_timeline->thread_flag.test_and_set()) {}

	/// [3] SAVE TRACK
	save_track(root);

	/// [4] CLEAR THREAD FLAG
	g_timeline->thread_flag.clear();

	return root;
}
