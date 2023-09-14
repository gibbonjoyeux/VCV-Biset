
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static int key_midi(const Widget::SelectKeyEvent &e) {
	int			midi;

	if (e.key < 0 || e.key >= 128)
		return -2;
	if (e.keyName[0] == 'o')
		return -1;
	midi = table_keyboard[e.key];
	if (midi < 0)
		return -2;
	midi += g_editor->pattern_octave * 12;
	if (midi > 127)
		midi = 127;
	return midi;
}

static int key_dec(const Widget::SelectKeyEvent &e) {
	if (e.key >= GLFW_KEY_0 && e.key <= GLFW_KEY_9)
		return e.key - GLFW_KEY_0;
	return -1;
}

static int key_alpha(const Widget::SelectKeyEvent &e) {
	if (e.keyName[0] >= 'a' && e.keyName[0] <= 'z') {
		if (e.mods & GLFW_MOD_SHIFT)
			return e.keyName[0] + ('A' - 'a');
		else
			return e.keyName[0];
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::on_key_pattern(const Widget::SelectKeyEvent &e) {
	PatternSource	*pattern;
	PatternNoteCol	*col_note;
	PatternNote		*line_note;
	PatternNote		*line_note_comp;
	PatternEffect	*effect;
	PatternCVCol	*col_cv;
	PatternCV		*line_cv;
	int				fx_1, fx_2;
	int				key;
	int				i;

	e.consume(this);
	if (e.action == GLFW_PRESS
	|| e.action == GLFW_REPEAT) {
		if (g_editor->pattern) {
			/// EVENT CURSOR MOVE
			if (e.key == GLFW_KEY_LEFT) {
				g_editor->pattern_move_cursor_x(-1);
			} else if (e.key == GLFW_KEY_RIGHT) {
				g_editor->pattern_move_cursor_x(+1);
			} else if (e.key == GLFW_KEY_UP) {
				g_editor->pattern_move_cursor_y(-1);
			} else if (e.key == GLFW_KEY_DOWN) {
				g_editor->pattern_move_cursor_y(+1);
			/// EVENT KEYBOARD
			} else {
				pattern = g_editor->pattern;
				/// KEY ON NOTE
				if (g_editor->pattern_col < pattern->note_count) {
					col_note = pattern->notes[g_editor->pattern_col];
					line_note = &(col_note->lines[g_editor->pattern_line]);
					switch (g_editor->pattern_cell) {
						/// PITCH
						case 0:
							/// NOTE DELETE
							if (e.key == GLFW_KEY_DELETE
							|| e.key == GLFW_KEY_BACKSPACE) {
								if (line_note->mode == PATTERN_NOTE_NEW)
									line_note->mode = PATTERN_NOTE_KEEP;
								else if (line_note->mode == PATTERN_NOTE_KEEP)
									line_note->mode = PATTERN_NOTE_STOP;
								else if (line_note->mode == PATTERN_NOTE_STOP)
									line_note->mode = PATTERN_NOTE_KEEP;
								else if (line_note->mode == PATTERN_NOTE_GLIDE)
									line_note->mode = PATTERN_NOTE_KEEP;
								g_editor->pattern_jump_cursor();
							/// NOTE EDIT
							} else {
								key = key_midi(e);
								/// NOTE NEW
								if (key >= 0) {
									line_note->pitch = key;
									if (g_editor->synth_id >= 0)
										line_note->synth = g_editor->synth_id;
									else
										line_note->synth = 0;
									if (line_note->mode == PATTERN_NOTE_KEEP
									|| line_note->mode == PATTERN_NOTE_STOP) {
										line_note->mode = PATTERN_NOTE_NEW;
										line_note->velocity = 99;
										line_note->panning = 50;
									}
									strcpy(g_editor->pattern_debug,
									/**/ table_pitch[key % 12]);
									g_editor->pattern_jump_cursor();
								/// NOTE STOP
								} else if (key == -1) {
									line_note->mode = PATTERN_NOTE_STOP;
									g_editor->pattern_jump_cursor();
								}
							}
							break;
						/// OCTAVE
						case 1:
							key = key_dec(e);
							if (key >= 0 && key <= 9) {
								line_note->pitch =
								/**/ line_note->pitch % 12
								/**/ + key * 12;
								g_editor->pattern_jump_cursor();
							}
							break;
						/// VELOCITY
						case 2:
							key = key_dec(e);
							if (key >= 0) {
								if (g_editor->pattern_char == 0) {
									line_note->velocity =
									/**/ line_note->velocity % 10
									/**/ + key * 10;
									g_editor->pattern_char += 1;
								} else {
									line_note->velocity =
									/**/ (line_note->velocity / 10) * 10
									/**/ + key;
									g_editor->pattern_jump_cursor();
								}
							}
							if (line_note->mode == PATTERN_NOTE_KEEP) {
								line_note->mode = PATTERN_NOTE_GLIDE;
								line_note->glide = 99;
								line_note->pitch = 69;
								line_note->panning = 50;
								for (i = g_editor->pattern_line - 1; i >= 0; --i) {
									line_note_comp = &(col_note->lines[i]);
									if (line_note_comp->mode != PATTERN_NOTE_KEEP) {
										line_note->pitch = line_note_comp->pitch;
										line_note->panning = line_note_comp->panning;
										break;
									}
								}
							}
							break;
						/// PANNING
						case 3:
							key = key_dec(e);
							if (key >= 0) {
								if (g_editor->pattern_char == 0) {
									line_note->panning =
									/**/ line_note->panning % 10
									/**/ + key * 10;
									g_editor->pattern_char += 1;
								} else {
									line_note->panning =
									/**/ (line_note->panning / 10) * 10
									/**/ + key;
									g_editor->pattern_jump_cursor();
								}
							}
							if (line_note->mode == PATTERN_NOTE_KEEP) {
								line_note->mode = PATTERN_NOTE_GLIDE;
								line_note->glide = 99;
								line_note->pitch = 69;
								line_note->velocity = 99;
								for (i = g_editor->pattern_line - 1; i >= 0; --i) {
									line_note_comp = &(col_note->lines[i]);
									if (line_note_comp->mode != PATTERN_NOTE_KEEP) {
										line_note->pitch = line_note_comp->pitch;
										line_note->velocity = line_note_comp->velocity;
										break;
									}
								}
							}
							break;
						/// SYNTH
						case 4:
							key = key_dec(e);
							if (key >= 0) {
								if (g_editor->pattern_char == 0) {
									line_note->synth =
									/**/ line_note->synth % 10
									/**/ + key * 10;
									g_editor->pattern_char += 1;
								} else {
									line_note->synth =
									/**/ (line_note->synth / 10) * 10
									/**/ + key;
									g_editor->pattern_jump_cursor();
								}
							}
							break;
						/// DELAY
						case 5:
							key = key_dec(e);
							if (key >= 0) {
								if (g_editor->pattern_char == 0) {
									line_note->delay =
									/**/ line_note->delay % 10
									/**/ + key * 10;
									g_editor->pattern_char += 1;
								} else {
									line_note->delay =
									/**/ (line_note->delay / 10) * 10
									/**/ + key;
									g_editor->pattern_jump_cursor();
								}
							}
							break;
						/// GLIDE
						case 6:
							/// GLIDE DELETE
							if (e.key == GLFW_KEY_DELETE
							|| e.key == GLFW_KEY_BACKSPACE) {
								if (line_note->mode == PATTERN_NOTE_GLIDE)
									line_note->mode = PATTERN_NOTE_NEW;
								g_editor->pattern_jump_cursor();
							/// GLIDE EDIT
							} else {
								key = key_dec(e);
								if (key >= 0) {
									if (line_note->mode == PATTERN_NOTE_NEW)
										line_note->mode = PATTERN_NOTE_GLIDE;
									if (g_editor->pattern_char == 0) {
										line_note->glide =
										/**/ line_note->glide % 10
										/**/ + key * 10;
										g_editor->pattern_char += 1;
									} else {
										line_note->glide =
										/**/ (line_note->glide / 10) * 10
										/**/ + key;
										g_editor->pattern_jump_cursor();
									}
								}
							}
							break;
						/// EFFECT
						default:
							fx_1 = (g_editor->pattern_cell - 7) / 2;
							fx_2 = (g_editor->pattern_cell - 7) % 2;
							effect = &(line_note->effects[fx_1]);
							/// EFFECT DELETE
							if (e.key == GLFW_KEY_DELETE
							|| e.key == GLFW_KEY_BACKSPACE) {
								effect->type = PATTERN_EFFECT_NONE;
								effect->value = 0;
								g_editor->pattern_jump_cursor();
							/// EFFECT EDIT
							} else {
								/// EDIT EFFECT TYPE
								if (fx_2 == 0) {
									key = key_alpha(e);
									if (key > 0) {
										i = 0;
										/// FIND EFFECT TYPE
										while (i < (int)sizeof(table_effect)) {
											/// MATCH EFFECT TYPE
											if (key == table_effect[i]) {
												effect->type = i + 1;
												g_editor->pattern_jump_cursor();
											}
											i += 1;
										}
									}
								/// EDIT EFFECT VALUE
								} else {
									key = key_dec(e);
									if (key >= 0) {
										if (g_editor->pattern_char == 0) {
											effect->value =
											/**/ effect->value % 10
											/**/ + key * 10;
											g_editor->pattern_char += 1;
										} else {
											effect->value =
											/**/ (effect->value / 10) * 10
											/**/ + key;
											g_editor->pattern_jump_cursor();
										}
									}
								}
							}
							break;
					}
				/// KEY ON CV
				} else {
					col_cv = pattern->cvs[g_editor->pattern_col - pattern->note_count];
					line_cv = &(col_cv->lines[g_editor->pattern_line]);
					switch (g_editor->pattern_cell) {
						/// VALUE
						case 0:
							/// VALUE DELETE
							if (e.key == GLFW_KEY_DELETE
							|| e.key == GLFW_KEY_BACKSPACE) {
								line_cv->mode = PATTERN_CV_KEEP;
								g_editor->pattern_jump_cursor();
							/// VALUE EDIT
							} else {
								key = key_dec(e);
								if (key >= 0) {
									if (line_cv->mode == PATTERN_CV_KEEP)
										line_cv->mode = PATTERN_CV_SET;
									if (g_editor->pattern_char == 0) {
										line_cv->value =
										/**/ key * 100
										/**/ + line_cv->value % 100;
										g_editor->pattern_char += 1;
									} else if (g_editor->pattern_char == 1) {
										line_cv->value =
										/**/ (line_cv->value / 100) * 100
										/**/ + key * 10
										/**/ + (line_cv->value % 10);
										g_editor->pattern_char += 1;
									} else {
										line_cv->value =
										/**/ (line_cv->value / 10) * 10
										/**/ + key;
										g_editor->pattern_jump_cursor();
									}
								}
							}
							break;
						/// GLIDE
						case 1:
							key = key_dec(e);
							if (key >= 0) {
								if (g_editor->pattern_char == 0) {
									line_cv->glide =
									/**/ line_cv->glide % 10
									/**/ + key * 10;
									g_editor->pattern_char += 1;
								} else {
									line_cv->glide =
									/**/ (line_cv->glide / 10) * 10
									/**/ + key;
									g_editor->pattern_char = 0;
									g_editor->pattern_jump_cursor();
								}
							}
							break;
						/// DELAY
						case 2:
							key = key_dec(e);
							if (key >= 0) {
								if (g_editor->pattern_char == 0) {
									line_cv->delay =
									/**/ line_cv->delay % 10
									/**/ + key * 10;
									g_editor->pattern_char += 1;
								} else {
									line_cv->delay =
									/**/ (line_cv->delay / 10) * 10
									/**/ + key;
									g_editor->pattern_char = 0;
									g_editor->pattern_jump_cursor();
								}
							}
							break;
					}
				}
			}
			/// CLAMP CURSOR
			g_editor->pattern_clamp_cursor();
		}
	}
}
