
#include "plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
/// EDITOR STRUCT
//////////////////////////////////////////////////

Editor::Editor() {
	this->mode = EDITOR_MODE_PATTERN;

	this->selected = false;
	this->pattern_id = 0;
	this->pattern = &(g_timeline.patterns[0]);
	this->pattern_track = 0;
	this->pattern_row = 0;
	this->pattern_row_prev = -1;
	this->pattern_line = 0;
	this->pattern_cell = 0;
	this->pattern_char = 0;
	this->pattern_cam_x = 0;
	this->pattern_cam_y = 0;

	this->pattern_view_velo = true;
	this->pattern_view_pan = true;
	this->pattern_view_glide = true;
	this->pattern_view_delay = true;
	this->pattern_view_fx = true;

	this->pattern_octave = 4;
	this->pattern_jump = 1;

	this->pattern_debug[0] = 0;

	this->synth_id = 0;
}

void Editor::process(void) {
	Module			*module;
	int				value;
	int				i;

	module = g_editor.module;
	// TODO: check change in g_editor.pattern_row
	// -> On change update edit knobs (Note or CV)

	/// [1] HANDLE VIEW SWITCHES
	for (i = 0; i < 5; ++i) {
		if (this->switch_view[i].process(module->params[Tracker::PARAM_VIEW + i].getValue()))
			module->lights[Tracker::LIGHT_VIEW + i].setBrightness(1.0);
		else
			module->lights[Tracker::LIGHT_VIEW + i].setBrightness(0.0);
	}

	/// [2] HANDLE EDITOR MODES
	if (this->button_mode[0].process(module->params[Tracker::PARAM_MODE + 0].getValue()))
		this->mode = EDITOR_MODE_PATTERN;
	if (this->button_mode[1].process(module->params[Tracker::PARAM_MODE + 1].getValue()))
		this->mode = EDITOR_MODE_TIMELINE;
	if (this->button_mode[2].process(module->params[Tracker::PARAM_MODE + 2].getValue()))
		this->mode = EDITOR_MODE_PARAMETERS;
	/// HANDLE EDITOR MODES LIGHTS
	if (this->mode == EDITOR_MODE_PATTERN) {
		module->lights[Tracker::LIGHT_MODE].setBrightness(1.0);
		module->lights[Tracker::LIGHT_MODE + 1].setBrightness(0.0);
		module->lights[Tracker::LIGHT_MODE + 2].setBrightness(0.0);
	} else if (this->mode == EDITOR_MODE_TIMELINE) {
		module->lights[Tracker::LIGHT_MODE].setBrightness(0.0);
		module->lights[Tracker::LIGHT_MODE + 1].setBrightness(1.0);
		module->lights[Tracker::LIGHT_MODE + 2].setBrightness(0.0);
	} else {
		module->lights[Tracker::LIGHT_MODE].setBrightness(0.0);
		module->lights[Tracker::LIGHT_MODE + 1].setBrightness(0.0);
		module->lights[Tracker::LIGHT_MODE + 2].setBrightness(1.0);
	}

	/// [3] HANDLE PLAYING BUTTONS
	/// HANDLE OCTAVE BUTTONS
	if (this->button_octave[0].process(module->params[Tracker::PARAM_OCTAVE_UP].getValue()))
		if (this->pattern_octave < 9)
			this->pattern_octave += 1;
	if (this->button_octave[1].process(module->params[Tracker::PARAM_OCTAVE_DOWN].getValue()))
		if (this->pattern_octave > 0)
			this->pattern_octave -= 1;
	/// HANDLE JUMP BUTTONS
	if (this->button_jump[0].process(module->params[Tracker::PARAM_JUMP_UP].getValue()))
		if (this->pattern_jump < 16)
			this->pattern_jump += 1;
	if (this->button_jump[1].process(module->params[Tracker::PARAM_JUMP_DOWN].getValue()))
		if (this->pattern_jump > 0)
			this->pattern_jump -= 1;

	/// [4] HANDLE INFO SCREEN
	/// HANDLE PATTERN SELECTION
	value = module->params[Tracker::PARAM_PATTERN].getValue();
	if (value != this->pattern_id)
		if (value >= 0 && value < 256)
			this->set_pattern(value, false);
	/// HANDLE SYNTH SELECTION
	value = module->params[Tracker::PARAM_SYNTH].getValue();
	if (value != this->synth_id)
		if (value >= 0 && value < 64)
			this->set_synth(value, false);

	/// [5] HANDLE EDIT SCREEN
	if (g_editor.pattern_row != g_editor.pattern_row_prev)
		this->set_row(g_editor.pattern_row);
	/// HANDLE EDIT SAVE BUTTON
	if (this->button_save.process(module->params[Tracker::PARAM_EDIT_SAVE].getValue()))
		this->save_edition();
}

