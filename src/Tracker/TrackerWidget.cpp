
#include "Tracker.hpp"

#define BTN_PLAY_X			(5.2 + 1.0)
#define BTN_PLAY_Y			6.0
#define BTN_PLAY_STEP		6.0
#define KNOB_X				(10.0 + 1.0)
#define KNOB_Y				18.0
#define BTN_JUMP_X			(4.75 + 1.0)
#define BTN_JUMP_Y			40.0
#define BTN_OCTAVE_X		(BTN_JUMP_X + 12.25)
#define BTN_OCTAVE_Y		BTN_JUMP_Y
#define DISPLAY_X			31.25				// 51.5
#define DISPLAY_Y			5.0
#define DISPLAY_SIDE_X		(DISPLAY_X + 192.5)	// 244
#define DISPLAY_SIDE_Y		5.0

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static void set_temperament(float *table) {
	float	value;
	float	note;
	int		note_round;
	int		i;

	for (i = 0; i < 12; ++i) {
		note = g_module->params[Tracker::PARAM_TUNING + i].getValue();
		note_round = (int)note;
		if (note - (float)note_round >= 0.5) {
			if (note_round < 11)
				value = table[note_round + 1] / 100.0;
			else
				value = table[0] / 100.0;
		} else {
			value = table[note_round] / 100.0;
		}
		g_module->params[Tracker::PARAM_TUNING + i].setValue(value);
	}
}

