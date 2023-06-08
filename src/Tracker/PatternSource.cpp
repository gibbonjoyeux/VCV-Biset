
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
	this->glide = 0;
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
	strcpy(this->name, "Pattern");
	/// COLOR
	this->color = 0;
	/// TIME
	this->beat_count = 0;
	this->line_count = 0;
	this->lpb = 0;
	this->line_play = 0;
	/// COLUMNS
	this->note_count = 0;
	this->cv_count = 0;
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
	this->beat_count = 0;
	this->line_count = 0;
	this->lpb = 0;
	this->line_play = 0;
	/// COLUMNS
	this->note_count = 0;
	this->cv_count = 0;
	/// INIT COLUMNS CELLS
	this->resize(note_count, cv_count, beat_count, lpb);
}

void PatternSource::destroy(void) {
	this->notes.delocate();
	this->cvs.delocate();
}

void PatternSource::resize(int note_count, int cv_count, int beat_count,
		int lpb) {
	bool					change_note, change_cv;

	/// [1] CHECK CHANGES
	change_note = false;
	change_cv = false;
	if (this->beat_count != beat_count
	|| this->lpb != lpb) {
		change_note = true;
		change_cv = true;
	}
	if (this->note_count != note_count)
		change_note = true;
	if (this->cv_count != cv_count)
		change_cv = true;
	/// [2] SET VARIABLES
	this->lpb = lpb;
	this->beat_count = beat_count;
	this->line_count = this->beat_count * this->lpb;
	this->note_count = note_count;
	this->cv_count = cv_count;
	/// [3] ALLOCATE ARRAYS
	if (change_note) {
		this->notes.allocate(this->note_count,
		/**/ this->line_count * sizeof(PatternNote));
	}
	if (change_cv) {
		this->cvs.allocate(this->cv_count,
		/**/ this->line_count * sizeof(PatternCV));
	}
	/// [4] INIT NOTE COLUMNS
	// TODO: init note columns effects
}

void PatternSource::rename(char *name) {
	strcpy(this->name, name);
}

void PatternSource::context_menu(Menu *menu) {
	ParamQuantity	*quant_length;
	ParamQuantity	*quant_lpb;
	ParamQuantity	*quant_note_count;
	ParamQuantity	*quant_cv_count;
	int				length;
	int				lpb;
	int				note_count;
	int				cv_count;

	/// ADD PATTERN LENGTH SLIDER
	length = this->beat_count;
	quant_length = g_module->paramQuantities[Tracker::PARAM_PATTERN_LENGTH];
	quant_length->setValue(length);
	quant_length->defaultValue = length;
	menu->addChild(new MenuSliderEdit(quant_length, 0));
	/// ADD PATTERN LPB SLIDER
	lpb = this->lpb;
	quant_lpb = g_module->paramQuantities[Tracker::PARAM_PATTERN_LPB];
	quant_lpb->setValue(lpb);
	quant_lpb->defaultValue = lpb;
	menu->addChild(new MenuSliderEdit(quant_lpb, 0));
	/// ADD PATTERN NOTE COUNT SLIDER
	note_count = this->note_count;
	quant_note_count = g_module->paramQuantities[Tracker::PARAM_PATTERN_NOTE_COUNT];
	quant_note_count->setValue(note_count);
	quant_note_count->defaultValue = note_count;
	menu->addChild(new MenuSliderEdit(quant_note_count, 0));
	/// ADD PATTERN CV COUNT SLIDER
	cv_count = this->cv_count;
	quant_cv_count = g_module->paramQuantities[Tracker::PARAM_PATTERN_CV_COUNT];
	quant_cv_count->setValue(cv_count);
	quant_cv_count->defaultValue = cv_count;
	menu->addChild(new MenuSliderEdit(quant_cv_count, 0));
	/// ADD PATTERN UPDATE BUTTON
	menu->addChild(new MenuItemStay("Update pattern", "",
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

}