void Editor::save_edition(void) {
	PatternNoteRow	*col_note;
	PatternCVRow	*col_cv;
	int				beat_count;
	int				note_count, cv_count;
	int				lpb;
	int				channels;
	int				note_mode, note_effect;
	int				cv_mode, cv_synth, cv_channel;

	/// SONG LENGTH
	beat_count = g_editor.module->params[Tracker::PARAM_EDIT + 0].getValue();
	if (g_timeline.beat_count != beat_count) {
		g_timeline.resize(beat_count);
		this->set_song_length(beat_count, true);
	}
	/// SYNTH CHANNELS
	channels = g_editor.module->params[Tracker::PARAM_EDIT + 1].getValue();
	if (g_timeline.synths[g_editor.synth_id].channel_count != channels) {
		g_timeline.synths[g_editor.synth_id].channel_count = channels;
	}
	/// PATTERN
	beat_count = g_editor.module->params[Tracker::PARAM_EDIT + 2].getValue();
	lpb = g_editor.module->params[Tracker::PARAM_EDIT + 3].getValue();
	note_count = g_editor.module->params[Tracker::PARAM_EDIT + 4].getValue();
	cv_count = g_editor.module->params[Tracker::PARAM_EDIT + 5].getValue();
	if (beat_count != g_editor.pattern->beat_count
	|| lpb != g_editor.pattern->lpb
	|| note_count != g_editor.pattern->note_count
	|| cv_count != g_editor.pattern->cv_count) {
		g_editor.pattern->resize(note_count, cv_count, beat_count, lpb);
	}
	/// ROW
	//// AS NOTE
	if (g_editor.pattern_row < g_editor.pattern->note_count) {
		col_note = g_editor.pattern->notes[g_editor.pattern_row];
		note_mode = g_editor.module->params[Tracker::PARAM_EDIT + 6].getValue();
		note_effect = g_editor.module->params[Tracker::PARAM_EDIT + 7].getValue();
		if (note_mode != col_note->mode)
			col_note->mode = note_mode;
		if (note_effect != col_note->effect_count)
			col_note->effect_count = note_effect;
	//// AS CV
	} else {
		col_cv = g_editor.pattern->cvs[g_editor.pattern_row - g_editor.pattern->note_count];
		cv_mode = g_editor.module->params[Tracker::PARAM_EDIT + 6].getValue();
		cv_synth = g_editor.module->params[Tracker::PARAM_EDIT + 7].getValue();
		cv_channel = g_editor.module->params[Tracker::PARAM_EDIT + 8].getValue();
		if (cv_mode != col_cv->mode)
			col_cv->mode = cv_mode;
		if (cv_synth != col_cv->synth)
			col_cv->synth = cv_synth;
		if (cv_channel != col_cv->channel)
			col_cv->channel = cv_channel;
	}
}

