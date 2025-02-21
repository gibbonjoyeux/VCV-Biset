
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

PatternEffect::PatternEffect() {
	this->type = PATTERN_EFFECT_NONE;
	this->value = 0;
}

PatternCV::PatternCV() {
	this->mode = PATTERN_CV_KEEP;
	this->value = 0;
	this->curve = 50;
}

PatternNote::PatternNote() {
	this->mode = PATTERN_NOTE_KEEP;
	this->glide = 0;
	this->synth = 0;
	this->pitch = 0;
	this->velocity = 255;
	this->delay = 0;
}

PatternSource::PatternSource() {
}

void PatternSource::init(void) {
	/// NAME
	memset(this->name, 0, 256);
	strcpy(this->name, "Pattern");
	/// COLOR
	this->color = 0;
	/// TIME
	this->line_play = 0;
	this->line_phase = 0.0;
	/// INIT COLUMNS CELLS
	this->resize(1, 0, 8, 4);
}

void PatternSource::init(int note_count, int cv_count, int beat_count,
		int lpb) {
	/// NAME
	strcpy(this->name, "Pattern");
	/// COLOR
	this->color = 0;
	/// TIME
	this->line_play = 0;
	this->line_phase = 0.0;
	/// INIT COLUMNS CELLS
	this->resize(note_count, cv_count, beat_count, lpb);
}

void PatternSource::destroy(void) {
	this->resize(0, 0, 0, 0);
}

void PatternSource::resize(int note_count, int cv_count, int beat_count,
		int lpb) {
	int		prev_size;
	int		i, j;

	/// [1] INIT

	this->lpb = lpb;
	this->beat_count = beat_count;
	this->line_count = this->beat_count * this->lpb;
	this->note_count = note_count;
	this->cv_count = cv_count;

	/// [2] ALLOCATE COLUMNS & LINES

	for (i = 0; i < 32; ++i) {

		prev_size = this->notes[i].lines.size();

		/// ALLOCATE NOTES

		/// ALLOCATE COLUMN
		if (i < note_count) {
			this->notes[i].lines.resize(this->line_count);
			for (j = prev_size; j < this->line_count; ++j)
				new (&(this->notes[i].lines[j])) PatternNote();
		/// FREE COLUMN
		} else {
			this->notes[i].lines.clear();
		}

		/// ALLOCATE CVS

		/// ALLOCATE COLUMN
		prev_size = this->cvs[i].lines.size();
		if (i < cv_count) {
			this->cvs[i].lines.resize(this->line_count);
			for (j = prev_size; j < this->line_count; ++j)
				new (&(this->cvs[i].lines[j])) PatternCV();
		/// FREE COLUMN
		} else {
			this->cvs[i].lines.clear();
		}
	}
}

void PatternSource::rename(char *name) {
	strncpy(this->name, name, 255);
}

void PatternSource::context_menu(Menu *menu) {
	ParamQuantityLink	*quant_length;
	ParamQuantityLink	*quant_lpb;
	ParamQuantityLink	*quant_note_count;
	ParamQuantityLink	*quant_cv_count;

	/// ADD PATTERN LENGTH SLIDER
	quant_length = (ParamQuantityLink*)
	/**/ g_module->paramQuantities[Tracker::PARAM_MENU + 0];
	quant_length->displayOffset = 0;
	quant_length->minValue = 1;
	quant_length->maxValue = 999;
	quant_length->defaultValue = this->beat_count;
	quant_length->setValue(this->beat_count);
	quant_length->name = "Pattern length";
	quant_length->unit = " beats";
	quant_length->precision = 0;
	quant_length->setLink(NULL);
	menu->addChild(new MenuSliderEdit(quant_length, 0));

	/// ADD PATTERN LPB SLIDER
	quant_lpb = (ParamQuantityLink*)
	/**/ g_module->paramQuantities[Tracker::PARAM_MENU + 1];
	quant_lpb->displayOffset = 0;
	quant_lpb->minValue = 1;
	quant_lpb->maxValue = 32;
	quant_lpb->defaultValue = this->lpb;
	quant_lpb->setValue(this->lpb);
	quant_lpb->name = "Pattern lpb";
	quant_lpb->unit = " lines / beat";
	quant_lpb->precision = 0;
	quant_lpb->setLink(NULL);
	menu->addChild(new MenuSliderEdit(quant_lpb, 0));

	/// ADD PATTERN NOTE COUNT SLIDER
	quant_note_count = (ParamQuantityLink*)
	/**/ g_module->paramQuantities[Tracker::PARAM_MENU + 2];
	quant_note_count->displayOffset = 0;
	quant_note_count->minValue = 0;
	quant_note_count->maxValue = 32;
	quant_note_count->defaultValue = this->note_count;
	quant_note_count->setValue(this->note_count);
	quant_note_count->name = "Pattern notes";
	quant_note_count->unit = " columns";
	quant_note_count->precision = 0;
	quant_note_count->setLink(NULL);
	menu->addChild(new MenuSliderEdit(quant_note_count, 0));

	/// ADD PATTERN CV COUNT SLIDER
	quant_cv_count = (ParamQuantityLink*)
	/**/ g_module->paramQuantities[Tracker::PARAM_MENU + 3];
	quant_cv_count->displayOffset = 0;
	quant_cv_count->minValue = 0;
	quant_cv_count->maxValue = 32;
	quant_cv_count->defaultValue = this->cv_count;
	quant_cv_count->setValue(this->cv_count);
	quant_cv_count->name = "Pattern cvs";
	quant_cv_count->unit = " columns";
	quant_cv_count->precision = 0;
	quant_cv_count->setLink(NULL);
	menu->addChild(new MenuSliderEdit(quant_cv_count, 0));

	/// ADD PATTERN UPDATE BUTTON
	menu->addChild(createMenuItem("Update pattern", "",
		[=]() {
			int		beat_count;
			int		lpb;
			int		note_count;
			int		cv_count;

			/// WAIT FOR THREAD FLAG
			while (g_timeline->thread_flag.test_and_set()) {}

			/// GET PATTERN SPECS
			beat_count = quant_length->getValue();
			lpb = quant_lpb->getValue();
			note_count = quant_note_count->getValue();
			cv_count = quant_cv_count->getValue();

			/// UPDATE PATTERN LENGTH
			if (beat_count != g_editor->pattern->beat_count
			|| lpb != g_editor->pattern->lpb
			|| note_count != g_editor->pattern->note_count
			|| cv_count != g_editor->pattern->cv_count) {
				g_editor->pattern->resize(note_count, cv_count, beat_count, lpb);
				g_editor->pattern_clamp_cursor();
			}

			/// CLEAR THREAD FLAG
			g_timeline->thread_flag.clear();
		}
	));

}

int PatternSource::cv_prev(int column, int line) {
	PatternCVCol	*col;
	
	col = &(this->cvs[column]);
	line -= 1;
	while (line >= 0) {
		if (col->lines[line].mode == PATTERN_CV_SET)
			return line;
		line -= 1;
	}
	return -1;
}

int PatternSource::cv_next(int column, int line) {
	PatternCVCol	*col;
	
	col = &(this->cvs[column]);
	line += 1;
	while (line < this->line_count) {
		if (col->lines[line].mode == PATTERN_CV_SET)
			return line;
		line += 1;
	}
	return -1;
}
