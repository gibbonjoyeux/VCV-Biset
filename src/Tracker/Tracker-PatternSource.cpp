
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
}

void PatternSource::rename(char *name) {
	strcpy(this->name, name);
}