void Editor::set_row(int index) {
	ParamQuantity	*param;
	PatternNoteRow	*row_note;
	PatternCVRow	*row_cv;

	g_editor.pattern_row_prev = g_editor.pattern_row;
	/// ROW NOTE
	if (g_editor.pattern_row < g_editor.pattern->note_count) {
		row_note = g_editor.pattern->notes[g_editor.pattern_row];
		/// NOTE MODE
		param = g_editor.module->getParamQuantity(Tracker::PARAM_EDIT + 6);
		param->defaultValue = row_note->mode;
		param->minValue = 0;
		param->maxValue = 1;
		g_editor.module->params[Tracker::PARAM_EDIT + 6].setValue(row_note->mode);
		/// NOTE EFFECTS
		param = g_editor.module->getParamQuantity(Tracker::PARAM_EDIT + 7);
		param->defaultValue = row_note->effect_count;
		param->minValue = 0;
		param->maxValue = 8;
		g_editor.module->params[Tracker::PARAM_EDIT + 7].setValue(row_note->effect_count);
	/// ROW CV
	} else {
		row_cv = g_editor.pattern->cvs[g_editor.pattern_row - g_editor.pattern->note_count];
		/// CV MODE
		param = g_editor.module->getParamQuantity(Tracker::PARAM_EDIT + 6);
		param->defaultValue = row_cv->mode;
		param->minValue = 0;
		param->maxValue = 1;
		g_editor.module->params[Tracker::PARAM_EDIT + 6].setValue(row_cv->mode);
		/// CV SYNTH
		param = g_editor.module->getParamQuantity(Tracker::PARAM_EDIT + 7);
		param->defaultValue = row_cv->synth;
		param->minValue = 0;
		param->maxValue = 63;
		g_editor.module->params[Tracker::PARAM_EDIT + 7].setValue(row_cv->synth);
		/// CV CHANNEL
		param = g_editor.module->getParamQuantity(Tracker::PARAM_EDIT + 8);
		param->defaultValue = row_cv->channel;
		param->minValue = 0;
		param->maxValue = 7;
		g_editor.module->params[Tracker::PARAM_EDIT + 8].setValue(row_cv->channel);
	}
}

void Editor::set_song_length(int length, bool mode) {
	/// UPDATE SONG LENGTH DEFAULT VALUE
	g_editor.module->getParamQuantity(Tracker::PARAM_EDIT)->defaultValue = length;
	g_editor.module->params[Tracker::PARAM_EDIT].setValue(length);
	/// UPDATE SONG LENGTH REAL VALUE
	if (mode)
		g_editor.module->params[Tracker::PARAM_EDIT].setValue(length);
}

void Editor::set_synth(int index, bool mode) {
	int		value;

	/// [1] UPDATE EDITOR
	/// UPDATE SYNTH
	this->synth_id = index;
	/// UPDATE SYNTH PARAM
	value = g_timeline.synths[index].channel_count;
	g_editor.module->getParamQuantity(Tracker::PARAM_EDIT + 1)->defaultValue = value;
	g_editor.module->params[Tracker::PARAM_EDIT + 1].setValue(value);
	/// [2] UPDATE KNOB
	/// UPDATE SYNTH KNOB
	if (mode)
		g_editor.module->params[Tracker::PARAM_SYNTH].setValue(index);
}

void Editor::set_pattern(int index, bool mode) {
	int		value;

	/// [1] UPDATE EDITOR
	/// UPDATE PATTERN
	this->pattern_id = index;
	this->pattern = &(g_timeline.patterns[index]);
	/// UPDATE PATTERN PARAMS
	//// PATTERN LENGTH
	value = this->pattern->beat_count;
	g_editor.module->getParamQuantity(Tracker::PARAM_EDIT + 2)->defaultValue = value;
	g_editor.module->params[Tracker::PARAM_EDIT + 2].setValue(value);
	//// PATTERN LPB
	value = this->pattern->lpb;
	g_editor.module->getParamQuantity(Tracker::PARAM_EDIT + 3)->defaultValue = value;
	g_editor.module->params[Tracker::PARAM_EDIT + 3].setValue(value);
	//// PATTERN NOTE COUNT
	value = this->pattern->note_count;
	g_editor.module->getParamQuantity(Tracker::PARAM_EDIT + 4)->defaultValue = value;
	g_editor.module->params[Tracker::PARAM_EDIT + 4].setValue(value);
	//// PATTERN CV COUNT
	value = this->pattern->cv_count;
	g_editor.module->getParamQuantity(Tracker::PARAM_EDIT + 5)->defaultValue = value;
	g_editor.module->params[Tracker::PARAM_EDIT + 5].setValue(value);
	/// [2] UPDATE KNOB
	/// UPDATE PATTERN KNOB
	if (mode)
		g_editor.module->params[Tracker::PARAM_PATTERN].setValue(index);
}