static void set_scale(float *table) {
	float	value;
	int		i;

	for (i = 0; i < 12; ++i) {
		value = table[i] / 100.0;
		g_module->params[Tracker::PARAM_TUNING + i].setValue(value);
	}
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerWidget::TrackerWidget(Tracker* _module) {
	TrackerDisplay			*display;
	TrackerDisplaySide		*display_side;
	//TrackerDisplayBPM		*display_bpm;
	//TrackerDisplaySynth		*display_synth;
	//TrackerDisplayPattern	*display_pattern;
	LedDisplayDigit			*display_bpm;
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
	/**/ createParamCentered<ButtonPlaySong>(mm2px(Vec(BTN_PLAY_X, BTN_PLAY_Y)),
	/**/ module,
	/**/ Tracker::PARAM_PLAY_SONG));
	addParam(
	/**/ createParamCentered<ButtonPlayPattern>(mm2px(Vec(BTN_PLAY_X + BTN_PLAY_STEP, BTN_PLAY_Y)),
	/**/ module,
	/**/ Tracker::PARAM_PLAY_PATTERN));
	addParam(
	/**/ createParamCentered<ButtonPlaySimple>(mm2px(Vec(BTN_PLAY_X + BTN_PLAY_STEP * 2, BTN_PLAY_Y)),
	/**/ module,
	/**/ Tracker::PARAM_PLAY));
	addParam(
	/**/ createParamCentered<ButtonStop>(mm2px(Vec(BTN_PLAY_X + BTN_PLAY_STEP * 3, BTN_PLAY_Y)),
	/**/ module,
	/**/ Tracker::PARAM_STOP));

	//// EDITOR MODE SWITCHES
	addParam(
	/**/ createParamCentered<ButtonViewPattern>(mm2px(Vec(BTN_PLAY_X, 108)),
	/**/ module,
	/**/ Tracker::PARAM_MODE_PATTERN));
	addParam(
	/**/ createParamCentered<ButtonViewTimeline>(mm2px(Vec(BTN_PLAY_X + BTN_PLAY_STEP, 108)),
	/**/ module,
	/**/ Tracker::PARAM_MODE_TIMELINE));
	addParam(
	/**/ createParamCentered<ButtonViewMatrix>(mm2px(Vec(BTN_PLAY_X + BTN_PLAY_STEP * 2, 108)),
	/**/ module,
	/**/ Tracker::PARAM_MODE_MATRIX));
	addParam(
	/**/ createParamCentered<ButtonViewTuning>(mm2px(Vec(BTN_PLAY_X + BTN_PLAY_STEP * 3, 108)),
	/**/ module,
	/**/ Tracker::PARAM_MODE_TUNING));

	//// PATTERN VIEW MODE SWITCHES
	for (i = 0; i < 5; ++i) {
		addParam(
		/**/ createParamCentered<ButtonSwitch>(mm2px(Vec(90.0 + 7.0 * i, 123.7)),
		/**/ module,
		/**/ Tracker::PARAM_VIEW + i));
	}

	/// [3] ADD DISPLAYS
	//// MAIN LED DISPLAY
	display = createWidget<TrackerDisplay>(mm2px(Vec(DISPLAY_X, DISPLAY_Y)));
	//display->box.size = mm2px(Vec(173.5 + 14.0, 94.5 + 15.0));
	display->box.size = Vec(CHAR_W * (CHAR_COUNT_X + 3) + 4, CHAR_H * CHAR_COUNT_Y + 5.5);
	display->module = module;
	display->moduleWidget = this;
	this->display = display;
	addChild(display);

	//// SIDE LED DISPLAY
	display_side = createWidget<TrackerDisplaySide>(mm2px(Vec(DISPLAY_SIDE_X, DISPLAY_SIDE_Y)));
	display_side->box.size = Vec(CHAR_W * 26, CHAR_H * CHAR_COUNT_Y + 5.5);
	display_side->module = module;
	display_side->moduleWidget = this;
	this->display_side = display_side;
	addChild(display_side);

	//// BPM / SYNTH / PATTERN KNOBS
	/// BPM SELECTOR
	//// DISPLAY
	display_bpm = createWidget<LedDisplayDigit>(mm2px(Vec(KNOB_X, KNOB_Y)));
	display_bpm->box.size = mm2px(Vec(8.25, 3.5));
	display_bpm->module = module;
	if (module)
		display_bpm->value_quant = module->paramQuantities[Tracker::PARAM_BPM];
	display_bpm->value_length = 3;
	display_bpm->color_back = colors[15];
	display_bpm->color_font = colors[4];
	addChild(display_bpm);
	//display_bpm = createWidget<TrackerDisplayBPM>(mm2px(Vec(KNOB_X, KNOB_Y)));
	//display_bpm->box.size = mm2px(Vec(8.25, 3.5));
	//display_bpm->module = module;
	//if (module)
	//	display_bpm->value_quant = module->paramQuantities[Tracker::PARAM_BPM];
	//display_bpm->value_length = 3;
	//display_bpm->color_back = colors[15];
	//display_bpm->color_font = colors[4];
	//addChild(display_bpm);
	//// KNOB
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(KNOB_X + 4.25, KNOB_Y + 10.0)),
	/**/ module,
	/**/ Tracker::PARAM_BPM));

	/// SELECT JUMP
	//// DISPLAY
	display_jump = createWidget<LedDisplayDigit>(mm2px(Vec(BTN_JUMP_X, BTN_JUMP_Y)));
	display_jump->box.size = mm2px(Vec(8.25, 3.5));
	display_jump->module = module;
	display_jump->value_link = &(g_editor->pattern_jump);
	display_jump->value_length = 2;
	display_jump->color_back = colors[15];
	display_jump->color_font = colors[4];
	addChild(display_jump);
	//// BUTTONS
	addParam(
	/**/ createParamCentered<ButtonPlus>(mm2px(Vec(BTN_JUMP_X + 4.0, BTN_JUMP_Y + 8.0)),
	/**/ module,
	/**/ Tracker::PARAM_JUMP_UP));
	addParam(
	/**/ createParamCentered<ButtonMinus>(mm2px(Vec(BTN_JUMP_X + 4.0, BTN_JUMP_Y + 14.0)),
	/**/ module,
	/**/ Tracker::PARAM_JUMP_DOWN));
	/// SELECT OCTAVE
	//// DISPLAY
	display_octave = createWidget<LedDisplayDigit>(mm2px(Vec(BTN_OCTAVE_X, BTN_OCTAVE_Y)));
	display_octave->box.size = mm2px(Vec(8.25, 3.5));
	display_octave->module = module;
	display_octave->value_link = &(g_editor->pattern_octave);
	display_octave->value_length = 2;
	display_octave->color_back = colors[15];
	display_octave->color_font = colors[4];
	addChild(display_octave);
	//// BUTTONS
	addParam(
	/**/ createParamCentered<ButtonPlus>(mm2px(Vec(BTN_OCTAVE_X + 4.0, BTN_OCTAVE_Y + 8.0)),
	/**/ module,
	/**/ Tracker::PARAM_OCTAVE_UP));
	addParam(
	/**/ createParamCentered<ButtonMinus>(mm2px(Vec(BTN_OCTAVE_X + 4.0, BTN_OCTAVE_Y + 14.0)),
	/**/ module,
	/**/ Tracker::PARAM_OCTAVE_DOWN));
}

