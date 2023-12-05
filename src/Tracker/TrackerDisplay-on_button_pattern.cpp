
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static void get_cell(
	const	rack::Widget::ButtonEvent &e,
	int		&row,
	int		&col,
	int		&cell) {
	PatternSource	*pattern;
	PatternNoteCol	*col_note;
	int		x, x_aim;
	int		i, j;

	pattern = g_editor->pattern;
	/// [1] COMPUTE ROW
	row = (int)((e.pos.y - 3.0) / CHAR_H) + g_editor->pattern_cam_y;
	/// [2] COMPUTE COLUMN
	cell = 0;
	col = 0;
	x = 0;
	x_aim = (int)((e.pos.x - 2.0) / CHAR_W) - 2 + g_editor->pattern_cam_x;
	//// FOR EACH NOTE COLUMN
	for (i = 0; i < pattern->note_count; ++i) {
		col_note = pattern->notes[i];
		/// PITCH
		x += 2;
		if (x >= x_aim) {
			cell = 0;
			return;
		}
		/// PITCH OCTAVE
		x += 1;
		if (x >= x_aim) {
			cell = 1;
			return;
		}
		/// VELOCITY
		if (g_editor->pattern_view_velo) {
			x += 2;
			if (x >= x_aim) {
				cell = 2;
				return;
			}
		}
		/// PANNING
		if (g_editor->pattern_view_pan) {
			x += 2;
			if (x >= x_aim) {
				cell = 3;
				return;
			}
		}
		/// SYNTH
		x += 2;
		if (x >= x_aim) {
			cell = 4;
			return;
		}
		/// DELAY
		if (g_editor->pattern_view_glide) {
			x += 2;
			if (x >= x_aim) {
				cell = 5;
				return;
			}
		}
		/// GLIDE
		if (g_editor->pattern_view_delay) {
			x += 2;
			if (x >= x_aim) {
				cell = 6;
				return;
			}
		}
		/// EFFECTS
		if (g_editor->pattern_view_fx) {
			cell = 7;
			for (j = 0; j < col_note->fx_count; ++j) {
				/// TYPE
				x += 1;
				if (x >= x_aim) {
					cell = 7 + j * 2;
					return;
				}
				/// VALUE
				x += 2;
				if (x >= x_aim) {
					cell = 7 + j * 2 + 1;
					return;
				}
			}
		}
		x += 1;
		col += 1;
	}
	//// FOR EACH CV COLUMN
	if (x < x_aim) {
		for (i = 0; i < pattern->cv_count; ++i) {
			/// VALUE
			x += 3;
			if (x >= x_aim) {
				cell = 0;
				return;
			}
			/// GLIDE
			x += 2;
			if (x >= x_aim) {
				cell = 1;
				return;
			}
			/// DELAY
			x += 2;
			if (x >= x_aim) {
				cell = 2;
				return;
			}
			x += 1;
			col += 1;
		}
	}
}

static void on_button_left(const rack::Widget::ButtonEvent &e) {
	int		row, col, cell;

	/// [1] COMPUTE ROW
	get_cell(e, row, col, cell);
	/// [3] MOVE CURSOR
	g_editor->pattern_line = row;
	g_editor->pattern_col = col;
	g_editor->pattern_cell = cell;
	g_editor->pattern_char = 0;
	/// [4] CLAMP CURSOR
	g_editor->pattern_clamp_cursor();
}