void Editor::pattern_move_cursor_x(int delta_x) {
	PatternNoteRow	*note_row;
	int				x;
	int				i;

	/// [1] MOVE CURSOR
	this->pattern_cell += delta_x;
	this->pattern_char = 0;
	/// [2] HANDLE ON/OFF VIEW MODES
	if (this->pattern_row < pattern->note_count) {
		note_row = pattern->notes[this->pattern_row];
		/// TO RIGHT
		if (delta_x > 0) {
			//// VELOCITY
			if (this->pattern_cell == 2 && !g_editor.switch_view[0].state)
				this->pattern_cell += 1;
			//// PANNING
			if (this->pattern_cell == 3 && !g_editor.switch_view[1].state)
				this->pattern_cell += 1;
			//// DELAY
			if (this->pattern_cell == 5 && !g_editor.switch_view[2].state)
				this->pattern_cell += 1;
			//// GLIDE
			if (this->pattern_cell == 6 && !g_editor.switch_view[3].state)
				this->pattern_cell += 1;
			//// EFFECT
			if (this->pattern_cell > 6 && !g_editor.switch_view[4].state)
				this->pattern_cell = 7 + 2 * note_row ->effect_count;
		/// TO LEFT
		} else if (delta_x < 0) {
			//// EFFECT
			if (this->pattern_cell > 6 && !g_editor.switch_view[4].state)
				this->pattern_cell = 6;
			//// GLIDE
			if (this->pattern_cell == 6 && !g_editor.switch_view[3].state)
				this->pattern_cell -= 1;
			//// DELAY
			if (this->pattern_cell == 5 && !g_editor.switch_view[2].state)
				this->pattern_cell -= 1;
			//// PANNING
			if (this->pattern_cell == 3 && !g_editor.switch_view[1].state)
				this->pattern_cell -= 1;
			//// VELOCITY
			if (this->pattern_cell == 2 && !g_editor.switch_view[0].state)
				this->pattern_cell -= 1;
		}
	}
	/// [3] HANDLE CLAMPING
	this->pattern_clamp_cursor();
	/// [4] HANDLE CAMERA
	//// COMPUTE X POSITION
	x = 0;
	i = 0;
	while (i < this->pattern->note_count + this->pattern->cv_count) {
		/// ON NOTE
		if (i < this->pattern->note_count) {
			note_row = this->pattern->notes[i];
			if (i == this->pattern_row) {
				if (this->pattern_cell > 0)
					x += 2;
				if (this->pattern_cell > 1)
					x += 1;
				if (this->pattern_cell > 2 && this->switch_view[0].state)
					x += 2;
				if (this->pattern_cell > 3 && this->switch_view[1].state)
					x += 2;
				if (this->pattern_cell > 4)
					x += 2;
				if (this->pattern_cell > 5 && this->switch_view[2].state)
					x += 2;
				if (this->pattern_cell > 6 && this->switch_view[3].state)
					x += 2;
				if (this->pattern_cell > 7 && this->switch_view[4].state) {
					x += 3 * ((this->pattern_cell - 7) / 2);
					x += (this->pattern_cell - 7) % 2;
				}
				break;
			}
			x += (2 + 1
			/**/ + g_editor.switch_view[0].state * 2
			/**/ + g_editor.switch_view[1].state * 2
			/**/ + 2
			/**/ + g_editor.switch_view[2].state * 2
			/**/ + g_editor.switch_view[3].state * 2
			/**/ + g_editor.switch_view[4].state * 3 * note_row->effect_count
			/**/ + 1);
		/// ON CV
		} else {
			if (i == this->pattern_row) {
				if (this->pattern_cell == 0)
					;
				else if (this->pattern_cell == 1)
					x += 2;
				else if (this->pattern_cell == 2)
					x += 4;
				break;
			}
			x += (2 + 2 + 2 + 1);
		}
		i += 1;
	}
	//// OFFSET CAMERA
	if (this->pattern_cam_x < x - (CHAR_COUNT_X - 2))
		this->pattern_cam_x = x - (CHAR_COUNT_X - 2);
	if (this->pattern_cam_x > x)
		this->pattern_cam_x = x;
}

