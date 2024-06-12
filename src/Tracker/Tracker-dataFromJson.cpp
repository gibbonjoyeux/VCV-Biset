
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static void load_midi(json_t *root) {
	json_t			*obj;

	obj = json_object_get(root, "midi");
	if (obj)
		g_module->midi_input.fromJson(obj);
	/// RECOVER EDITOR (JUMP + OCTAVE)
	obj = json_object_get(root, "editor_jump");
	if (obj)
		g_editor->pattern_jump = json_integer_value(obj);
	obj = json_object_get(root, "editor_octave");
	if (obj)
		g_editor->pattern_octave = json_integer_value(obj);
}

static void load_track(json_t *root) {
	json_t			*json_patterns;
	json_t			*json_pattern;
	json_t			*json_cols;
	json_t			*json_col;
	json_t			*json_lines;
	json_t			*json_line;
	json_t			*json_fxs;
	json_t			*json_fx;
	json_t			*json_synths;
	json_t			*json_synth;
	json_t			*json_instances;
	json_t			*json_instance;
	PatternSource	*pattern;
	PatternNoteCol	*note_col;
	PatternNote		*note;
	PatternCVCol	*cv_col;
	PatternCV		*cv;
	Synth			*synth;
	PatternInstance	*instance;
	int				pattern_source;
	int				line;
	size_t			i, j, k, l;

	g_timeline->clear();

	/// [1] LOAD PATTERNS
	json_patterns = json_object_get(root, "patterns");
	json_array_foreach(json_patterns, i, json_pattern) {

		pattern = g_timeline->pattern_new(
		/**/ json_integer_value(					// Pattern note count
		/**/ json_object_get(json_pattern, "note_count")),
		/**/ json_integer_value(					// Pattern cv count
		/**/ json_object_get(json_pattern, "cv_count")),
		/**/ json_integer_value(					// Pattern beat count
		/**/ json_object_get(json_pattern, "beat_count")),
		/**/ json_integer_value(					// Pattern lpb
		/**/ json_object_get(json_pattern, "lpb")));
		pattern->color = json_integer_value(		// Pattern color
		/**/ json_object_get(json_pattern, "color"));
		pattern->rename((char*)json_string_value(	// Pattern name
		/**/ json_object_get(json_pattern, "name")));

		json_cols = json_object_get(json_pattern, "notes");
		json_array_foreach(json_cols, j, json_col) {

			note_col = &(pattern->notes[j]);

			// TODO - Muted
			// TODO - Fx velo
			// TODO - Fx pan
			// TODO - Fx delay
			// TODO - Fx chance
			// TODO - Fx chance mode
			// TODO - Fx octave
			// TODO - Fx octave mode
			// TODO - Fx pitch
			note_col->fx_count = json_integer_value(
			/**/ json_object_get(json_col, "effect_count"));

			json_lines = json_object_get(json_col, "lines");
			json_array_foreach(json_lines, k, json_line) {

				line = json_integer_value(					// Note line
				/**/ json_object_get(json_line, "line"));
				if (line < 0 || line >= pattern->line_count)
					continue;
				note = &(note_col->lines[line]);

				note->mode = json_integer_value(			// Note mode
				/**/ json_object_get(json_line, "mode"));
				if (note->mode == PATTERN_NOTE_NEW) {

					note->pitch = json_integer_value(		// Note pitch
					/**/ json_object_get(json_line, "pitch"));
					note->velocity = json_integer_value(	// Note velocity
					/**/ json_object_get(json_line, "velocity"));
					note->panning = json_integer_value(		// Note panning
					/**/ json_object_get(json_line, "panning"));
					note->synth = json_integer_value(		// Note synth
					/**/ json_object_get(json_line, "synth"));
					note->delay = json_integer_value(		// Note delay
					/**/ json_object_get(json_line, "delay"));
					note->glide = json_integer_value(		// Note glide
					/**/ json_object_get(json_line, "glide"));

					json_fxs = json_object_get(json_line, "effects");
					json_array_foreach(json_fxs, l, json_fx) {
						note->effects[l].type = json_integer_value(
						/**/ json_object_get(json_fx, "type"));
						note->effects[l].value = json_integer_value(
						/**/ json_object_get(json_fx, "value"));
					}

				} else if (note->mode == PATTERN_NOTE_STOP) {

					note->delay = json_integer_value(		// Note delay
					/**/ json_object_get(json_line, "delay"));

				} else if (note->mode == PATTERN_NOTE_GLIDE) {

					note->pitch = json_integer_value(		// Note pitch
					/**/ json_object_get(json_line, "pitch"));
					note->velocity = json_integer_value(	// Note velocity
					/**/ json_object_get(json_line, "velocity"));
					note->panning = json_integer_value(		// Note panning
					/**/ json_object_get(json_line, "panning"));
					note->glide = json_integer_value(		// Note glide
					/**/ json_object_get(json_line, "glide"));

					json_fxs = json_object_get(json_line, "effects");
					json_array_foreach(json_fxs, l, json_fx) {
						note->effects[l].type = json_integer_value(
						/**/ json_object_get(json_fx, "type"));
						note->effects[l].value = json_integer_value(
						/**/ json_object_get(json_fx, "value"));
					}

				}
			}

		}

		json_cols = json_object_get(json_pattern, "cvs");
		json_array_foreach(json_cols, j, json_col) {

			cv_col = &(pattern->cvs[j]);

			cv_col->mode = json_integer_value(			// Column mode (cv / bpm)
			/**/ json_object_get(json_col, "mode"));
			cv_col->synth = json_integer_value(			// Column synth
			/**/ json_object_get(json_col, "synth"));
			cv_col->channel = json_integer_value(		// Column channel
			/**/ json_object_get(json_col, "channel"));

			json_lines = json_object_get(json_col, "lines");
			json_array_foreach(json_lines, k, json_line) {

				line = json_integer_value(				// CV line
				/**/ json_object_get(json_line, "line"));
				if (line < 0 || line >= pattern->line_count)
					continue;
				cv = &(cv_col->lines[line]);

				cv->mode = json_integer_value(			// CV mode
				/**/ json_object_get(json_line, "mode"));
				cv->value = json_integer_value(			// CV value
				/**/ json_object_get(json_line, "value"));
				cv->delay = json_integer_value(			// CV delay
				/**/ json_object_get(json_line, "delay"));
				cv->curve = json_integer_value(			// CV curve
				/**/ json_object_get(json_line, "curve"));

			}
		}
	}

	/// [2] LOAD SYNTHS
	json_synths = json_object_get(root, "synths");
	json_array_foreach(json_synths, i, json_synth) {
		synth = g_timeline->synth_new();
		synth->color = json_integer_value(			// Synth color
		/**/ json_object_get(json_synth, "color"));
		synth->mode = json_integer_value(			// Synth mode (gate / trigger / drum)
		/**/ json_object_get(json_synth, "mode"));
		synth->channel_count = json_integer_value(	// Synth polyphony
		/**/ json_object_get(json_synth, "polyphony"));
		synth->rename((char*)json_string_value(		// Synth name
		/**/ json_object_get(json_synth, "name")));
	}

	/// [3] LOAD TIMELINE
	json_instances = json_object_get(root, "instances");
	json_array_foreach(json_instances, i, json_instance) {
		pattern_source = json_integer_value(		// Instance pattern source
		/**/ json_object_get(json_instance, "pattern"));
		if (pattern_source < 0 || pattern_source >= g_timeline->pattern_count)
			continue;
		pattern = &(g_timeline->patterns[pattern_source]);

		instance = g_timeline->instance_new(
		/**/pattern,
		/**/ json_integer_value(						// Instance row
		/**/ json_object_get(json_instance, "row")),
		/**/ json_integer_value(						// Instance beat
		/**/ json_object_get(json_instance, "beat")));
		instance->beat_start = json_integer_value(		// Instance beat start
		/**/ json_object_get(json_instance, "start"));
		instance->beat_length = json_integer_value(		// Instance beat length
		/**/ json_object_get(json_instance, "length"));
		instance->muted = json_integer_value(			// Instance muted
		/**/ json_object_get(json_instance, "muted"));
	}

	/// [4] LOAD MATRIX
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void Tracker::dataFromJson(json_t *root) {
	if (g_module != this)
		return;

	/// [1] RECOVER MIDI (DRIVER + DEVICE + CHANNEL)
	load_midi(root);

	/// [2] WAIT FOR THREAD FLAG
	while (g_timeline->thread_flag.test_and_set()) {}

	/// [3] LOAD TRACK
	load_track(root);

	/// [4] CLEAR THREAD FLAG
	g_timeline->thread_flag.clear();
}