static void on_button_right(const rack::Widget::ButtonEvent &e) {
	PatternSource		*pattern;
	PatternNoteCol		*col_note;
	PatternCVCol		*col_cv;
	Menu				*menu;
	MenuLabel			*label;
	ParamQuantityLink	*quant_fx_count;
	ParamQuantityLink	*quant_synth;
	ParamQuantityLink	*quant_channel;
	ParamQuantityLink	*quant_mode;

	/// SELECT CLICKED COLUMN
	on_button_left(e);

	if (g_timeline->play != TIMELINE_MODE_STOP)
		return;

	pattern = g_editor->pattern;
	menu = createMenu();

	/// ADD PATTERN EDITION SECTION

	label = new MenuLabel();
	label->text = "Edit Pattern";
	menu->addChild(label);

	pattern->context_menu(menu);

	/// ADD COLUMN EDITION SECTION

	menu->addChild(new MenuSeparator());

	label = new MenuLabel();
	label->text = "Edit Column";
	menu->addChild(label);

	/// COLUMN AS NOTE COLUMN
	if (g_editor->pattern_col < g_editor->pattern->note_count) {
		col_note = g_editor->pattern->notes[g_editor->pattern_col];
		/// ADD COLUMN EFFECT COUNT SLIDER
		quant_fx_count = (ParamQuantityLink*)
		/**/ g_module->paramQuantities[Tracker::PARAM_MENU + 4];
		quant_fx_count->minValue = 0;
		quant_fx_count->maxValue = 16;
		quant_fx_count->defaultValue = col_note->fx_count;
		quant_fx_count->setValue(col_note->fx_count);
		quant_fx_count->name = "Column effects";
		quant_fx_count->unit = "";
		quant_fx_count->precision = 0;
		quant_fx_count->setLink(NULL);
		menu->addChild(new MenuSliderEdit(quant_fx_count, 0));

		/// ADD COLUMN UPDATE BUTTON
		menu->addChild(createMenuItem("Update pattern column", "",
			[=]() {
				PatternNoteCol	*col_note;
				int				count;

				/// WAIT FOR THREAD FLAG
				while (g_timeline->thread_flag.test_and_set()) {}

				/// UPDATE PATTERN COLUMNS
				col_note = g_editor->pattern->notes[g_editor->pattern_col];
				count = g_module->params[Tracker::PARAM_MENU + 4]
				/**/ .getValue();
				if (count != col_note->fx_count)
					col_note->fx_count = count;

				/// CLEAR THREAD FLAG
				g_timeline->thread_flag.clear();
			}
		));
	/// COLUMN AS CV COLUMN
	} else if (g_editor->pattern_col
	< g_editor->pattern->note_count + g_editor->pattern->cv_count) {
		col_cv = g_editor->pattern->cvs
		/**/ [g_editor->pattern_col - g_editor->pattern->note_count];
		/// ADD COLUMN MODE LIST
		quant_mode = (ParamQuantityLink*)
		/**/ g_module->paramQuantities[Tracker::PARAM_MENU + 4];
		quant_mode->minValue = 0;
		quant_mode->maxValue = 3;
		quant_mode->defaultValue = col_cv->mode;
		quant_mode->setValue(col_cv->mode);
		//quant_mode->name = "Column effects";
		//quant_mode->unit = "";
		quant_mode->precision = 0;
		quant_mode->setLink(NULL);
		menu->addChild(rack::createSubmenuItem("Mode", "",
			[=](Menu *menu) {
				menu->addChild(new MenuCheckItem("CV", "",
					[=]() { return quant_mode->getValue() == PATTERN_CV_MODE_CV; },
					[=]() { quant_mode->setValue(PATTERN_CV_MODE_CV); }
				));
				menu->addChild(new MenuCheckItem("Gate", "",
					[=]() { return quant_mode->getValue() == PATTERN_CV_MODE_GATE; },
					[=]() { quant_mode->setValue(PATTERN_CV_MODE_GATE); }
				));
				menu->addChild(new MenuCheckItem("BPM", "",
					[=]() { return quant_mode->getValue() == PATTERN_CV_MODE_BPM; },
					[=]() { quant_mode->setValue(PATTERN_CV_MODE_BPM); }
				));
			}
		));
		/// ADD COLUMN SYNTH SELECT SLIDER
		quant_synth = (ParamQuantityLink*)
		/**/ g_module->paramQuantities[Tracker::PARAM_MENU + 5];
		quant_synth->minValue = 0;
		quant_synth->maxValue = 99;
		quant_synth->defaultValue = col_cv->synth;
		quant_synth->setValue(col_cv->synth);
		quant_synth->name = "Column synth";
		//quant_synth->unit = "";
		quant_synth->precision = 0;
		quant_synth->setLink(NULL);
		menu->addChild(new MenuSliderEdit(quant_synth, 0));
		/// ADD COLUMN SYNTH CHANNEL SELECT SLIDER
		quant_channel = (ParamQuantityLink*)
		/**/ g_module->paramQuantities[Tracker::PARAM_MENU + 6];
		quant_channel->minValue = 0;
		quant_channel->maxValue = 7;
		quant_channel->defaultValue = col_cv->channel;
		quant_channel->setValue(col_cv->channel);
		quant_channel->name = "Column synth channel";
		//quant_channel->unit = "";
		quant_channel->precision = 0;
		quant_channel->setLink(NULL);
		menu->addChild(new MenuSliderEdit(quant_channel, 0));
		/// ADD COLUMN UPDATE BUTTON
		menu->addChild(createMenuItem("Update pattern column", "",
			[=]() {
				//PatternCVCol	*col_cv;
				int				cv_mode;
				int				cv_synth;
				int				cv_channel;

				/// WAIT FOR THREAD FLAG
				while (g_timeline->thread_flag.test_and_set()) {}

				/// UPDATE PATTERN COLUMNS
				//col_cv = g_editor->pattern->cvs[g_editor->pattern_col - g_editor->pattern->note_count];
				cv_mode = quant_mode->getValue();
				cv_synth = quant_synth->getValue();
				cv_channel = quant_channel->getValue();
				if (cv_mode != col_cv->mode)
					col_cv->mode = cv_mode;
				if (cv_synth != col_cv->synth)
					col_cv->synth = cv_synth;
				if (cv_channel != col_cv->channel)
					col_cv->channel = cv_channel;

				/// CLEAR THREAD FLAG
				g_timeline->thread_flag.clear();
			}
		));
	}
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::on_button_pattern(const ButtonEvent &e) {
	if (g_editor->pattern == NULL)
		return;
	if (e.button == GLFW_MOUSE_BUTTON_LEFT)
		on_button_left(e);
	else if (e.button == GLFW_MOUSE_BUTTON_RIGHT)
		on_button_right(e);
	e.consume(this);
}