void TrackerWidget::onSelectKey(const SelectKeyEvent &e) {

	if (g_module == NULL)
		return;

	/// CHANGE VIEW
	if (e.action == GLFW_PRESS && (e.mods & GLFW_MOD_SHIFT)) {
		if (e.key == GLFW_KEY_LEFT) {
			g_module->params[Tracker::PARAM_MODE_PATTERN].setValue(1);
			g_module->params[Tracker::PARAM_MODE_TIMELINE].setValue(0);
			g_module->params[Tracker::PARAM_MODE_MATRIX].setValue(0);
			g_module->params[Tracker::PARAM_MODE_TUNING].setValue(0);
			e.consume(this);
			return;
		} else if (e.key == GLFW_KEY_RIGHT) {
			g_module->params[Tracker::PARAM_MODE_PATTERN].setValue(0);
			g_module->params[Tracker::PARAM_MODE_TIMELINE].setValue(1);
			g_module->params[Tracker::PARAM_MODE_MATRIX].setValue(0);
			g_module->params[Tracker::PARAM_MODE_TUNING].setValue(0);
			e.consume(this);
			return;
		} else if (e.key == GLFW_KEY_UP) {
			g_module->params[Tracker::PARAM_MODE_PATTERN].setValue(0);
			g_module->params[Tracker::PARAM_MODE_TIMELINE].setValue(0);
			g_module->params[Tracker::PARAM_MODE_MATRIX].setValue(1);
			g_module->params[Tracker::PARAM_MODE_TUNING].setValue(0);
			e.consume(this);
			return;
		} else if (e.key == GLFW_KEY_DOWN) {
			g_module->params[Tracker::PARAM_MODE_PATTERN].setValue(0);
			g_module->params[Tracker::PARAM_MODE_TIMELINE].setValue(0);
			g_module->params[Tracker::PARAM_MODE_MATRIX].setValue(0);
			g_module->params[Tracker::PARAM_MODE_TUNING].setValue(1);
			e.consume(this);
			return;
		}
	}
	/// EDIT PATTERN & TIMELINE
	if (g_editor->mode == EDITOR_MODE_PATTERN) {
		this->display->on_key_pattern(e);
	} else if (g_editor->mode == EDITOR_MODE_TIMELINE) {
		this->display->on_key_timeline(e);
	}
	e.consume(this);
}

void TrackerWidget::onHoverScroll(const HoverScrollEvent &e) {

	if (g_module == NULL)
		return;

	ModuleWidget::onHoverScroll(e);
	//if (g_editor->selected == false)
	//	return;
	//if (g_editor->pattern) {
	//	/// SCROLL X
	//	if (APP->window->getMods() & GLFW_MOD_SHIFT) {
	//		/// MOVE CURSOR
	//		if (e.scrollDelta.y > 0)
	//			g_editor->pattern_move_cursor_x(-1);
	//		else
	//			g_editor->pattern_move_cursor_x(+1);
	//	/// SCROLL Y
	//	} else {
	//		/// MOVE CURSOR
	//		if (e.scrollDelta.y > 0)
	//			g_editor->pattern_move_cursor_y(-1);
	//		else
	//			g_editor->pattern_move_cursor_y(+1);
	//	}
	//}
	//e.consume(this);
}