void Editor::pattern_move_cursor_y(int delta_y) {
	/// [1] MOVE CURSOR
	this->pattern_line += delta_y;
	this->pattern_char = 0;
	/// [2] HANDLE CLAMPING
	this->pattern_clamp_cursor();
	/// [3] HANDLE CAMERA
	if (this->pattern_cam_y < this->pattern_line - (CHAR_COUNT_Y - 1))
		this->pattern_cam_y = this->pattern_line - (CHAR_COUNT_Y - 1);
	else if (this->pattern_cam_y > this->pattern_line)
		this->pattern_cam_y = this->pattern_line;
}

void Editor::pattern_clamp_cursor(void) {
	PatternSource	*pattern;
	PatternNoteRow	*row_note;

	pattern = g_editor.pattern;
	if (pattern == NULL)
		return;

	/// HANDLE LINE UNDERFLOW
	if (this->pattern_line < 0)
		this->pattern_line = 0;
	/// HANDLE LINE OVERFLOW
	if (this->pattern_line >= pattern->line_count)
		this->pattern_line = pattern->line_count - 1;
	/// HANDLE CELL UNDERFLOW
	if (this->pattern_cell < 0) {
		this->pattern_row -= 1;
		/// HANDLE ROW UNDERFLOW
		if (this->pattern_row < 0) {
			this->pattern_row = 0;
			this->pattern_cell = 0;
		/// HANDLE ROW OFFSET
		} else {
			/// FALL ON NOTE ROW
			if (this->pattern_row < pattern->note_count) {
				row_note = pattern->notes[this->pattern_row];
				this->pattern_cell = 7 + 2 * row_note->effect_count - 1;
				/// CHECK ON/OFF VIEW MODES
				//// EFFECT
				if (g_editor.switch_view[4].state == false)
					this->pattern_cell = 6;
				//// GLIDE
				if (this->pattern_cell == 6
				&& g_editor.switch_view[3].state == false)
					this->pattern_cell -= 1;
				//// DELAY
				if (this->pattern_cell == 5
				&& g_editor.switch_view[2].state == false)
					this->pattern_cell -= 1;
			/// FALL ON CV ROW
			} else {
				this->pattern_cell = 2;
			}
		}
	}
	/// HANDLE CELL OVERFLOW
	/// ON NOTE ROW
	if (this->pattern_row < pattern->note_count) {
		row_note = pattern->notes[this->pattern_row];
		/// HANDLE ROW NOTE OVERFLOW
		if (this->pattern_cell >= 7 + 2 * row_note->effect_count) {
			/// FROM NOTE TO CV
			if (this->pattern_row == pattern->note_count - 1) {
				/// GOT NO CV
				if (pattern->cv_count == 0) {
					this->pattern_cell = 7 + 2 * row_note->effect_count - 1;
					/// CHECK ON/OFF VIEW MODES
					//// EFFECT
					if (g_editor.switch_view[4].state == false)
						this->pattern_cell = 6;
					//// GLIDE
					if (this->pattern_cell == 6
					&& g_editor.switch_view[3].state == false)
						this->pattern_cell -= 1;
					//// DELAY
					if (this->pattern_cell == 5
					&& g_editor.switch_view[2].state == false)
						this->pattern_cell -= 1;
				/// GOT CV
				} else {
					this->pattern_row += 1;
					this->pattern_cell = 0;
				}
			/// FROM NOTE TO NOTE
			} else {
				this->pattern_row += 1;
				this->pattern_cell = 0;
			}
		}
	/// ON CV ROW
	} else {
		/// HANDLE ROW CV OVERFLOW
		if (this->pattern_cell > 2) {
			/// GOT NO NEXT
			if (this->pattern_row >=
			pattern->note_count + pattern->cv_count - 1) {
				this->pattern_cell = 2;
			/// GOT NEXT
			} else {
				this->pattern_row += 1;
				this->pattern_cell = 0;
			}
		}
	}
}

//////////////////////////////////////////////////
/// EDITORSWITCH STRUCT
//////////////////////////////////////////////////

EditorSwitch::EditorSwitch(void) {
	this->state = true;
	this->previous_input = 0.0;
}

bool EditorSwitch::process(float in) {
	if (in > this->previous_input)
		this->state = !this->state;
	this->previous_input = in;
	return this->state;
}
