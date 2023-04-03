
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
	//this->chance = 255;
}

PatternSource::PatternSource() {
	this->note_count = 1;
	this->cv_count = 0;
	this->beat_count = 1;
	this->lpb = 1;
	this->line_count = this->beat_count * this->lpb;
	this->notes.allocate(this->note_count,
	/**/ this->line_count * sizeof(PatternNote));
	this->cvs.allocate(this->cv_count,
	/**/ this->line_count * sizeof(PatternCV));
	//this->color = 0;
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
