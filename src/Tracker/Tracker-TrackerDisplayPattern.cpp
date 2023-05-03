
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerPatternDisplay::TrackerPatternDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerPatternDisplay::draw(const DrawArgs &args) {
	Rect					rect;

	LedDisplay::draw(args);

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[1]);
	nvgRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0);
	nvgFill(args.vg);

}

void TrackerPatternDisplay::drawLayer(const DrawArgs& args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	Vec						p;
	int						pattern;

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
	p = rect.getTopLeft();
	p.y -= 1.0;

	/// DRAW ACTIVE PATTERN
	pattern = module->params[Tracker::PARAM_PATTERN].getValue();
	itoaw(this->str, pattern, 3);
	nvgFillColor(args.vg, colors[13]);
	nvgText(args.vg, p.x + 2.0, p.y + 9.5, this->str, NULL);

	LedDisplay::drawLayer(args, layer);
}

void TrackerPatternDisplay::onButton(const ButtonEvent &e) {
	PatternSource	*pattern;
	PatternNoteRow	*col_note;
	PatternCVRow	*col_cv;
	Menu			*menu;
	MenuLabel		*label;
	ParamQuantity	*quant_length;
	ParamQuantity	*quant_lpb;
	ParamQuantity	*quant_note_count;
	ParamQuantity	*quant_cv_count;
	ParamQuantity	*quant_effect_count;
	ParamQuantity	*quant;
	int				length;
	int				lpb;
	int				note_count;
	int				cv_count;

	menu = createMenu();

	/// ADD PATTERN EDITION SECTION

	label = new MenuLabel();
	label->text = "Edit Pattern";
	menu->addChild(label);

	pattern = g_editor.pattern;
	/// ADD PATTERN LENGTH SLIDER
	length = pattern->beat_count;
	quant_length = g_module->paramQuantities[Tracker::PARAM_PATTERN_LENGTH];
	quant_length->setValue(length);
	quant_length->defaultValue = length;
	menu->addChild(new MenuSliderEdit(quant_length, 0));
	/// ADD PATTERN LPB SLIDER
	lpb = pattern->lpb;
	quant_lpb = g_module->paramQuantities[Tracker::PARAM_PATTERN_LPB];
	quant_lpb->setValue(lpb);
	quant_lpb->defaultValue = lpb;
	menu->addChild(new MenuSliderEdit(quant_lpb, 0));
	/// ADD PATTERN NOTE COUNT SLIDER
	note_count = pattern->note_count;
	quant_note_count = g_module->paramQuantities[Tracker::PARAM_PATTERN_NOTE_COUNT];
	quant_note_count->setValue(note_count);
	quant_note_count->defaultValue = note_count;
	menu->addChild(new MenuSliderEdit(quant_note_count, 0));
	/// ADD PATTERN CV COUNT SLIDER
	cv_count = pattern->cv_count;
	quant_cv_count = g_module->paramQuantities[Tracker::PARAM_PATTERN_CV_COUNT];
	quant_cv_count->setValue(cv_count);
	quant_cv_count->defaultValue = cv_count;
	menu->addChild(new MenuSliderEdit(quant_cv_count, 0));
	/// ADD PATTERN UPDATE BUTTON
	menu->addChild(new MenuItemStay("Update Pattern", "",
		[=]() {
			int	beat_count;
			int	lpb;
			int	note_count;
			int	cv_count;

			/// WAIT FOR THREAD FLAG
			while (g_timeline.thread_flag.test_and_set()) {}

			/// GET PATTERN SPECS
			beat_count = g_module->params[Tracker::PARAM_PATTERN_LENGTH].getValue();
			lpb = g_module->params[Tracker::PARAM_PATTERN_LPB].getValue();
			note_count = g_module->params[Tracker::PARAM_PATTERN_NOTE_COUNT].getValue();
			cv_count = g_module->params[Tracker::PARAM_PATTERN_CV_COUNT].getValue();
			/// UPDATE PATTERN LENGTH
			if (beat_count != g_editor.pattern->beat_count
			|| lpb != g_editor.pattern->lpb
			|| note_count != g_editor.pattern->note_count
			|| cv_count != g_editor.pattern->cv_count) {
				g_editor.pattern->resize(note_count, cv_count, beat_count, lpb);
				g_editor.pattern_clamp_cursor();
			}

			/// CLEAR THREAD FLAG
			g_timeline.thread_flag.clear();
		}
	));

	/// ADD COLUMN EDITION SECTION

	menu->addChild(new MenuSeparator());

	label = new MenuLabel();
	label->text = "Edit Column";
	menu->addChild(label);

	/// COLUMN AS NOTE COLUMN
	if (g_editor.pattern_row < g_editor.pattern->note_count) {
		col_note = g_editor.pattern->notes[g_editor.pattern_row];
		/// ADD COLUMN MODE LIST
		menu->addChild(rack::createSubmenuItem("Mode", "",
			[=](Menu *menu) {
				ParamQuantity		*quant_mode;

				quant_mode = g_module->paramQuantities[Tracker::PARAM_COLUMN_NOTE_MODE];
				quant_mode->setValue(col_note->mode);
				menu->addChild(new MenuCheckItem("Gate", "",
					[=]() { return quant_mode->getValue() == PATTERN_NOTE_MODE_GATE; },
					[=]() { quant_mode->setValue(PATTERN_NOTE_MODE_GATE); }
				));
				menu->addChild(new MenuCheckItem("Trigger", "",
					[=]() { return quant_mode->getValue() == PATTERN_NOTE_MODE_TRIGGER; },
					[=]() { quant_mode->setValue(PATTERN_NOTE_MODE_TRIGGER); }
				));
				menu->addChild(new MenuCheckItem("Drum", "",
					[=]() { return quant_mode->getValue() == PATTERN_NOTE_MODE_DRUM; },
					[=]() { quant_mode->setValue(PATTERN_NOTE_MODE_DRUM); }
				));
			}
		));
		/// ADD COLUMN EFFECT COUNT SLIDER
		quant_effect_count = g_module->paramQuantities[Tracker::PARAM_COLUMN_NOTE_EFFECT_COUNT];
		quant_effect_count->setValue(col_note->effect_count);
		quant_effect_count->defaultValue = col_note->effect_count;
		menu->addChild(new MenuSliderEdit(quant_effect_count, 0));
	/// COLUMN AS CV COLUMN
	} else {
		col_cv = g_editor.pattern->cvs[g_editor.pattern_row - g_editor.pattern->note_count];
		/// ADD COLUMN MODE LIST
		menu->addChild(rack::createSubmenuItem("Mode", "",
			[=](Menu *menu) {
				ParamQuantity		*quant_mode;

				quant_mode = g_module->paramQuantities[Tracker::PARAM_COLUMN_CV_MODE];
				quant_mode->setValue(col_cv->mode);
				menu->addChild(new MenuCheckItem("CV", "",
					[=]() { return quant_mode->getValue() == PATTERN_CV_MODE_CV; },
					[=]() { quant_mode->setValue(PATTERN_CV_MODE_CV); }
				));
				menu->addChild(new MenuCheckItem("Gate", "",
					[=]() { return quant_mode->getValue() == PATTERN_CV_MODE_GATE; },
					[=]() { quant_mode->setValue(PATTERN_CV_MODE_GATE); }
				));
				menu->addChild(new MenuCheckItem("Trigger", "",
					[=]() { return quant_mode->getValue() == PATTERN_CV_MODE_TRIGGER; },
					[=]() { quant_mode->setValue(PATTERN_CV_MODE_TRIGGER); }
				));
				menu->addChild(new MenuCheckItem("BPM", "",
					[=]() { return quant_mode->getValue() == PATTERN_CV_MODE_BPM; },
					[=]() { quant_mode->setValue(PATTERN_CV_MODE_BPM); }
				));
			}
		));
		/// ADD COLUMN SYNTH SELECT SLIDER
		quant = g_module->paramQuantities[Tracker::PARAM_COLUMN_CV_SYNTH];
		quant->setValue(col_cv->synth);
		quant->defaultValue = col_cv->synth;
		menu->addChild(new MenuSliderEdit(quant, 0));
		/// ADD COLUMN SYNTH CHANNEL SELECT SLIDER
		quant = g_module->paramQuantities[Tracker::PARAM_COLUMN_CV_CHANNEL];
		quant->setValue(col_cv->channel);
		quant->defaultValue = col_cv->channel;
		menu->addChild(new MenuSliderEdit(quant, 0));
	}

	/// ADD PATTERN COLUMN UPDATE BUTTON
	menu->addChild(new MenuItemStay("Update Column", "",
		[=]() {
			//int	note_mode;
			//int	note_effect_count;
			//int	cv_mode;
			//int	cv_synth;
			//int	cv_channel;

			/// WAIT FOR THREAD FLAG
			while (g_timeline.thread_flag.test_and_set()) {}

			///// UPDATE PATTERN COLUMNS
			////// AS NOTE
			//if (g_editor.pattern_row < g_editor.pattern->note_count) {
			//	col_note = g_editor.pattern->notes[g_editor.pattern_row];
			//	note_mode = g_module->params[Tracker::PARAM_EDIT + 6].getValue();
			//	note_effect = g_module->params[Tracker::PARAM_EDIT + 7].getValue();
			//	if (note_mode != col_note->mode)
			//		col_note->mode = note_mode;
			//	if (note_effect != col_note->effect_count)
			//		col_note->effect_count = note_effect;
			////// AS CV
			//} else {
			//	col_cv = g_editor.pattern->cvs[g_editor.pattern_row - g_editor.pattern->note_count];
			//	cv_mode = g_module->params[Tracker::PARAM_EDIT + 6].getValue();
			//	cv_synth = g_module->params[Tracker::PARAM_EDIT + 7].getValue();
			//	cv_channel = g_module->params[Tracker::PARAM_EDIT + 8].getValue();
			//	if (cv_mode != col_cv->mode)
			//		col_cv->mode = cv_mode;
			//	if (cv_synth != col_cv->synth)
			//		col_cv->synth = cv_synth;
			//	if (cv_channel != col_cv->channel)
			//		col_cv->channel = cv_channel;
			//}

			/// CLEAR THREAD FLAG
			g_timeline.thread_flag.clear();
		}
	));
}
