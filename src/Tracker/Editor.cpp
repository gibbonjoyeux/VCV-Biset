
#include "Tracker.hpp"

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

	// TODO: Check pattern cannot be used when no pattern
	/// TODO: check synth cannot be used when no synth
	this->pattern_id = -1;
	this->pattern = NULL;
	this->synth_id = -1;
	this->synth = NULL;
	this->instance = NULL;

	this->pattern_track = 0;
	this->pattern_col = 0;
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

	this->timeline_column = 0;
	this->timeline_line = 0;
	this->timeline_cell = 0;
	this->timeline_char = 0;
	this->timeline_cam_x = 0;
	this->timeline_cam_y = 0;

	this->side_synth_cam_y = 0;
	this->side_pattern_cam_y = 0;
}

void Editor::process(i64 frame) {
	Module			*module;

	if (frame % 256 != 0)
		return;

	module = g_module;
	// TODO: check change in g_editor->pattern_col
	// -> On change update edit knobs (Note or CV)

	/// [1] HANDLE VIEW SWITCHES
	pattern_view_velo = module->params[Tracker::PARAM_VIEW + 0].getValue();
	pattern_view_pan = module->params[Tracker::PARAM_VIEW + 1].getValue();
	pattern_view_glide = module->params[Tracker::PARAM_VIEW + 2].getValue();
	pattern_view_delay = module->params[Tracker::PARAM_VIEW + 3].getValue();
	pattern_view_fx = module->params[Tracker::PARAM_VIEW + 4].getValue();

	/// [2] HANDLE PLAY LIGHT
	if (g_editor->selected == true)
		module->lights[Tracker::LIGHT_FOCUS].setBrightness(1.0);
	else
		module->lights[Tracker::LIGHT_FOCUS].setBrightness(0.0);
	if (g_timeline->play != TIMELINE_MODE_STOP)
		module->lights[Tracker::LIGHT_PLAY].setBrightness(1.0);
	else
		module->lights[Tracker::LIGHT_PLAY].setBrightness(0.0);
	/// [2] HANDLE EDITOR MODES
	if (module->params[Tracker::PARAM_MODE_PATTERN].getValue())
		this->mode = EDITOR_MODE_PATTERN;
	else if (module->params[Tracker::PARAM_MODE_TIMELINE].getValue())
		this->mode = EDITOR_MODE_TIMELINE;
	else if (module->params[Tracker::PARAM_MODE_MATRIX].getValue())
		this->mode = EDITOR_MODE_MATRIX;
	else
		this->mode = EDITOR_MODE_TUNING;

	/// [3] HANDLE PLAYING BUTTONS
	/// HANDLE PLAY
	//// PLAY SONG
	if (this->button_play[0]
	.process(module->params[Tracker::PARAM_PLAY_SONG].getValue())) {
		g_timeline->stop();
		g_timeline->clock.reset();
		g_timeline->play = TIMELINE_MODE_PLAY_SONG;
	}
	//// PLAY PATTERN
	if (this->button_play[1]
	.process(module->params[Tracker::PARAM_PLAY_PATTERN].getValue())) {
		g_timeline->stop();
		g_timeline->clock.reset();
		g_timeline->play = TIMELINE_MODE_PLAY_PATTERN;
	}
	//// PLAY
	if (this->button_play[2]
	.process(module->params[Tracker::PARAM_PLAY].getValue())) {
		if (g_timeline->play == TIMELINE_MODE_PLAY_PATTERN) {
			g_timeline->stop();
			g_timeline->clock.reset();
		} else if (g_timeline->play == TIMELINE_MODE_STOP) {
			g_timeline->stop();
		}
		g_timeline->play = TIMELINE_MODE_PLAY_SONG;
	}
	//// STOP
	if (this->button_play[3]
	.process(module->params[Tracker::PARAM_STOP].getValue())) {
		g_timeline->stop();
		g_timeline->play = TIMELINE_MODE_STOP;
	}
	/// HANDLE OCTAVE BUTTONS
	if (this->button_octave[0]
	.process(module->params[Tracker::PARAM_OCTAVE_UP].getValue()))
		if (this->pattern_octave < 9)
			this->pattern_octave += 1;
	if (this->button_octave[1]
	.process(module->params[Tracker::PARAM_OCTAVE_DOWN].getValue()))
		if (this->pattern_octave > 0)
			this->pattern_octave -= 1;
	/// HANDLE JUMP BUTTONS
	if (this->button_jump[0]
	.process(module->params[Tracker::PARAM_JUMP_UP].getValue()))
		if (this->pattern_jump < 16)
			this->pattern_jump += 1;
	if (this->button_jump[1]
	.process(module->params[Tracker::PARAM_JUMP_DOWN].getValue()))
		if (this->pattern_jump > 0)
			this->pattern_jump -= 1;
}

