
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

	this->pattern_debug[0] = 0;

	this->synth_id = 0;
}

void Editor::process(Module *module) {
	int		value;
	int		i;

	/// HANDLE VIEW SWITCHES
	for (i = 0; i < 5; ++i) {
		if (this->view_switch[i].process(module->params[Tracker::PARAM_VIEW + i].getValue()))
			module->lights[Tracker::LIGHT_VIEW + i].setBrightness(1.0);
		else
			module->lights[Tracker::LIGHT_VIEW + i].setBrightness(0.0);
	}

	/// HANDLE EDITOR MODES
	if (this->mode_button[0].process(module->params[Tracker::PARAM_MODE + 0].getValue()))
		this->mode = EDITOR_MODE_PATTERN;
	if (this->mode_button[1].process(module->params[Tracker::PARAM_MODE + 1].getValue()))
		this->mode = EDITOR_MODE_TIMELINE;
	if (this->mode_button[2].process(module->params[Tracker::PARAM_MODE + 2].getValue()))
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

	/// HANDLE PATTERN SELECTION
	value = module->params[Tracker::PARAM_PATTERN].getValue();
	if (value != this->pattern_id) {
		if (value >= 0 && value < 256) {
			this->pattern_id = value;
			this->pattern = &(g_timeline.patterns[value]);
		}
	}
	/// HANDLE SYNTH SELECTION
	value = module->params[Tracker::PARAM_SYNTH].getValue();
	if (value != this->synth_id) {
		if (value >= 0 && value < 64) {
			this->synth_id = value;
		}
	}
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
			if (this->pattern_cell == 2 && !g_editor.view_switch[0].state)
				this->pattern_cell += 1;
			//// PANNING
			if (this->pattern_cell == 3 && !g_editor.view_switch[1].state)
				this->pattern_cell += 1;
			//// DELAY
			if (this->pattern_cell == 5 && !g_editor.view_switch[2].state)
				this->pattern_cell += 1;
			//// GLIDE
			if (this->pattern_cell == 6 && !g_editor.view_switch[3].state)
				this->pattern_cell += 1;
			//// EFFECT
			if (this->pattern_cell > 6 && !g_editor.view_switch[4].state)
				this->pattern_cell = 7 + 2 * note_row ->effect_count;
		/// TO LEFT
		} else if (delta_x < 0) {
			//// EFFECT
			if (this->pattern_cell > 6 && !g_editor.view_switch[4].state)
				this->pattern_cell = 6;
			//// GLIDE
			if (this->pattern_cell == 6 && !g_editor.view_switch[3].state)
				this->pattern_cell -= 1;
			//// DELAY
			if (this->pattern_cell == 5 && !g_editor.view_switch[2].state)
				this->pattern_cell -= 1;
			//// PANNING
			if (this->pattern_cell == 3 && !g_editor.view_switch[1].state)
				this->pattern_cell -= 1;
			//// VELOCITY
			if (this->pattern_cell == 2 && !g_editor.view_switch[0].state)
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
				if (this->pattern_cell > 2 && this->view_switch[0].state)
					x += 2;
				if (this->pattern_cell > 3 && this->view_switch[1].state)
					x += 2;
				if (this->pattern_cell > 4)
					x += 2;
				if (this->pattern_cell > 5 && this->view_switch[2].state)
					x += 2;
				if (this->pattern_cell > 6 && this->view_switch[3].state)
					x += 2;
				if (this->pattern_cell > 7 && this->view_switch[4].state) {
					x += 3 * ((this->pattern_cell - 7) / 2);
					x += (this->pattern_cell - 7) % 2;
				}
				break;
			}
			x += (2 + 1
			/**/ + g_editor.view_switch[0].state * 2
			/**/ + g_editor.view_switch[1].state * 2
			/**/ + 2
			/**/ + g_editor.view_switch[2].state * 2
			/**/ + g_editor.view_switch[3].state * 2
			/**/ + g_editor.view_switch[4].state * 3 * note_row->effect_count
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
	if (this->pattern_cam_x < x - (CHAR_COUNT_X - 1))
		this->pattern_cam_x = x - (CHAR_COUNT_X - 1);
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
				if (g_editor.view_switch[4].state == false)
					this->pattern_cell = 6;
				//// GLIDE
				if (this->pattern_cell == 6
				&& g_editor.view_switch[3].state == false)
					this->pattern_cell -= 1;
				//// DELAY
				if (this->pattern_cell == 5
				&& g_editor.view_switch[2].state == false)
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
					if (g_editor.view_switch[4].state == false)
						this->pattern_cell = 6;
					//// GLIDE
					if (this->pattern_cell == 6
					&& g_editor.view_switch[3].state == false)
						this->pattern_cell -= 1;
					//// DELAY
					if (this->pattern_cell == 5
					&& g_editor.view_switch[2].state == false)
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
