
#include "plugin.hpp"

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
	return midi + 60;
}

static int key_hex(const Widget::SelectKeyEvent &e) {
	if (e.key >= GLFW_KEY_0 && e.key <= GLFW_KEY_9)
		return e.key - GLFW_KEY_0;
	else if (e.keyName[0] >= 'a' && e.keyName[0] <= 'f')
		return 10 + e.keyName[0] - 'a';
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

TrackerWidget::TrackerWidget(Tracker* _module) {
	TrackerDisplay*		display;
	TrackerBPMDisplay*	display_bpm;

	//
	// BUTTONS:
	// - TL1105
	//
	// KNOBS:
	// - RoundBlackKnob
	// - RoundKnob
	// - Round(Big / Huge / Large / Small)BlackKnob
	// - Trimpot (very small)
	// - Rogan(1 / 2 / 3 / 5 / 6)(P / PS)(Blue / Green / Red / White / Gray)
	// - Befaco(Big / Tiny)Knob
	// - Davies1900h(Large)(Black / Red / White)Knob
	//
	// INPUTS:
	// - PJ301MPort
	//
	// OUTPUTS:
	// - PJ301MPort
	//
	// LIGHTS:
	// - LargeLight<YellowLight>
	// - SmallLight<YellowLight>
	//

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker.svg")));

	/// [1] ADD PARAMS
	addParam(
	/**/ createParamCentered<Rogan2PSWhite>(mm2px(Vec(10.125, 14.0)),
	/**/ module,
	/**/ Tracker::PARAM_BPM));
	/// [2] ADD OUTPUT
	addOutput(
	/**/ createOutputCentered<PJ301MPort>(mm2px(Vec(9.1, 119.35)), //135.05
	/**/ module,
	/**/ Tracker::OUTPUT_CLOCK));
	/// [3] ADD LIGHTS
	addChild(
	/**/ createLightCentered<LargeLight<YellowLight>>(mm2px(Vec(240.0, 3.5)),
	/**/ module,
	/**/ Tracker::LIGHT_FOCUS));
	/// [4] ADD DISPLAYS
	//// MAIN LED DISPLAY
	display = createWidget<TrackerDisplay>(mm2px(Vec(63.40, 7.15)));
	display->box.size = mm2px(Vec(173.5, 94.5));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);
	//// BPM LED DISPLAY
	display_bpm = createWidget<TrackerBPMDisplay>(mm2px(Vec(2.0, 23.0)));
	display_bpm->box.size = mm2px(Vec(16.5, 10.0));
	display_bpm->module = module;
	display_bpm->moduleWidget = this;
	addChild(display_bpm);
}