void Editor::set_synth(int index) {
	if (index >= g_timeline->synth_count)
		return;
	g_editor->synth_id = index;
	g_editor->synth = &(g_timeline->synths[index]);
}

void Editor::set_pattern(int index) {
	if (index >= g_timeline->pattern_count)
		return;
	this->pattern_id = index;
	this->pattern = &(g_timeline->patterns[index]);
	//this->pattern_reset_cursor();
}

void Editor::pattern_move_cursor_x(int delta_x) {
	PatternNoteCol	*note_col;
	int				x;
	int				i;

	/// [1] MOVE CURSOR
	this->pattern_cell += delta_x;
	this->pattern_char = 0;
	/// [2] HANDLE ON/OFF VIEW MODES
	if (this->pattern_col < pattern->note_count) {
		note_col = pattern->notes[this->pattern_col];
		/// TO RIGHT
		if (delta_x > 0) {
			//// VELOCITY
			if (this->pattern_cell == 2 && !g_editor->pattern_view_velo)
				this->pattern_cell += 1;
			//// PANNING
			if (this->pattern_cell == 3 && !g_editor->pattern_view_pan)
				this->pattern_cell += 1;
			//// DELAY
			if (this->pattern_cell == 5 && !g_editor->pattern_view_glide)
				this->pattern_cell += 1;
			//// GLIDE
			if (this->pattern_cell == 6 && !g_editor->pattern_view_delay)
				this->pattern_cell += 1;
			//// EFFECT
			if (this->pattern_cell > 6 && !g_editor->pattern_view_fx)
				this->pattern_cell = 7 + 2 * note_col ->effect_count;
		/// TO LEFT
		} else if (delta_x < 0) {
			//// EFFECT
			if (this->pattern_cell > 6 && !g_editor->pattern_view_fx)
				this->pattern_cell = 6;
			//// GLIDE
			if (this->pattern_cell == 6 && !g_editor->pattern_view_delay)
				this->pattern_cell -= 1;
			//// DELAY
			if (this->pattern_cell == 5 && !g_editor->pattern_view_glide)
				this->pattern_cell -= 1;
			//// PANNING
			if (this->pattern_cell == 3 && !g_editor->pattern_view_pan)
				this->pattern_cell -= 1;
			//// VELOCITY
			if (this->pattern_cell == 2 && !g_editor->pattern_view_velo)
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
			note_col = this->pattern->notes[i];
			if (i == this->pattern_col) {
				if (this->pattern_cell > 0)
					x += 2;
				if (this->pattern_cell > 1)
					x += 1;
				if (this->pattern_cell > 2 && this->pattern_view_velo)
					x += 2;
				if (this->pattern_cell > 3 && this->pattern_view_pan)
					x += 2;
				if (this->pattern_cell > 4)
					x += 2;
				if (this->pattern_cell > 5 && this->pattern_view_glide)
					x += 2;
				if (this->pattern_cell > 6 && this->pattern_view_delay)
					x += 2;
				if (this->pattern_cell > 7 && this->pattern_view_fx) {
					x += 3 * ((this->pattern_cell - 7) / 2);
					x += (this->pattern_cell - 7) % 2;
				}
				break;
			}
			x += (2 + 1
			/**/ + g_editor->pattern_view_velo * 2
			/**/ + g_editor->pattern_view_pan * 2
			/**/ + 2
			/**/ + g_editor->pattern_view_glide * 2
			/**/ + g_editor->pattern_view_delay * 2
			/**/ + g_editor->pattern_view_fx * 3 * note_col->effect_count
			/**/ + 1);
		/// ON CV
		} else {
			if (i == this->pattern_col) {
				if (this->pattern_cell == 0)
					;
				else if (this->pattern_cell == 1)
					x += 2;
				else if (this->pattern_cell == 2)
					x += 4;
				break;
			}
			x += (3 + 2 + 2 + 1);
		}
		i += 1;
	}
	//// OFFSET CAMERA
	if (this->pattern_cam_x < x - (CHAR_COUNT_X - 3))
		this->pattern_cam_x = x - (CHAR_COUNT_X - 3);
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
	PatternNoteCol	*col_note;

	pattern = g_editor->pattern;
	if (pattern == NULL)
		return;

	/// HANDLE LINE UNDERFLOW
	if (this->pattern_line < 0)
		this->pattern_line = 0;
	/// HANDLE LINE OVERFLOW
	if (this->pattern_line >= pattern->line_count)
		this->pattern_line = pattern->line_count - 1;
	/// HANDLE COLUMN OVERFLOW
	if (this->pattern_col >= pattern->note_count + pattern->cv_count) {
		this->pattern_col = pattern->note_count + pattern->cv_count - 1;
		/// NOTE COL
		if (this->pattern_col < pattern->note_count) {
			col_note = pattern->notes[this->pattern_col];
			this->pattern_cell = 7 + 2 * col_note->effect_count - 1;
		/// CV COL
		} else { 
			this->pattern_cell = 2;
		}
		this->pattern_char = 0;
	}
	/// HANDLE CELL UNDERFLOW
	if (this->pattern_cell < 0) {
		this->pattern_col -= 1;
		/// HANDLE COL UNDERFLOW
		if (this->pattern_col < 0) {
			this->pattern_col = 0;
			this->pattern_cell = 0;
		/// HANDLE COL OFFSET
		} else {
			/// FALL ON NOTE COL
			if (this->pattern_col < pattern->note_count) {
				col_note = pattern->notes[this->pattern_col];
				this->pattern_cell = 7 + 2 * col_note->effect_count - 1;
				/// CHECK ON/OFF VIEW MODES
				//// EFFECT
				if (g_editor->pattern_view_fx == false)
					this->pattern_cell = 6;
				//// GLIDE
				if (this->pattern_cell == 6
				&& g_editor->pattern_view_delay == false)
					this->pattern_cell -= 1;
				//// DELAY
				if (this->pattern_cell == 5
				&& g_editor->pattern_view_glide == false)
					this->pattern_cell -= 1;
			/// FALL ON CV COL
			} else {
				this->pattern_cell = 2;
			}
		}
	}
	/// HANDLE CELL OVERFLOW
	/// ON NOTE COL
	if (this->pattern_col < pattern->note_count) {
		col_note = pattern->notes[this->pattern_col];
		/// HANDLE COL NOTE OVERFLOW
		if (this->pattern_cell >= 7 + 2 * col_note->effect_count) {
			/// FROM NOTE TO CV
			if (this->pattern_col == pattern->note_count - 1) {
				/// GOT NO CV
				if (pattern->cv_count == 0) {
					this->pattern_cell = 7 + 2 * col_note->effect_count - 1;
					/// CHECK ON/OFF VIEW MODES
					//// EFFECT
					if (g_editor->pattern_view_fx == false)
						this->pattern_cell = 6;
					//// GLIDE
					if (this->pattern_cell == 6
					&& g_editor->pattern_view_delay == false)
						this->pattern_cell -= 1;
					//// DELAY
					if (this->pattern_cell == 5
					&& g_editor->pattern_view_glide == false)
						this->pattern_cell -= 1;
				/// GOT CV
				} else {
					this->pattern_col += 1;
					this->pattern_cell = 0;
				}
			/// FROM NOTE TO NOTE
			} else {
				this->pattern_col += 1;
				this->pattern_cell = 0;
			}
		}
	/// ON CV COL
	} else {
		/// HANDLE COL CV OVERFLOW
		if (this->pattern_cell > 2) {
			/// GOT NO NEXT
			if (this->pattern_col >=
			pattern->note_count + pattern->cv_count - 1) {
				this->pattern_cell = 2;
			/// GOT NEXT
			} else {
				this->pattern_col += 1;
				this->pattern_cell = 0;
			}
		}
	}
}

void Editor::pattern_reset_cursor(void) {
	this->pattern_col = 0;
	this->pattern_line = 0;
	this->pattern_cell = 0;
	this->pattern_char = 0;
	this->pattern_cam_x = 0;
	this->pattern_cam_y = 0;
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