//void TrackerWidget::onSelect(const SelectEvent &e) {
//	g_editor->selected = true;
//	//this->module->lights[0].setBrightness(1.0);
//}
//
//void TrackerWidget::onDeselect(const DeselectEvent &e) {
//	//ParamWidget	*pw;
//	//
//	//pw = APP->scene->rack->touchedParam;
//	//if (pw && pw->module == this->module) {
//	//	APP->event->setSelectedWidget(this);
//	//	return;
//	//}
//	g_editor->selected = false;
//	//this->module->lights[0].setBrightness(0.0);
//}

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
	MenuSliderEdit	*slider;
	Param			*param_rate;

	separator = new MenuSeparator();
	menu->addChild(separator);

	/// [1] BASE PITCH
	slider = new MenuSliderEdit(g_module->paramQuantities[Tracker::PARAM_PITCH_BASE]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);
	/// [2] RATE
	param_rate = &(g_module->params[Tracker::PARAM_RATE]);
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

	/// [3] TUNING
	menu->addChild(rack::createSubmenuItem("Tuning", "",
		[=](Menu *menu) {
			MenuSliderEdit	*slider;
			int				i;

			/// PRESETS
			menu->addChild(new MenuItemStay("Reset scale", "",
				[=]() { set_scale(table_temp_equal); }
			));
			//// SCALE PRESETS
			menu->addChild(rack::createSubmenuItem("Presets scale", "",
				[=](Menu *menu) {
					/// FAMILY 1 - MAJOR
					menu->addChild(rack::createSubmenuItem("Major modes", "",
						[=](Menu *menu) {
							menu->addChild(new MenuItemStay("Ionian", "Major",
								[=]() { set_scale(table_scale_ionian); }
							));
							menu->addChild(new MenuItemStay("Dorian", "",
								[=]() { set_scale(table_scale_dorian); }
							));
							menu->addChild(new MenuItemStay("Phrygian", "",
								[=]() { set_scale(table_scale_phrygian); }
							));
							menu->addChild(new MenuItemStay("Lydian", "",
								[=]() { set_scale(table_scale_lydian); }
							));
							menu->addChild(new MenuItemStay("Mixolydian", "",
								[=]() { set_scale(table_scale_mixolydian); }
							));
							menu->addChild(new MenuItemStay("Aeolian", "",
								[=]() { set_scale(table_scale_aeolian); }
							));
							menu->addChild(new MenuItemStay("Locrian", "",
								[=]() { set_scale(table_scale_locrian); }
							));
						}
					));

					/// FAMILY 2 - MELODIC MINOR
					menu->addChild(rack::createSubmenuItem("Melodic minor modes", "",
						[=](Menu *menu) {
							menu->addChild(new MenuItemStay("Ionian #1", "Major",
								[=]() { set_scale(table_scale_ionian_s1); }
							));
							menu->addChild(new MenuItemStay("Dorian #7", "",
								[=]() { set_scale(table_scale_dorian_s7); }
							));
							menu->addChild(new MenuItemStay("Phrygian #6", "",
								[=]() { set_scale(table_scale_phrygian_s6); }
							));
							menu->addChild(new MenuItemStay("Lydian #5", "",
								[=]() { set_scale(table_scale_lydian_s5); }
							));
							menu->addChild(new MenuItemStay("Mixolydian #4", "",
								[=]() { set_scale(table_scale_mixolydian_s4); }
							));
							menu->addChild(new MenuItemStay("Aeolian #3", "",
								[=]() { set_scale(table_scale_aeolian_s3); }
							));
							menu->addChild(new MenuItemStay("Locrian #2", "",
								[=]() { set_scale(table_scale_locrian_s2); }
							));
						}
					));

					/// FAMILY 3 - HARMONIC MINOR
					menu->addChild(rack::createSubmenuItem("Harmonic minor modes", "",
						[=](Menu *menu) {
							menu->addChild(new MenuItemStay("Ionian #5", "Major",
								[=]() { set_scale(table_scale_ionian_s5); }
							));
							menu->addChild(new MenuItemStay("Dorian #4", "",
								[=]() { set_scale(table_scale_dorian_s4); }
							));
							menu->addChild(new MenuItemStay("Phrygian #3", "",
								[=]() { set_scale(table_scale_phrygian_s3); }
							));
							menu->addChild(new MenuItemStay("Lydian #2", "",
								[=]() { set_scale(table_scale_lydian_s2); }
							));
							menu->addChild(new MenuItemStay("Mixolydian #1", "",
								[=]() { set_scale(table_scale_mixolydian_s1); }
							));
							menu->addChild(new MenuItemStay("Aeolian #7", "",
								[=]() { set_scale(table_scale_aeolian_s7); }
							));
							menu->addChild(new MenuItemStay("Locrian #6", "",
								[=]() { set_scale(table_scale_locrian_s6); }
							));
						}
					));

					/// FAMILY 4 - HARMONIC MAJOR
					menu->addChild(rack::createSubmenuItem("Harmonic major modes", "",
						[=](Menu *menu) {
							menu->addChild(new MenuItemStay("Ionian b6", "Major",
								[=]() { set_scale(table_scale_ionian_b6); }
							));
							menu->addChild(new MenuItemStay("Dorian b5", "",
								[=]() { set_scale(table_scale_dorian_b5); }
							));
							menu->addChild(new MenuItemStay("Phrygian b4", "",
								[=]() { set_scale(table_scale_phrygian_b4); }
							));
							menu->addChild(new MenuItemStay("Lydian b3", "",
								[=]() { set_scale(table_scale_lydian_b3); }
							));
							menu->addChild(new MenuItemStay("Mixolydian b2", "",
								[=]() { set_scale(table_scale_mixolydian_b2); }
							));
							menu->addChild(new MenuItemStay("Aeolian b1", "",
								[=]() { set_scale(table_scale_aeolian_b1); }
							));
							menu->addChild(new MenuItemStay("Locrian b7", "",
								[=]() { set_scale(table_scale_locrian_b7); }
							));
						}
					));

					/// FAMILY 5 / 6 / 7
					menu->addChild(rack::createSubmenuItem("Other scales", "",
						[=](Menu *menu) {
							/// FAMILY 5 - DIMINISHED MODES
							menu->addChild(new MenuItemStay("Diminished", "",
								[=]() { set_scale(table_scale_diminished); }
							));
							menu->addChild(new MenuItemStay("Diminished inverted", "",
								[=]() { set_scale(table_scale_diminished_inverted); }
							));

							/// FAMILY 6 - AUGMENTED MODES
							menu->addChild(new MenuItemStay("Augmented", "",
								[=]() { set_scale(table_scale_augmented); }
							));
							menu->addChild(new MenuItemStay("Augmented inverted", "",
								[=]() { set_scale(table_scale_augmented_inverted); }
							));

							/// FAMILY 7 - WHOLE TONE
							menu->addChild(new MenuItemStay("Whole tone", "",
								[=]() { set_scale(table_scale_whole); }
							));
						}
					));
				}
			));
			//// TEMPERAMENT PRESETS
			menu->addChild(rack::createSubmenuItem("Presets temperament", "",
				[=](Menu *menu) {
					menu->addChild(new MenuItemStay("Equal", "default",
						[=]() { set_temperament(table_temp_equal); }
					));
					menu->addChild(new MenuItemStay("Just", "",
						[=]() { set_temperament(table_temp_just); }
					));
					menu->addChild(new MenuItemStay("Pythagorean", "",
						[=]() { set_temperament(table_temp_pyth); }
					));
					menu->addChild(new MenuItemStay("Wendy Carlos Super Just", "",
						[=]() { set_temperament(table_temp_carlos_super_just); }
					));
					menu->addChild(new MenuItemStay("Wendy Carlos Harmonic", "",
						[=]() { set_temperament(table_temp_carlos_harmonic); }
					));
					menu->addChild(new MenuItemStay("Kirnberger", "",
						[=]() { set_temperament(table_temp_kirnberger); }
					));
					menu->addChild(new MenuItemStay("Vallotti Young", "",
						[=]() { set_temperament(table_temp_vallotti_young); }
					));
					menu->addChild(new MenuItemStay("Werckmeister III", "",
						[=]() { set_temperament(table_temp_werckmeister_3); }
					));
				}
			));
			/// MANUAL
			for (i = 0; i < 12; ++i) {
				slider = new MenuSliderEdit(g_module->paramQuantities[Tracker::PARAM_TUNING + i]);
				slider->box.size.x = 200.f;
				menu->addChild(slider);
			}
		}
	));
}
