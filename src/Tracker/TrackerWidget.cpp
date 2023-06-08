
#include "Tracker.hpp"

#define BTN_PLAY_X			17.25
#define BTN_PLAY_Y			6.0
#define BTN_PLAY_STEP		6.0
#define KNOB_X				10.0
#define KNOB_Y				18.0
#define KNOB_STEP			12.0
#define BTN_JUMP_X			15.50
#define BTN_JUMP_Y			40.0
#define BTN_OCTAVE_X		(BTN_JUMP_X + 13.0)
#define BTN_OCTAVE_Y		BTN_JUMP_Y

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

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerWidget::TrackerWidget(Tracker* _module) {
	TrackerDisplay			*display;
	TrackerDisplaySide		*display_side;
	//TrackerDisplayBPM		*display_bpm;
	//TrackerDisplaySynth		*display_synth;
	//TrackerDisplayPattern	*display_pattern;
	//LedDisplayDigit			*display_jump;
	//LedDisplayDigit			*display_octave;
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
	display = createWidget<TrackerDisplay>(mm2px(Vec(51.50, 5.0)));
	//display->box.size = mm2px(Vec(173.5 + 14.0, 94.5 + 15.0));
	display->box.size = Vec(CHAR_W * (CHAR_COUNT_X + 3) + 4, CHAR_H * CHAR_COUNT_Y + 5.5);
	display->module = module;
	display->moduleWidget = this;
	this->display = display;
	addChild(display);

	//// SIDE LED DISPLAY
	display_side = createWidget<TrackerDisplaySide>(mm2px(Vec(244.0, 5.0)));
	display_side->box.size = Vec(CHAR_W * 26, CHAR_H * CHAR_COUNT_Y + 5.5);
	display_side->module = module;
	display_side->moduleWidget = this;
	this->display_side = display_side;
	addChild(display_side);

	//// BPM / SYNTH / PATTERN KNOBS
	/// BPM SELECTOR
	//// DISPLAY
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

	/// SYNTH SELECTOR
	//// DISPLAY
	//display_synth = createWidget<TrackerDisplaySynth>(mm2px(Vec(KNOB_X + KNOB_STEP, KNOB_Y)));
	//display_synth->box.size = mm2px(Vec(8.25, 3.5));
	//display_synth->module = module;
	//if (module)
	//	display_synth->value_quant = module->paramQuantities[Tracker::PARAM_SYNTH];
	//display_synth->value_length = 2;
	//display_synth->color_back = colors[15];
	//display_synth->color_font = colors[4];
	//addChild(display_synth);
	//// KNOB
	addParam(createParamCentered<KnobMedium>(mm2px(Vec(KNOB_X + KNOB_STEP + 4.25, KNOB_Y + 10.0)),
	/**/ module,
	/**/ Tracker::PARAM_SYNTH));

	/// PATTERN SELECTOR
	//// DISPLAY
	//display_pattern = createWidget<TrackerDisplayPattern>(mm2px(Vec(KNOB_X + KNOB_STEP * 2, KNOB_Y)));
	//display_pattern->box.size = mm2px(Vec(8.25, 3.5));
	//display_pattern->module = module;
	//if (module)
	//	display_pattern->value_quant = module->paramQuantities[Tracker::PARAM_PATTERN];
	//display_pattern->value_length = 3;
	//display_pattern->color_back = colors[15];
	//display_pattern->color_font = colors[4];
	//addChild(display_pattern);
	//// KNOB
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(KNOB_X + KNOB_STEP * 2 + 4.25, KNOB_Y + 10.0)),
	/**/ module,
	/**/ Tracker::PARAM_PATTERN));

	/// SELECT JUMP
	//// DISPLAY
	//display_jump = createWidget<LedDisplayDigit>(mm2px(Vec(BTN_JUMP_X, BTN_JUMP_Y)));
	//display_jump->box.size = mm2px(Vec(8.25, 3.5));
	//display_jump->module = module;
	//display_jump->value_link = &(g_editor.pattern_jump);
	//display_jump->value_length = 2;
	//display_jump->color_back = colors[15];
	//display_jump->color_font = colors[4];
	//addChild(display_jump);
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
	//display_octave = createWidget<LedDisplayDigit>(mm2px(Vec(BTN_OCTAVE_X, BTN_OCTAVE_Y)));
	//display_octave->box.size = mm2px(Vec(8.25, 3.5));
	//display_octave->module = module;
	//display_octave->value_link = &(g_editor.pattern_octave);
	//display_octave->value_length = 2;
	//display_octave->color_back = colors[15];
	//display_octave->color_font = colors[4];
	//addChild(display_octave);
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
	/// CHANGE VIEW
	if (e.action == GLFW_PRESS && (e.mods & GLFW_MOD_SHIFT)
	&& (e.key == GLFW_KEY_LEFT || e.key == GLFW_KEY_RIGHT)) {
		if (g_editor.mode == EDITOR_MODE_PATTERN) {
			g_module->params[Tracker::PARAM_MODE + 0].setValue(0);
			g_module->params[Tracker::PARAM_MODE + 1].setValue(1);
			g_editor.mode = EDITOR_MODE_TIMELINE;
		} else if (g_editor.mode == EDITOR_MODE_TIMELINE) {
			g_module->params[Tracker::PARAM_MODE + 0].setValue(1);
			g_module->params[Tracker::PARAM_MODE + 1].setValue(0);
			g_editor.mode = EDITOR_MODE_PATTERN;
		}
		e.consume(this);
		return;
	}
	/// EDIT PATTERN & TIMELINE
	if (g_editor.mode == EDITOR_MODE_PATTERN) {
		this->display->on_key_pattern(e);
	} else if (g_editor.mode == EDITOR_MODE_TIMELINE) {
		this->display->on_key_timeline(e);
	}
	e.consume(this);
}

void TrackerWidget::onHoverScroll(const HoverScrollEvent &e) {
	ModuleWidget::onHoverScroll(e);
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

//void TrackerWidget::onSelect(const SelectEvent &e) {
//	g_editor.selected = true;
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
//	g_editor.selected = false;
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
