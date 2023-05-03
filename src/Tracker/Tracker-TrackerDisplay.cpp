
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// PATTERN TEXT
static void ptext(const Widget::DrawArgs& args, Vec p, float x, float y,
	char *str, int color, bool background) {
	float	sx, sy;

	// TODO: handle left under flow ?
	// TODO: handle right over flow ?

	/// [1] OFFSET COORD WITH CAMERA
	x -= g_editor.pattern_cam_x;
	if (x < 0)
		return;
	if (x > CHAR_COUNT_X - 1)
		return;
	/// [2] COMPUTE REAL COORD
	sx = p.x + 2.0 + CHAR_W * (x + 3.0);
	sy = p.y + 11.0 + CHAR_H * y;
	/// [3] DRAW BACKGROUND
	if (background) {
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, colors[12]);
		if (str[1] == 0)
			nvgRect(args.vg, sx, sy - CHAR_H + 1.0, CHAR_W, CHAR_H);
		else if (str[2] == 0)
			nvgRect(args.vg, sx, sy - CHAR_H + 1.0, 2.0 * CHAR_W, CHAR_H);
		else if (str[3] == 0)
			nvgRect(args.vg, sx, sy - CHAR_H + 1.0, 3.0 * CHAR_W, CHAR_H);
		nvgFill(args.vg);
	}
	/// [4] DRAW TEXT
	nvgFillColor(args.vg, colors[color]);
	nvgText(args.vg, sx, sy, str, NULL);
}

/// TIMELINE TEXT
static void ttext(const Widget::DrawArgs& args, Vec p, float x, float y,
	char *str, int color, bool background) {
	float	sx, sy;

	/// [1] COMPUTE REAL COORD
	sx = p.x + 2.0 + CHAR_W * (x + 3.0);
	sy = p.y + 11.0 + CHAR_H * y;
	/// [3] DRAW BACKGROUND
	if (background) {
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, colors[12]);
		nvgRect(args.vg, sx, sy - CHAR_H + 1.0, 3.0 * CHAR_W, CHAR_H);
		nvgFill(args.vg);
	}
	/// [4] DRAW TEXT
	nvgFillColor(args.vg, colors[color]);
	nvgText(args.vg, sx, sy, str, NULL);
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerDisplay::TrackerDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerDisplay::draw(const DrawArgs &args) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	float					off_x, off_y;

	LedDisplay::draw(args);

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[0]);
	//nvgRect(args.vg, RECT_ARGS(rect));
	nvgRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0);
	nvgFill(args.vg);

	if (module == NULL) {

		/// GET FONT
		font = APP->window->loadFont(font_path);
		if (font == NULL)
			return;
		/// SET FONT
		nvgFontSize(args.vg, 80);
		nvgFontFaceId(args.vg, font->handle);
		/// DRAW TITLE
		off_x = (rect.size.x / 2) - 195.0;
		off_y = (rect.size.y / 2) + 30.0;
		nvgFillColor(args.vg, colors[15]);
		nvgText(args.vg, rect.pos.x + off_x, rect.pos.y + off_y,
		/**/ "TRACKER", NULL);
	}
}

