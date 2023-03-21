
#include "plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static void int_to_hex(char *str, int number, int width) {
	char		digit;
	int			i;

	str[width] = 0;
	for (i = width - 1; i >= 0; --i) {
		digit = number % 16;
		if (digit < 10)
			str[i] = '0' + digit;
		else
			str[i] = 'A' + (digit - 10);
		number /= 16;
	}
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////
/// DISPLAY MAIN
//////////////////////////////////////////////////

TrackerDisplay::TrackerDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerDisplay::drawPattern(const DrawArgs &args, Rect rect) {
	PatternSource			*pattern;
	PatternNote				*note;
	PatternNoteRow			*note_row;
	PatternCV				*cv;
	PatternCVRow			*cv_row;
	PatternEffect			*effect;
	Vec						p;
	bool					focus_row, focus_line, focus_fx, focus;
	int						i, j, k;
	int						line;
	float					x, y;
	int						tx, tx_row;
	char					str[32];

	p = rect.getTopLeft();

	pattern = g_editor.pattern;

	/// [1] LAYER 1 (MARKERS + NOTES + CURVES)

	/// DRAW BEAT CURSOR
	// TODO: clean and not use g_timeline.debug
	// but a pattern internal variable that is set to the line that
	// is being read (last updated if multiple instance of same pattern)
	// Only if play is on
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, module->colors[15]);
	nvgRect(args.vg,
	/**/ p.x,
	/**/ p.y + 3.5 + CHAR_H * (g_timeline.debug - g_editor.pattern_cam_y),
	/**/ rect.getWidth() + 0.5, CHAR_H);
	nvgFill(args.vg);

	/// DRAW PATTERN CURSOR LINE
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, module->colors[15]);
	nvgRect(args.vg,
	/**/ p.x,
	/**/ p.y + 3.5 + CHAR_H * (g_editor.pattern_line - g_editor.pattern_cam_y),
	/**/ rect.getWidth() + 0.5, CHAR_H);
	nvgFill(args.vg);

	/// DRAW LINE / BEAT COUNT
	x = p.x + 2;
	for (i = 0; i < CHAR_COUNT_Y; ++i) {
		line = g_editor.pattern_cam_y + i;
		y = p.y + 11.0 + i * CHAR_H;
		/// BEAT COUNT
		if (line % pattern->lpb == 0) {
			int_to_hex(str, line / pattern->lpb, 2);
			nvgFillColor(args.vg, module->colors[13]);
		/// LINE COUNT
		} else {
			int_to_hex(str, line % pattern->lpb, 2);
			nvgFillColor(args.vg, module->colors[15]);
		}
		nvgText(args.vg, x, y, str, NULL);
	}

	/// [2] LAYER 2 (TRACKER)
	/// FOR EACH NOTE ROW	
	tx = 0;
	for (i = 0; i < pattern->note_count; ++i) {
		note_row = pattern->notes[i];
		focus_row = (g_editor.pattern_row == i);
		/// FOR EACH NOTE ROW LINE
		for (j = 0; j < CHAR_COUNT_Y; ++j) {
			line = g_editor.pattern_cam_y + j;
			if (line >= pattern->line_count)
				break;
			focus_line = focus_row & (g_editor.pattern_line == line);
			tx_row = tx;
			note = &(note_row->lines[line]);
			/// PITCH
			focus = focus_line & (g_editor.pattern_cell == 0);
			nvgFillColor(args.vg, module->colors[3]);
			if (note->mode == PATTERN_NOTE_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = 0;
				this->text(args, p, tx_row, j, str, 3, focus);
			} else {
				this->text(args, p, tx_row, j,
				/**/ table_pitch[note->pitch % 12], 3, focus);
			}
			tx_row += 2;
			/// OCTAVE
			focus = focus_line & (g_editor.pattern_cell == 1);
			nvgFillColor(args.vg, module->colors[2]);
			if (note->mode == PATTERN_NOTE_KEEP) {
				str[0] = '.';
				str[1] = 0;
			} else {
				itoa(note->pitch / 12, str, 10);
			}
			this->text(args, p, tx_row, j, str, 2, focus);
			tx_row += 1;
			/// VELOCITY
			if (g_editor.view_switch[0].state) {
				focus = focus_line & (g_editor.pattern_cell == 2);
				if (note->mode == PATTERN_NOTE_KEEP) {
					str[0] = '.';
					str[1] = '.';
					str[2] = 0;
				} else {
					int_to_hex(str, note->velocity, 2);
				}
				this->text(args, p, tx_row, j, str, 5, focus);
				tx_row += 2;
			}
			/// PANNING
			if (g_editor.view_switch[1].state) {
				focus = focus_line & (g_editor.pattern_cell == 3);
				if (note->mode == PATTERN_NOTE_KEEP) {
					str[0] = '.';
					str[1] = '.';
					str[2] = 0;
				} else {
					int_to_hex(str, note->panning, 2);
				}
				this->text(args, p, tx_row, j, str, 6, focus);
				tx_row += 2;
			}
			/// SYNTH
			focus = focus_line & (g_editor.pattern_cell == 4);
			if (note->mode == PATTERN_NOTE_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = 0;
			} else {
				int_to_hex(str, note->synth, 2);
			}
			this->text(args, p, tx_row, j, str, 4, focus);
			tx_row += 2;
			/// DELAY
			if (g_editor.view_switch[2].state) {
				focus = focus_line & (g_editor.pattern_cell == 5);
				if (note->mode == PATTERN_NOTE_KEEP) {
					str[0] = '.';
					str[1] = '.';
					str[2] = 0;
				} else {
					int_to_hex(str, note->delay, 2);
				}
				this->text(args, p, tx_row, j, str, 10, focus);
				tx_row += 2;
			}
			/// GLIDE
			if (g_editor.view_switch[3].state) {
				focus = focus_line & (g_editor.pattern_cell == 6);
				if (note->mode == PATTERN_NOTE_KEEP
				|| note->mode == PATTERN_NOTE_NEW) {
					str[0] = '.';
					str[1] = '.';
					str[2] = 0;
				} else {
					int_to_hex(str, note->glide, 2);
				}
				this->text(args, p, tx_row, j, str, 11, focus);
				tx_row += 2;
			}
			/// EFFECTS
			if (g_editor.view_switch[4].state) {
				for (k = 0; k < note_row->effect_count; ++k) {
					effect = &(note->effects[k]);
					focus_fx = focus_line & ((g_editor.pattern_cell - 7) / 2 == k);
					/// COMPUTE STRINGS
					if (note->mode == PATTERN_NOTE_KEEP
					|| effect->type == PATTERN_EFFECT_NONE) {
						str[0] = '.';
						str[1] = 0;
						str[2] = '.';
						str[3] = '.';
						str[4] = 0;
					} else {
						str[0] = table_effect[effect->type - 1];
						str[1] = 0;
						int_to_hex(str + 2, note->effects[k].value, 2);
					}
					/// EFFECT TYPE
					focus = focus_fx & ((g_editor.pattern_cell - 7) % 2 == 0);
					this->text(args, p, tx_row, j, str, 13, focus);
					tx_row += 1;
					/// EFFECT VALUE
					focus = focus_fx & ((g_editor.pattern_cell - 7) % 2 == 1);
					this->text(args, p, tx_row, j, str + 2, 14, focus);
					tx_row += 2;
				}
			}
		}
		/// OFFSET X
		tx += (2 + 1														// PITCH
		/**/ + g_editor.view_switch[0].state * 2							// VELOCITY
		/**/ + g_editor.view_switch[1].state * 2							// PANNING
		/**/ + 2					  										// SYNTH
		/**/ + g_editor.view_switch[2].state * 2							// DELAY
		/**/ + g_editor.view_switch[3].state * 2							// GLIDE
		/**/ + g_editor.view_switch[4].state * 3 * note_row->effect_count	// EFFECTS
		/**/ + 1);
	}
	/// FOR EACH CV ROW
	for (i = 0; i < pattern->cv_count; ++i) {
		cv_row = pattern->cvs[i];
		focus_row = (g_editor.pattern_row == pattern->note_count + i);
		/// FOR EACH CV ROW LINE
		for (j = 0; j < CHAR_COUNT_Y; ++j) {
			line = g_editor.pattern_cam_y + j;
			if (line >= pattern->line_count)
				break;
			focus_line = focus_row & (g_editor.pattern_line == line);
			tx_row = tx;
			cv = &(cv_row->lines[line]);
			/// VALUE
			focus = focus_line & (g_editor.pattern_cell == 0);
			nvgFillColor(args.vg, module->colors[3]);
			if (cv->mode == PATTERN_CV_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = 0;
			} else {
				int_to_hex(str, cv->value, 2);
			}
			this->text(args, p, tx_row, j, str, 3, focus);
			tx_row += 2;
			/// GLIDE
			focus = focus_line & (g_editor.pattern_cell == 1);
			nvgFillColor(args.vg, module->colors[5]);
			if (cv->mode == PATTERN_CV_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = 0;
			} else {
				int_to_hex(str, cv->glide, 2);
			}
			this->text(args, p, tx_row, j, str, 5, focus);
			tx_row += 2;
			/// DELAY
			focus = focus_line & (g_editor.pattern_cell == 2);
			nvgFillColor(args.vg, module->colors[10]);
			if (cv->mode == PATTERN_CV_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = 0;
			} else {
				int_to_hex(str, cv->delay, 2);
			}
			this->text(args, p, tx_row, j, str, 10, focus);
			tx_row += 2;
		}
		tx += (2 + 2 + 2 + 1);
	}
}

