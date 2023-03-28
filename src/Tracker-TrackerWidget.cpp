
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
	midi += g_editor.pattern_octave * 12;
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

TrackerWidget::TrackerWidget(Tracker* _module) {
	TrackerDisplay*		display;
	TrackerInfoDisplay*	display_bpm;
	TrackerEditDisplay*	display_edit;
	int					i;

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
	//// PLAY BUTTONS
	addParam(
	/**/ createParamCentered<VCVButton>(mm2px(Vec(8.0, 6.0)),
	/**/ module,
	/**/ Tracker::PARAM_PLAY_SONG));
	addParam(
	/**/ createParamCentered<VCVButton>(mm2px(Vec(8.0 + 8.0 * 1, 6.0)),
	/**/ module,
	/**/ Tracker::PARAM_PLAY_PATTERN));
	addParam(
	/**/ createParamCentered<VCVButton>(mm2px(Vec(8.0 + 8.0 * 2, 6.0)),
	/**/ module,
	/**/ Tracker::PARAM_PLAY));
	addParam(
	/**/ createParamCentered<VCVButton>(mm2px(Vec(8.0 + 8.0 * 3, 6.0)),
	/**/ module,
	/**/ Tracker::PARAM_STOP));

	//// MODE BUTTON
	//// VIEW LIGHT SWITCHES
	for (i = 0; i < 3; ++i) {
		addParam(
		/**/ createParamCentered<LEDButton>(mm2px(Vec(55.0 + 8.0 * i, 123.7)),
		/**/ module,
		/**/ Tracker::PARAM_MODE + i));
		addChild(
		/**/ createLightCentered<LargeLight<YellowLight>>(mm2px(Vec(55.0 + 8.0 * i, 123.7)),
		/**/ module,
		/**/ Tracker::LIGHT_MODE + i));
	}

	//// BPM / SYNTH / PATTERN KNOBS
	addParam(
	/**/ createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(9.0, 24.0)),
	/**/ module,
	/**/ Tracker::PARAM_BPM));
	addParam(
	/**/ createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(9.0 + 11, 24.0)),
	/**/ module,
	/**/ Tracker::PARAM_SYNTH));
	addParam(
	/**/ createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(9.0 + 22, 24.0)),
	/**/ module,
	/**/ Tracker::PARAM_PATTERN));

	//// EDIT KNOBS
	for (i = 0; i < 8; ++i) {
		addParam(
		///**/ createParamCentered<Trimpot>(mm2px(Vec(40.0, 73.0 + 8.75 * i - 37.5)),
		/**/ createParamCentered<Trimpot>(mm2px(Vec(40.0, 73.0 + 6.00 * i - 37.5)),
		/**/ module,
		/**/ Tracker::PARAM_EDIT + i));
	}
	//// EDIT BUTTON
	addParam(
	/**/ createParamCentered<VCVButton>(mm2px(Vec(20.5, 106.0)),
	/**/ module,
	/**/ Tracker::PARAM_EDIT_SAVE));

	//// JUMP BUTTONS
	addParam(
	/**/ createParamCentered<TL1105>(mm2px(Vec(41.0, 12.5)),
	/**/ module,
	/**/ Tracker::PARAM_JUMP_UP));
	addParam(
	/**/ createParamCentered<TL1105>(mm2px(Vec(41.0, 18.5)),
	/**/ module,
	/**/ Tracker::PARAM_JUMP_DOWN));
	//// OCTAVE BUTTONS
	addParam(
	/**/ createParamCentered<TL1105>(mm2px(Vec(47.0, 12.5)),
	/**/ module,
	/**/ Tracker::PARAM_OCTAVE_UP));
	addParam(
	/**/ createParamCentered<TL1105>(mm2px(Vec(47.0, 18.5)),
	/**/ module,
	/**/ Tracker::PARAM_OCTAVE_DOWN));

	//// VIEW LIGHT SWITCHES
	for (i = 0; i < 5; ++i) {
		addParam(
		/**/ createParamCentered<LEDButton>(mm2px(Vec(90.0 + 8.0 * i, 123.7)),
		/**/ module,
		/**/ Tracker::PARAM_VIEW + i));
		addChild(
		/**/ createLightCentered<SmallLight<YellowLight>>(mm2px(Vec(90.0 + 8.0 * i, 123.7)),
		/**/ module,
		/**/ Tracker::LIGHT_VIEW + i));
	}

	/// [2] ADD OUTPUT
	addOutput(
	/**/ createOutputCentered<PJ301MPort>(mm2px(Vec(237.0, 123.00)),
	/**/ module,
	/**/ Tracker::OUTPUT_CLOCK));

	/// [3] ADD LIGHTS
	addChild(
	/**/ createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(241.25, 3.0)),
	/**/ module,
	/**/ Tracker::LIGHT_FOCUS));
	addChild(
	/**/ createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(241.25, 7.5)),
	/**/ module,
	/**/ Tracker::LIGHT_PLAY));

	/// [4] ADD DISPLAYS
	//// MAIN LED DISPLAY
	display = createWidget<TrackerDisplay>(mm2px(Vec(65.50 - 14.0, 5.0)));
	//display->box.size = mm2px(Vec(173.5 + 14.0, 94.5 + 15.0));
	display->box.size = Vec(CHAR_W * (CHAR_COUNT_X + 3) + 4, CHAR_H * CHAR_COUNT_Y + 5.5);
	display->module = module;
	display->moduleWidget = this;
	addChild(display);
	//// BPM LED DISPLAY
	display_bpm = createWidget<TrackerInfoDisplay>(mm2px(Vec(5.0, 13.0)));
	display_bpm->box.size = mm2px(Vec(1.0, 1.0));
	display_bpm->module = module;
	display_bpm->moduleWidget = this;
	addChild(display_bpm);
	//// EDIT LED DISPLAY
	display_edit = createWidget<TrackerEditDisplay>(mm2px(Vec(5.0, 70.5 - 38.0)));
	display_edit->box.size = mm2px(Vec(29.5, 68.0));
	display_edit->module = module;
	display_edit->moduleWidget = this;
	addChild(display_edit);
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
									line_note->synth = g_editor.synth_id;
									if (line_note->mode == PATTERN_NOTE_KEEP
									|| line_note->mode == PATTERN_NOTE_STOP) {
										line_note->mode = PATTERN_NOTE_NEW;
										line_note->velocity = 99;
										line_note->panning = 50;
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
							key = key_dec(e);
							if (key >= 0 && key <= 9) {
								line_note->pitch =
								/**/ line_note->pitch % 12
								/**/ + key * 12;
							}
							break;
						/// VELOCITY
						case 2:
							key = key_dec(e);
							if (key >= 0) {
								if (g_editor.pattern_char == 0) {
									line_note->velocity =
									/**/ line_note->velocity % 10
									/**/ + key * 10;
									g_editor.pattern_char += 1;
								} else {
									line_note->velocity =
									/**/ (line_note->velocity / 10) * 10
									/**/ + key;
									g_editor.pattern_move_cursor_y(1);
								}
							}
							break;
						/// PANNING
						case 3:
							key = key_dec(e);
							if (key >= 0) {
								if (g_editor.pattern_char == 0) {
									line_note->panning =
									/**/ line_note->panning % 10
									/**/ + key * 10;
									g_editor.pattern_char += 1;
								} else {
									line_note->panning =
									/**/ (line_note->panning / 10) * 10
									/**/ + key;
									g_editor.pattern_move_cursor_y(1);
								}
							}
							break;
						/// SYNTH
						case 4:
							key = key_dec(e);
							if (key >= 0) {
								if (g_editor.pattern_char == 0) {
									line_note->synth =
									/**/ line_note->synth % 10
									/**/ + key * 10;
									g_editor.pattern_char += 1;
								} else {
									line_note->synth =
									/**/ (line_note->synth / 10) * 10
									/**/ + key;
									g_editor.pattern_move_cursor_y(1);
								}
							}
							break;
						/// DELAY
						case 5:
							key = key_dec(e);
							if (key >= 0) {
								if (g_editor.pattern_char == 0) {
									line_note->delay =
									/**/ line_note->delay % 10
									/**/ + key * 10;
									g_editor.pattern_char += 1;
								} else {
									line_note->delay =
									/**/ (line_note->delay / 10) * 10
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
								key = key_dec(e);
								if (key >= 0) {
									if (line_note->mode == PATTERN_NOTE_NEW)
										line_note->mode = PATTERN_NOTE_GLIDE;
									if (g_editor.pattern_char == 0) {
										line_note->glide =
										/**/ line_note->glide % 10
										/**/ + key * 10;
										g_editor.pattern_char += 1;
									} else {
										line_note->glide =
										/**/ (line_note->glide / 10) * 10
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
									key = key_dec(e);
									if (key >= 0) {
										if (g_editor.pattern_char == 0) {
											effect->value =
											/**/ effect->value % 10
											/**/ + key * 10;
											g_editor.pattern_char += 1;
										} else {
											effect->value =
											/**/ (effect->value / 10) * 10
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
								key = key_dec(e);
								if (key >= 0) {
									if (line_cv->mode == PATTERN_CV_KEEP)
										line_cv->mode = PATTERN_CV_SET;
									if (g_editor.pattern_char == 0) {
										line_cv->value =
										/**/ line_cv->value % 10
										/**/ + key * 10;
										g_editor.pattern_char += 1;
									} else {
										line_cv->value =
										/**/ (line_cv->value / 10) * 10
										/**/ + key;
										g_editor.pattern_char = 0;
									}
								}
							}
							break;
						/// GLIDE
						case 1:
							key = key_dec(e);
							if (key >= 0) {
								if (g_editor.pattern_char == 0) {
									line_cv->glide =
									/**/ line_cv->glide % 10
									/**/ + key * 10;
									g_editor.pattern_char += 1;
								} else {
									line_cv->glide =
									/**/ (line_cv->glide / 10) * 10
									/**/ + key;
									g_editor.pattern_char = 0;
								}
							}
							break;
						/// DELAY
						case 2:
							key = key_dec(e);
							if (key >= 0) {
								if (g_editor.pattern_char == 0) {
									line_cv->delay =
									/**/ line_cv->delay % 10
									/**/ + key * 10;
									g_editor.pattern_char += 1;
								} else {
									line_cv->delay =
									/**/ (line_cv->delay / 10) * 10
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
	//ParamWidget	*pw;
	//
	//pw = APP->scene->rack->touchedParam;
	//if (pw && pw->module == this->module) {
	//	APP->event->setSelectedWidget(this);
	//	return;
	//}
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