void TrackerDisplay::draw_pattern(const DrawArgs &args, Rect rect) {
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

	pattern = g_editor.pattern;
	p = rect.getTopLeft();

	/// [1] LAYER 1 (MARKERS + NOTES + CURVES)

	/// DRAW BEAT CURSOR
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[15]);
	nvgRect(args.vg,
	/**/ p.x,
	/**/ p.y + 3.5 + CHAR_H * (pattern->line_play - g_editor.pattern_cam_y),
	/**/ rect.getWidth() + 0.5, CHAR_H);
	nvgFill(args.vg);

	/// DRAW PATTERN CURSOR LINE
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[15]);
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
			itoaw(str, line / pattern->lpb, 3);
			nvgFillColor(args.vg, colors[13]);
		/// LINE COUNT
		} else {
			itoaw(str, line % pattern->lpb, 2);
			nvgFillColor(args.vg, colors[15]);
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
			nvgFillColor(args.vg, colors[3]);
			if (note->mode == PATTERN_NOTE_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = 0;
				ptext(args, p, tx_row, j, str, 3, focus);
			} else if (note->mode == PATTERN_NOTE_STOP) {
				str[0] = '-';
				str[1] = '-';
				str[2] = 0;
				ptext(args, p, tx_row, j, str, 3, focus);
			} else {
				ptext(args, p, tx_row, j,
				/**/ table_pitch[note->pitch % 12], 3, focus);
			}
			tx_row += 2;
			/// OCTAVE
			focus = focus_line & (g_editor.pattern_cell == 1);
			nvgFillColor(args.vg, colors[2]);
			if (note->mode == PATTERN_NOTE_KEEP) {
				str[0] = '.';
				str[1] = 0;
			} else if (note->mode == PATTERN_NOTE_STOP) {
				str[0] = '-';
				str[1] = 0;
			} else {
				itoa(note->pitch / 12, str, 10);
			}
			ptext(args, p, tx_row, j, str, 2, focus);
			tx_row += 1;
			/// VELOCITY
			if (g_editor.switch_view[0].state) {
				focus = focus_line & (g_editor.pattern_cell == 2);
				if (note->mode == PATTERN_NOTE_NEW) {
					itoaw(str, note->velocity, 2);
				} else {
					str[0] = '.';
					str[1] = '.';
					str[2] = 0;
				}
				ptext(args, p, tx_row, j, str, 5, focus);
				tx_row += 2;
			}
			/// PANNING
			if (g_editor.switch_view[1].state) {
				focus = focus_line & (g_editor.pattern_cell == 3);
				if (note->mode == PATTERN_NOTE_NEW) {
					itoaw(str, note->panning, 2);
				} else {
					str[0] = '.';
					str[1] = '.';
					str[2] = 0;
				}
				ptext(args, p, tx_row, j, str, 6, focus);
				tx_row += 2;
			}
			/// SYNTH
			focus = focus_line & (g_editor.pattern_cell == 4);
			if (note->mode == PATTERN_NOTE_NEW) {
				itoaw(str, note->synth, 2);
			} else {
				str[0] = '.';
				str[1] = '.';
				str[2] = 0;
			}
			ptext(args, p, tx_row, j, str, 4, focus);
			tx_row += 2;
			/// DELAY
			if (g_editor.switch_view[2].state) {
				focus = focus_line & (g_editor.pattern_cell == 5);
				if (note->mode != PATTERN_NOTE_KEEP) {
					itoaw(str, note->delay, 2);
				} else {
					str[0] = '.';
					str[1] = '.';
					str[2] = 0;
				}
				ptext(args, p, tx_row, j, str, 10, focus);
				tx_row += 2;
			}
			/// GLIDE
			if (g_editor.switch_view[3].state) {
				focus = focus_line & (g_editor.pattern_cell == 6);
				if (note->mode == PATTERN_NOTE_GLIDE) {
					itoaw(str, note->glide, 2);
				} else {
					str[0] = '.';
					str[1] = '.';
					str[2] = 0;
				}
				ptext(args, p, tx_row, j, str, 11, focus);
				tx_row += 2;
			}
			/// EFFECTS
			if (g_editor.switch_view[4].state) {
				for (k = 0; k < note_row->effect_count; ++k) {
					effect = &(note->effects[k]);
					focus_fx = focus_line & ((g_editor.pattern_cell - 7) / 2 == k);
					/// COMPUTE STRINGS
					if (note->mode == PATTERN_NOTE_KEEP
					|| note->mode == PATTERN_NOTE_STOP
					|| effect->type == PATTERN_EFFECT_NONE) {
						str[0] = '.';
						str[1] = 0;
						str[2] = '.';
						str[3] = '.';
						str[4] = 0;
					} else {
						str[0] = table_effect[effect->type - 1];
						str[1] = 0;
						itoaw(str + 2, note->effects[k].value, 2);
					}
					/// EFFECT TYPE
					focus = focus_fx & ((g_editor.pattern_cell - 7) % 2 == 0);
					ptext(args, p, tx_row, j, str, 13, focus);
					tx_row += 1;
					/// EFFECT VALUE
					focus = focus_fx & ((g_editor.pattern_cell - 7) % 2 == 1);
					ptext(args, p, tx_row, j, str + 2, 14, focus);
					tx_row += 2;
				}
			}
		}
		/// OFFSET X
		tx += (2 + 1														// PITCH
		/**/ + g_editor.switch_view[0].state * 2							// VELOCITY
		/**/ + g_editor.switch_view[1].state * 2							// PANNING
		/**/ + 2					  										// SYNTH
		/**/ + g_editor.switch_view[2].state * 2							// DELAY
		/**/ + g_editor.switch_view[3].state * 2							// GLIDE
		/**/ + g_editor.switch_view[4].state * 3 * note_row->effect_count	// EFFECTS
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
			nvgFillColor(args.vg, colors[3]);
			if (cv->mode == PATTERN_CV_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = '.';
				str[3] = 0;
			} else {
				itoaw(str, cv->value, 3);
			}
			ptext(args, p, tx_row, j, str, 3, focus);
			tx_row += 3;
			/// GLIDE
			focus = focus_line & (g_editor.pattern_cell == 1);
			nvgFillColor(args.vg, colors[5]);
			if (cv->mode == PATTERN_CV_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = 0;
			} else {
				itoaw(str, cv->glide, 2);
			}
			ptext(args, p, tx_row, j, str, 5, focus);
			tx_row += 2;
			/// DELAY
			focus = focus_line & (g_editor.pattern_cell == 2);
			nvgFillColor(args.vg, colors[10]);
			if (cv->mode == PATTERN_CV_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = 0;
			} else {
				itoaw(str, cv->delay, 2);
			}
			ptext(args, p, tx_row, j, str, 10, focus);
			tx_row += 2;
		}
		tx += (3 + 2 + 2 + 1);
	}
}