void TrackerDisplay::drawLayer(const DrawArgs &args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;

	if (module == NULL || layer != 1)
		return;
	/// GET FONT
	font = APP->window->loadFont(font_path);
	if (font == NULL)
		return;
	/// SET FONT
	nvgFontSize(args.vg, 9);
	nvgFontFaceId(args.vg, font->handle);
	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, module->colors[0]);
	nvgRect(args.vg, RECT_ARGS(rect));
	nvgFill(args.vg);

	//// TMP DEBUG ! ! !
	//nvgFillColor(args.vg, module->colors[3]);
	//nvgText(args.vg, p.x + 100, p.y + 11.0, g_timeline.debug_str, NULL);
	// TMP DEBUG ! ! !
	//char text[1024];
	//int test = 0x49;
	//itoa(sizeof(Test), text, 10);
	////int a1 = (test << 4) >> 4;
	////int a2 = (test >> 4);
	//sprintf(text, "%f", char_width);
	///**/ g_editor.pattern_line, g_editor.pattern_cell);
	//nvgText(args.vg, p.x + 400, p.y + 11.0, g_editor.pattern_debug, NULL);
	//nvgText(args.vg, p.x + 400, p.y + 22.0, text, NULL);
	// TMP DEBUG ! ! !

	nvgScissor(args.vg, RECT_ARGS(rect));

	/// DRAW PATTERN
	if (g_editor.pattern) {
		this->drawPattern(args, rect);
	}

	nvgResetScissor(args.vg);
	LedDisplay::drawLayer(args, layer);
}