void TrackerWidget::onSelectKey(const SelectKeyEvent &e) {
	PatternSource	*pattern;
	PatternNoteRow	*row_note;
	PatternNote		*line_note;
	PatternEffect	*effect;
	PatternCVRow	*row_cv;
	PatternCV		*line_cv;
	int				fx_1, fx_2;
	int				key;
	int				i;

	e.consume(this);
	if (e.action == GLFW_PRESS
	|| e.action == GLFW_REPEAT) {
		if (g_editor.pattern) {
			/// EVENT CURSOR MOVE
			if (e.key == GLFW_KEY_LEFT) {
				g_editor.pattern_move_cursor_x(-1);
			} else if (e.key == GLFW_KEY_RIGHT) {
				g_editor.pattern_move_cursor_x(+1);
			} else if (e.key == GLFW_KEY_UP) {
				g_editor.pattern_move_cursor_y(-1);
			} else if (e.key == GLFW_KEY_DOWN) {
				g_editor.pattern_move_cursor_y(+1);
			/// EVENT KEYBOARD
			} else {
				pattern = g_editor.pattern;
				/// KEY ON NOTE
				if (g_editor.pattern_row < pattern->note_count) {
					row_note = pattern->notes[g_editor.pattern_row];
					line_note = &(row_note->lines[g_editor.pattern_line]);
					switch (g_editor.pattern_cell) {
						/// PITCH
						case 0:
							/// NOTE DELETE
							if (e.key == GLFW_KEY_DELETE
							|| e.key == GLFW_KEY_BACKSPACE) {
								line_note->mode = PATTERN_NOTE_KEEP;
							/// NOTE EDIT
							} else {
								key = key_midi(e);
								/// NOTE NEW
								if (key >= 0) {
									line_note->pitch = key;
									if (line_note->mode == PATTERN_NOTE_KEEP
									|| line_note->mode == PATTERN_NOTE_STOP) {
										line_note->mode = PATTERN_NOTE_NEW;
										line_note->velocity = 255;
										line_note->panning = 128;
									}
									strcpy(g_editor.pattern_debug,
									/**/ table_pitch[key % 12]);
									g_editor.pattern_move_cursor_y(1);
								/// NOTE STOP
								} else if (key == -1) {
									line_note->mode = PATTERN_NOTE_STOP;
								}
							}
							break;
						/// OCTAVE
						case 1:
							key = key_hex(e);
							if (key >= 0 && key <= 9) {
								line_note->pitch =
								/**/ line_note->pitch % 12
								/**/ + key * 12;
							}
							break;
						/// VELOCITY
						case 2:
							key = key_hex(e);
							if (key >= 0) {
								if (g_editor.pattern_char == 0) {
									line_note->velocity =
									/**/ line_note->velocity % 16
									/**/ + key * 16;
									g_editor.pattern_char += 1;
								} else {
									line_note->velocity =
									/**/ (line_note->velocity / 16) * 16
									/**/ + key;
									g_editor.pattern_move_cursor_y(1);
								}
							}
							break;
						/// PANNING
						case 3:
							key = key_hex(e);
							if (key >= 0) {
								if (g_editor.pattern_char == 0) {
									line_note->panning =
									/**/ line_note->panning % 16
									/**/ + key * 16;
									g_editor.pattern_char += 1;
								} else {
									line_note->panning =
									/**/ (line_note->panning / 16) * 16
									/**/ + key;
									g_editor.pattern_move_cursor_y(1);
								}
							}
							break;
						/// SYNTH
						case 4:
							key = key_hex(e);
							if (key >= 0) {
								if (g_editor.pattern_char == 0) {
									line_note->synth =
									/**/ line_note->synth % 16
									/**/ + key * 16;
									g_editor.pattern_char += 1;
								} else {
									line_note->synth =
									/**/ (line_note->synth / 16) * 16
									/**/ + key;
									g_editor.pattern_move_cursor_y(1);
								}
							}
							break;
						/// DELAY
						case 5:
							key = key_hex(e);
							if (key >= 0) {
								if (g_editor.pattern_char == 0) {
									line_note->delay =
									/**/ line_note->delay % 16
									/**/ + key * 16;
									g_editor.pattern_char += 1;
								} else {
									line_note->delay =
									/**/ (line_note->delay / 16) * 16
									/**/ + key;
									g_editor.pattern_move_cursor_y(1);
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
							/// GLIDE EDIT
							} else {
								key = key_hex(e);
								if (key >= 0) {
									if (line_note->mode == PATTERN_NOTE_NEW)
										line_note->mode = PATTERN_NOTE_GLIDE;
									if (g_editor.pattern_char == 0) {
										line_note->glide =
										/**/ line_note->glide % 16
										/**/ + key * 16;
										g_editor.pattern_char += 1;
									} else {
										line_note->glide =
										/**/ (line_note->glide / 16) * 16
										/**/ + key;
										g_editor.pattern_move_cursor_y(1);
									}
								}
							}
							break;
						/// EFFECT
						default:
							fx_1 = (g_editor.pattern_cell - 7) / 2;
							fx_2 = (g_editor.pattern_cell - 7) % 2;
							effect = &(line_note->effects[fx_1]);
							/// EFFECT DELETE
							if (e.key == GLFW_KEY_DELETE
							|| e.key == GLFW_KEY_BACKSPACE) {
								effect->type = PATTERN_EFFECT_NONE;
								effect->value = 0;
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
												g_editor.pattern_move_cursor_y(1);
											}
											i += 1;
										}
									}
								/// EDIT EFFECT VALUE
								} else {
									key = key_hex(e);
									if (key >= 0) {
										if (g_editor.pattern_char == 0) {
											effect->value =
											/**/ effect->value % 16
											/**/ + key * 16;
											g_editor.pattern_char += 1;
										} else {
											effect->value =
											/**/ (effect->value / 16) * 16
											/**/ + key;
											g_editor.pattern_move_cursor_y(1);
										}
									}
								}
							}
							break;
					}
				/// KEY ON CV
				} else {
					row_cv = pattern->cvs[g_editor.pattern_row - pattern->note_count];
					line_cv = &(row_cv->lines[g_editor.pattern_line]);
					switch (g_editor.pattern_cell) {
						/// VALUE
						case 0:
							/// VALUE DELETE
							if (e.key == GLFW_KEY_DELETE
							|| e.key == GLFW_KEY_BACKSPACE) {
								line_cv->mode = PATTERN_CV_KEEP;
							/// VALUE EDIT
							} else {
								key = key_hex(e);
								if (key >= 0) {
									if (line_cv->mode == PATTERN_CV_KEEP)
										line_cv->mode = PATTERN_CV_SET;
									if (g_editor.pattern_char == 0) {
										line_cv->value =
										/**/ line_cv->value % 16
										/**/ + key * 16;
										g_editor.pattern_char += 1;
									} else {
										line_cv->value =
										/**/ (line_cv->value / 16) * 16
										/**/ + key;
										g_editor.pattern_char = 0;
									}
								}
							}
							break;
						/// GLIDE
						case 1:
							key = key_hex(e);
							if (key >= 0) {
								if (g_editor.pattern_char == 0) {
									line_cv->glide =
									/**/ line_cv->glide % 16
									/**/ + key * 16;
									g_editor.pattern_char += 1;
								} else {
									line_cv->glide =
									/**/ (line_cv->glide / 16) * 16
									/**/ + key;
									g_editor.pattern_char = 0;
								}
							}
							break;
						/// DELAY
						case 2:
							key = key_hex(e);
							if (key >= 0) {
								if (g_editor.pattern_char == 0) {
									line_cv->delay =
									/**/ line_cv->delay % 16
									/**/ + key * 16;
									g_editor.pattern_char += 1;
								} else {
									line_cv->delay =
									/**/ (line_cv->delay / 16) * 16
									/**/ + key;
									g_editor.pattern_char = 0;
								}
							}
							break;
					}
				}
			}
			/// CLAMP CURSOR
			g_editor.pattern_clamp_cursor();
		}
	}
}