void TrackerDisplay::draw_timeline(const DrawArgs &args, Rect rect) {
	TimelineCell			*cell;
	Vec						p;
	int						line;
	int						i, j;
	float					x, y;
	bool					focus_col, focus_line, focus_cell;
	char					str[32];

	p = rect.getTopLeft();

	/// [1] LAYER 1 (MARKERS + NOTES + CURVES)

	/// DRAW BEAT CURSOR
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[15]);
	nvgRect(args.vg,
	/**/ p.x,
	/**/ p.y + 3.5 + CHAR_H * (g_timeline.clock.beat - g_editor.pattern_cam_y),
	/**/ rect.getWidth() + 0.5, CHAR_H);
	nvgFill(args.vg);

	/// DRAW TIMELINE CURSOR LINE
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[15]);
	nvgRect(args.vg,
	/**/ p.x,
	/**/ p.y + 3.5 + CHAR_H * (g_editor.timeline_line - g_editor.timeline_cam_y),
	/**/ rect.getWidth() + 0.5, CHAR_H);
	nvgFill(args.vg);

	/// DRAW LINE / BEAT COUNT
	x = p.x + 2;
	for (i = 0; i < CHAR_COUNT_Y; ++i) {
		line = g_editor.timeline_cam_y + i;
		y = p.y + 11.0 + i * CHAR_H;
		itoaw(str, line, 3);
		if (line % 2 == 0)
			nvgFillColor(args.vg, colors[13]);
		else
			nvgFillColor(args.vg, colors[14]);
		nvgText(args.vg, x, y, str, NULL);
	}
	/// [2] LAYER 2 (TIMELINE)
	/// FOR EACH COLUMN
	for (i = 0; i < 12; ++i) {
		focus_col = (g_editor.timeline_column == i);
		/// FOR EACH LINE
		for (j = 0; j < CHAR_COUNT_Y; ++j) {
			line = g_editor.timeline_cam_y + j;
			if (line >= g_timeline.beat_count)
				break;
			cell = &(g_timeline.timeline[i][line]);
			focus_line = focus_col && (g_editor.timeline_line == line);
			/// DRAW PATTERN INDEX
			focus_cell = focus_line && (g_editor.timeline_cell == 0);
			if (cell->mode == TIMELINE_CELL_NEW) {
				itoaw(str, cell->pattern, 3);
			} else if (cell->mode == TIMELINE_CELL_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = '.';
				str[3] = 0;
			} else {
				str[0] = '-';
				str[1] = '-';
				str[2] = '-';
				str[3] = 0;
			}
			ttext(args, p, i * 7, j, str, 2, focus_cell);
			/// DRAW PATTERN START
			focus_cell = focus_line && (g_editor.timeline_cell == 1);
			if (cell->mode == TIMELINE_CELL_NEW) {
				itoaw(str, cell->beat, 3);
			} else if (cell->mode == TIMELINE_CELL_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = '.';
				str[3] = 0;
			} else {
				str[0] = '-';
				str[1] = '-';
				str[2] = '-';
				str[3] = 0;
			}
			ttext(args, p, i * 7 + 3, j, str, 3, focus_cell);
		}
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
	//nvgBeginPath(args.vg);
	//nvgFillColor(args.vg, colors[0]);
	//nvgRect(args.vg, RECT_ARGS(rect));
	//nvgFill(args.vg);

	//// TMP DEBUG ! ! !
	nvgFillColor(args.vg, colors[3]);
	nvgText(args.vg, rect.pos.x + 100, rect.pos.y + 21.0, g_timeline.debug_str, NULL);
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
	if (g_editor.mode == EDITOR_MODE_PATTERN) {
		this->draw_pattern(args, rect);
	} else if (g_editor.mode == EDITOR_MODE_TIMELINE) {
		this->draw_timeline(args, rect);
	} else if (g_editor.mode == EDITOR_MODE_PARAMETERS) {
	}

	nvgResetScissor(args.vg);
	LedDisplay::drawLayer(args, layer);
}