void TrackerDisplay::text(const DrawArgs& args, Vec p, float x, float y,
	char *str, int color, bool background) {
	float	sx, sy;

	// TODO: handle left under flow ?
	// TODO: handle right over flow ?

	/// [1] OFFSET COORD WITH CAMERA
	x -= g_editor.pattern_cam_x;
	if (x < 0)
		return;
	if (x > CHAR_COUNT_X)
		return;
	/// [2] COMPUTE REAL COORD
	sx = p.x + 2.0 + CHAR_W * (x + 2.0);
	sy = p.y + 11.0 + CHAR_H * y;
	/// [3] DRAW BACKGROUND
	if (background) {
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, module->colors[12]);
		if (str[1] == 0)
			nvgRect(args.vg, sx, sy - CHAR_H + 1.0, CHAR_W, CHAR_H);
		else if (str[2] == 0)
			nvgRect(args.vg, sx, sy - CHAR_H + 1.0, 2.0 * CHAR_W, CHAR_H);
		nvgFill(args.vg);
	}
	/// [4] DRAW TEXT
	nvgFillColor(args.vg, module->colors[color]);
	nvgText(args.vg, sx, sy, str, NULL);
}

///////////////////////////////////////////////////
/// DISPLAY GLOBAL
//////////////////////////////////////////////////

TrackerBPMDisplay::TrackerBPMDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerBPMDisplay::drawLayer(const DrawArgs& args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	Vec						p;
	int						bpm;
	int						synth;
	int						pattern;

	if (module == NULL || layer != 1)
		return;
	/// GET FONT
	font = APP->window->loadFont(font_path);
	if (font == NULL)
		return;
	/// SET FONT
	nvgFontSize(args.vg, 18);
	nvgFontFaceId(args.vg, font->handle);
	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, module->colors[0]);
	nvgRect(args.vg, RECT_ARGS(rect));
	nvgFill(args.vg);

	/// DRAW BPM
	bpm = module->params[Tracker::PARAM_BPM].getValue();
	if (bpm < 100) {
		itoa(bpm, str_bpm + 1, 10);
		str_bpm[0] = ' ';
	} else {
		itoa(bpm, str_bpm, 10);
	}
	nvgFillColor(args.vg, module->colors[12]);
	nvgText(args.vg, p.x + 40.0, p.y + 25.0, str_bpm, NULL);
	/// DRAW ACTIVE SYNTH
	synth = module->params[Tracker::PARAM_SYNTH].getValue();
	int_to_hex(str_bpm, synth, 2);
	nvgFillColor(args.vg, module->colors[12]);
	nvgText(args.vg, p.x + 40.0 + CHAR_W * 2, p.y + 25.0 + 25.0, str_bpm, NULL);
	/// DRAW ACTIVE PATTERN
	pattern = module->params[Tracker::PARAM_PATTERN].getValue();
	int_to_hex(str_bpm, pattern, 2);
	nvgFillColor(args.vg, module->colors[12]);
	nvgText(args.vg, p.x + 40.0 + CHAR_W * 2, p.y + 25.0 + 50.0, str_bpm, NULL);

	LedDisplay::drawLayer(args, layer);
}

///////////////////////////////////////////////////
/// DISPLAY EDIT
//////////////////////////////////////////////////

TrackerEditDisplay::TrackerEditDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerEditDisplay::drawLayer(const DrawArgs& args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	Vec						p;

	if (module == NULL || layer != 1)
		return;
	/// GET FONT
	font = APP->window->loadFont(font_path);
	if (font == NULL)
		return;
	/// SET FONT
	nvgFontSize(args.vg, 18);
	nvgFontFaceId(args.vg, font->handle);
	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, module->colors[0]);
	nvgRect(args.vg, RECT_ARGS(rect));
	nvgFill(args.vg);

	LedDisplay::drawLayer(args, layer);
}