void TrackerWidget::onHoverScroll(const HoverScrollEvent &e) {
	if (g_editor.selected == false)
		return;
	if (g_editor.pattern) {
		/// SCROLL X
		if (APP->window->getMods() & GLFW_MOD_SHIFT) {
			/// MOVE CURSOR
			if (e.scrollDelta.y > 0)
				g_editor.pattern_move_cursor_x(-1);
			else
				g_editor.pattern_move_cursor_x(+1);
		/// SCROLL Y
		} else {
			/// MOVE CURSOR
			if (e.scrollDelta.y > 0)
				g_editor.pattern_move_cursor_y(-1);
			else
				g_editor.pattern_move_cursor_y(+1);
		}
	}
	e.consume(this);
}

void TrackerWidget::onSelect(const SelectEvent &e) {
	g_editor.selected = true;
	this->module->lights[0].setBrightness(1.0);
}

void TrackerWidget::onDeselect(const DeselectEvent &e) {
	g_editor.selected = false;
	this->module->lights[0].setBrightness(0.0);
}

//void TrackerWidget::onDragStart(const DragStartEvent& e) {
//	APP->window->cursorLock();
//}
//void TrackerWidget::onDragMove(const DragMoveEvent& e) {
//	/// NOT CALLED = NOT MOVED
//	ModuleWidget::onDragMove(e);
//}
//void TrackerWidget::onDragEnd(const DragEndEvent& e) {
//	APP->window->cursorUnlock();
//}
