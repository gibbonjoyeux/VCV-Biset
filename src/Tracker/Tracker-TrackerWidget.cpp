
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static void set_scale(float *table) {
	float	value;
	int		i;

	for (i = 0; i < 12; ++i) {
		value = table[i] / 100.0;
		g_module->params[Tracker::PARAM_TEMPERAMENT + i].setValue(value);
	}
}

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

static bool event_key_pattern(const Widget::SelectKeyEvent &e) {
	PatternSource	*pattern;
	PatternNoteRow	*row_note;
	PatternNote		*line_note;
	PatternEffect	*effect;
	PatternCVRow	*row_cv;
	PatternCV		*line_cv;
	int				fx_1, fx_2;
	int				key;
	int				i;

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
				if ((e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL)
					return false;
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
								if (line_note->mode == PATTERN_NOTE_NEW)
									line_note->mode = PATTERN_NOTE_KEEP;
								else if (line_note->mode == PATTERN_NOTE_KEEP)
									line_note->mode = PATTERN_NOTE_STOP;
								else if (line_note->mode == PATTERN_NOTE_STOP)
									line_note->mode = PATTERN_NOTE_KEEP;
								else if (line_note->mode == PATTERN_NOTE_GLIDE)
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
										/**/ key * 100
										/**/ + line_cv->value % 100;
										g_editor.pattern_char += 1;
									} else if (g_editor.pattern_char == 1) {
										line_cv->value =
										/**/ (line_cv->value / 100) * 100
										/**/ + key * 10
										/**/ + (line_cv->value % 10);
										g_editor.pattern_char += 1;
									} else {
										line_cv->value =
										/**/ (line_cv->value / 10) * 10
										/**/ + key;
										g_editor.pattern_move_cursor_y(1);
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
	return true;
}

static bool event_key_timeline(const Widget::SelectKeyEvent &e) {
	TimelineCell	*cell;
	int				key;

	if (e.action == GLFW_PRESS
	|| e.action == GLFW_REPEAT) {
		/// EVENT CURSOR MOVE
		if (e.key == GLFW_KEY_LEFT) {
			g_editor.timeline_move_cursor_x(-1);
		} else if (e.key == GLFW_KEY_RIGHT) {
			g_editor.timeline_move_cursor_x(+1);
		} else if (e.key == GLFW_KEY_UP) {
			g_editor.timeline_move_cursor_y(-1);
		} else if (e.key == GLFW_KEY_DOWN) {
			g_editor.timeline_move_cursor_y(+1);
		/// EVENT KEYBOARD
		} else {
			if ((e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL)
				return false;
			cell = &(g_timeline.timeline
			/**/ [g_editor.timeline_column]
			/**/ [g_editor.timeline_line]);
			/// CELL PATTERN
			if (g_editor.timeline_cell == 0) {
				/// PATTERN DELETE
				if (e.key == GLFW_KEY_DELETE
				|| e.key == GLFW_KEY_BACKSPACE) {
					if (cell->mode == TIMELINE_CELL_NEW)
						cell->mode = TIMELINE_CELL_KEEP;
					else if (cell->mode == TIMELINE_CELL_KEEP)
						cell->mode = TIMELINE_CELL_STOP;
					else if (cell->mode == TIMELINE_CELL_STOP)
						cell->mode = TIMELINE_CELL_KEEP;
				/// PATTERN EDIT
				} else {
					key = key_dec(e);
					if (key >= 0) {
						if (cell->mode != PATTERN_NOTE_NEW) {
							cell->mode = PATTERN_NOTE_NEW;
							cell->beat = 0;
						}
						if (g_editor.timeline_char == 0) {
							cell->pattern =
							/**/ key * 100
							/**/ + cell->pattern % 100;
							g_editor.timeline_char += 1;
						} else if (g_editor.timeline_char == 1) {
							cell->pattern =
							/**/ (cell->pattern / 100) * 100
							/**/ + key * 10
							/**/ + (cell->pattern % 10);
							g_editor.timeline_char += 1;
						} else {
							cell->pattern =
							/**/ (cell->pattern / 10) * 10
							/**/ + key;
							g_editor.timeline_move_cursor_y(1);
						}
					}
				}
			/// CELL BEAT
			} else {
				/// PATTERN DELETE
				if (e.key == GLFW_KEY_DELETE
				|| e.key == GLFW_KEY_BACKSPACE) {
					cell->beat = 0;
				/// PATTERN EDIT
				} else {
					key = key_dec(e);
					if (key >= 0) {
						if (g_editor.timeline_char == 0) {
							cell->beat =
							/**/ key * 100
							/**/ + cell->beat % 100;
							g_editor.timeline_char += 1;
						} else if (g_editor.timeline_char == 1) {
							cell->beat =
							/**/ (cell->beat / 100) * 100
							/**/ + key * 10
							/**/ + (cell->beat % 10);
							g_editor.timeline_char += 1;
						} else {
							cell->beat =
							/**/ (cell->beat / 10) * 10
							/**/ + key;
							g_editor.timeline_move_cursor_y(1);
						}
					}
				}
			}
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerWidget::TrackerWidget(Tracker* _module) {
	TrackerDisplay			*display;
	TrackerBPMDisplay		*display_bpm;
	TrackerSynthDisplay		*display_synth;
	TrackerPatternDisplay	*display_pattern;
	TrackerEditDisplay		*display_edit;
	LedDisplayDigit			*display_jump;
	LedDisplayDigit			*display_octave;
	int						i;

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
	///**/ createParamCentered<KnobMedium>(mm2px(Vec(9.0, 24.0)),
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(20.0, 15.0)),
	/**/ module,
	/**/ Tracker::PARAM_BPM));
	addParam(createParamCentered<KnobMedium>(mm2px(Vec(20.0, 25.0)),
	/**/ module,
	/**/ Tracker::PARAM_SYNTH));
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(20.0, 35.0)),
	/**/ module,
	/**/ Tracker::PARAM_PATTERN));

	////// EDIT KNOBS
	//for (i = 0; i < 9; ++i) {
	//	addParam(
	//	///**/ createParamCentered<Trimpot>(mm2px(Vec(40.0, 73.0 + 8.75 * i - 37.5)),
	//	/**/ createParamCentered<KnobSmall>(mm2px(Vec(40.0, 73.0 + 7.80 * i - 37.5)),
	//	/**/ module,
	//	/**/ Tracker::PARAM_EDIT + i));
	//}
	////// EDIT BUTTON
	//addParam(
	///**/ createParamCentered<VCVButton>(mm2px(Vec(20.5, 106.0)),
	///**/ module,
	///**/ Tracker::PARAM_EDIT_SAVE));

	//// JUMP BUTTONS
	addParam(
	/**/ createParamCentered<TL1105>(mm2px(Vec(41.0, 12.5 + 10.0)),
	/**/ module,
	/**/ Tracker::PARAM_JUMP_UP));
	addParam(
	/**/ createParamCentered<TL1105>(mm2px(Vec(41.0, 17.5 + 10.0)),
	/**/ module,
	/**/ Tracker::PARAM_JUMP_DOWN));
	//// OCTAVE BUTTONS
	addParam(
	/**/ createParamCentered<TL1105>(mm2px(Vec(47.0, 12.5 + 10.0)),
	/**/ module,
	/**/ Tracker::PARAM_OCTAVE_UP));
	addParam(
	/**/ createParamCentered<TL1105>(mm2px(Vec(47.0, 17.5 + 10.0)),
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
	/**/ createOutputCentered<Outlet>(mm2px(Vec(237.0, 123.00)),
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
	display_bpm = createWidget<TrackerBPMDisplay>(mm2px(Vec(5.0, 13.0)));
	display_bpm->box.size = mm2px(Vec(8.25, 3.5));
	display_bpm->module = module;
	display_bpm->moduleWidget = this;
	addChild(display_bpm);
	//// SYNTH LED DISPLAY
	display_synth = createWidget<TrackerSynthDisplay>(mm2px(Vec(5.0, 23.0)));
	display_synth->box.size = mm2px(Vec(8.25, 3.5));
	display_synth->module = module;
	display_synth->moduleWidget = this;
	addChild(display_synth);
	//// PATTERN LED DISPLAY
	display_pattern = createWidget<TrackerPatternDisplay>(mm2px(Vec(5.0, 33.0)));
	display_pattern->box.size = mm2px(Vec(8.25, 3.5));
	display_pattern->module = module;
	display_pattern->moduleWidget = this;
	addChild(display_pattern);

	//// SELECT JUMP DISPLAY
	display_jump = createWidget<LedDisplayDigit>(mm2px(Vec(5.0, 43.0)));
	display_jump->box.size = mm2px(Vec(8.25, 3.5));
	display_jump->module = module;
	display_jump->value_link = &(g_editor.pattern_jump);
	display_jump->value_length = 2;
	display_jump->color_back = colors[15];
	display_jump->color_font = colors[4];
	addChild(display_jump);
	//// SELECT OCTAVE DISPLAY
	display_octave = createWidget<LedDisplayDigit>(mm2px(Vec(15.0, 43.0)));
	display_octave->box.size = mm2px(Vec(8.25, 3.5));
	display_octave->module = module;
	display_octave->value_link = &(g_editor.pattern_octave);
	display_octave->value_length = 2;
	display_octave->color_back = colors[15];
	display_octave->color_font = colors[4];
	addChild(display_octave);
	

	//// EDIT LED DISPLAY
	// MODE FULL SCREEN
	//display_edit = createWidget<TrackerEditDisplay>(mm2px(Vec(16.0, 5.0)));
	//display_edit->box.size = Vec(CHAR_W * 16 + 4, CHAR_H * CHAR_COUNT_Y + 5.5);
	// MODE SIDE SCREEN
	//display_edit = createWidget<TrackerEditDisplay>(mm2px(Vec(5.0, 70.5 - 38.0)));
	//display_edit->box.size = mm2px(Vec(29.5, 68.0));
	//display_edit->module = module;
	//display_edit->moduleWidget = this;
	//addChild(display_edit);
}

void TrackerWidget::onSelectKey(const SelectKeyEvent &e) {
	if (g_editor.mode == EDITOR_MODE_PATTERN) {
		if (event_key_pattern(e))
			e.consume(this);
	} else if (g_editor.mode == EDITOR_MODE_TIMELINE) {
		if (event_key_timeline(e))
			e.consume(this);
	}
}

void TrackerWidget::onHoverScroll(const HoverScrollEvent &e) {
	//if (g_editor.selected == false)
	//	return;
	//if (g_editor.pattern) {
	//	/// SCROLL X
	//	if (APP->window->getMods() & GLFW_MOD_SHIFT) {
	//		/// MOVE CURSOR
	//		if (e.scrollDelta.y > 0)
	//			g_editor.pattern_move_cursor_x(-1);
	//		else
	//			g_editor.pattern_move_cursor_x(+1);
	//	/// SCROLL Y
	//	} else {
	//		/// MOVE CURSOR
	//		if (e.scrollDelta.y > 0)
	//			g_editor.pattern_move_cursor_y(-1);
	//		else
	//			g_editor.pattern_move_cursor_y(+1);
	//	}
	//}
	//e.consume(this);
}

void TrackerWidget::onSelect(const SelectEvent &e) {
	g_editor.selected = true;
	//this->module->lights[0].setBrightness(1.0);
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
	//this->module->lights[0].setBrightness(0.0);
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

void TrackerWidget::appendContextMenu(Menu *menu) {
	MenuSeparator	*separator;
	Param			*param_pitch;
	Param			*param_rate;

	param_pitch = &(g_module->params[Tracker::PARAM_PITCH_BASE]);
	param_rate = &(g_module->params[Tracker::PARAM_RATE]);

	separator = new MenuSeparator();
	menu->addChild(separator);

	/// [1] BASE PITCH
	menu->addChild(rack::createSubmenuItem("Pitch offset", "",
		[=](Menu *menu) {
			MenuSliderEdit	*slider;
			MenuItem		*item;
			rack::Widget	*holder;

			/// SLIDER
			slider = new MenuSliderEdit(g_module->paramQuantities[Tracker::PARAM_PITCH_BASE]);
			slider->box.size.x = 200.f;
			menu->addChild(slider);

			/// PRESETS
			//// HOLDER
			holder = new rack::Widget();
			holder->box.size.x = 200.0f;
			holder->box.size.y = 20.0f;
			//// OCT -2
			item = new MenuItemStay("-2", "",	
				[=]() { param_pitch->setValue(-2); }
			);
			item->box.size.x = 30.0f;
			item->box.size.y = 20.0f;
			holder->addChild(item);
			//// OCT -1
			item = new MenuItemStay("-1", "",
				[=]() { param_pitch->setValue(-1); }
			);
			item->box.size.x = 30.0f;
			item->box.size.y = 20.0f;
			item->box.pos.x = 40.0f;
			holder->addChild(item);
			//// OCT 0
			item = new MenuItemStay("0", "",
				[=]() { param_pitch->setValue(0); }
			);
			item->box.size.x = 30.0f;
			item->box.size.y = 20.0f;
			item->box.pos.x = 80.0f;
			holder->addChild(item);
			//// OCT +1
			item = new MenuItemStay("+1", "",
				[=]() { param_pitch->setValue(+1); }
			);
			item->box.size.x = 30.0f;
			item->box.size.y = 20.0f;
			item->box.pos.x = 120.0f;
			holder->addChild(item);
			//// OCT +2
			item = new MenuItemStay("+2", "",
				[=]() { param_pitch->setValue(+2); }
			);
			item->box.size.x = 30.0f;
			item->box.size.y = 20.0f;
			item->box.pos.x = 158.0f;
			holder->addChild(item);
			menu->addChild(holder);
		}
	));

	/// [2] RATE
	menu->addChild(rack::createSubmenuItem("Rate", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Sample rate / 1", "",
				[=]() { return param_rate->getValue() == 1; },
				[=]() { param_rate->setValue(1); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 2", "",
				[=]() { return param_rate->getValue() == 2; },
				[=]() { param_rate->setValue(2); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 4", "",
				[=]() { return param_rate->getValue() == 4; },
				[=]() { param_rate->setValue(4); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 8", "",
				[=]() { return param_rate->getValue() == 8; },
				[=]() { param_rate->setValue(8); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 16", "",
				[=]() { return param_rate->getValue() == 16; },
				[=]() { param_rate->setValue(16); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 32", "",
				[=]() { return param_rate->getValue() == 32; },
				[=]() { param_rate->setValue(32); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 64", "default",
				[=]() { return param_rate->getValue() == 64; },
				[=]() { param_rate->setValue(64); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 128", "",
				[=]() { return param_rate->getValue() == 128; },
				[=]() { param_rate->setValue(128); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 256", "",
				[=]() { return param_rate->getValue() == 256; },
				[=]() { param_rate->setValue(256); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 512", "",
				[=]() { return param_rate->getValue() == 512; },
				[=]() { param_rate->setValue(512); }
			));
		}
	));

	/// [3] TEMPERAMENT
	menu->addChild(rack::createSubmenuItem("Temperament", "",
		[=](Menu *menu) {
			MenuSliderEdit	*slider;
			int				i;

			/// PRESETS
			//// REGULAR PRESETS
			menu->addChild(rack::createSubmenuItem("Presets temperament", "",
				[=](Menu *menu) {
					menu->addChild(new MenuItemStay("Equal", "default",
						[=]() { set_scale(table_temp_equal); }
					));
					menu->addChild(new MenuItemStay("Just", "",
						[=]() { set_scale(table_temp_just); }
					));
					menu->addChild(new MenuItemStay("Pythagorean", "",
						[=]() { set_scale(table_temp_pyth); }
					));
					menu->addChild(new MenuItemStay("Wendy Carlos Super Just", "",
						[=]() { set_scale(table_temp_carlos_super_just); }
					));
					menu->addChild(new MenuItemStay("Wendy Carlos Harmonic", "",
						[=]() { set_scale(table_temp_carlos_harmonic); }
					));
					menu->addChild(new MenuItemStay("Kirnberger", "",
						[=]() { set_scale(table_temp_kirnberger); }
					));
					menu->addChild(new MenuItemStay("Vallotti Young", "",
						[=]() { set_scale(table_temp_vallotti_young); }
					));
					menu->addChild(new MenuItemStay("Werckmeister III", "",
						[=]() { set_scale(table_temp_werckmeister_3); }
					));
				}
			));
			//// SCALE PRESETS
			menu->addChild(rack::createSubmenuItem("Presets scale", "",
				[=](Menu *menu) {
					menu->addChild(new MenuItemStay("Diatonic", "default",
						[=]() {
						}
					));
					menu->addChild(new MenuItemStay("Major", "",
						[=]() {
						}
					));
					menu->addChild(new MenuItemStay("Minor", "",
						[=]() {
						}
					));
				}
			));
			/// MANUAL
			for (i = 0; i < 12; ++i) {
				slider = new MenuSliderEdit(g_module->paramQuantities[Tracker::PARAM_TEMPERAMENT + i]);
				slider->box.size.x = 200.f;
				menu->addChild(slider);
			}
		}
	));

}
