
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

PatternSource *Timeline::pattern_new(void) {
	PatternSource	*pattern;

	if (this->pattern_count >= 999)
		return NULL;

	/// GET NEW PATTERN
	pattern = &(this->patterns[this->pattern_count]);
	pattern->init();
	this->pattern_count += 1;
	/// SELECT NEW PATTERN
	g_editor->pattern_id = this->pattern_count - 1;
	g_editor->pattern = pattern;
	/// RETURN NEW PATTERN
	return pattern;
}

PatternSource *Timeline::pattern_new(int note_count, int cv_count,
				int beat_count, int lpb) {
	PatternSource	*pattern;

	if (this->pattern_count >= 999)
		return NULL;

	/// GET NEW PATTERN
	pattern = &(this->patterns[this->pattern_count]);
	pattern->init(note_count, cv_count, beat_count, lpb);
	this->pattern_count += 1;
	/// SELECT NEW PATTERN
	g_editor->pattern_id = this->pattern_count - 1;
	g_editor->pattern = pattern;
	/// RETURN NEW PATTERN
	return pattern;
}

void Timeline::pattern_del(PatternSource *pattern) {
	PatternSource	removed;
	bool			found;
	int				i;

	/// [1] REMOVE PATTERN (INSTANCES)

	for (i = 0; i < 32; ++i) {
		this->timeline[i].remove_if([=](PatternInstance &instance) {
			PatternSource	*instance_source;

			instance_source = instance.source;
			/// INSTANCE SELECTED
			if (&instance == g_editor->instance)
				g_editor->instance = NULL;
			/// INSTANCE OF UPPER PATTERN (OFFSET SOURCE)
			if (instance.source > pattern)
				instance.source -= 1;
			/// INSTANCE OF DELETED PATTERN
			return (instance_source == pattern);
		});
	}

	/// [2] REMOVE PATTERN (PATTERNS)

	found = false;
	for (i = 0; i < this->pattern_count; ++i) {
		// PATTERN FOUND
		if (&(this->patterns[i]) == pattern) {
			/// CLEAR PATTERN
			this->patterns[i].destroy();
			this->pattern_count -= 1;
			found = true;
			/// SAVE PATTERN
			removed = this->patterns[i];
		}
		/// PATTERN OFFSET
		if (found == true)
			this->patterns[i] = this->patterns[i + 1];
	}
	//// SWAP CLEARED PATTERN WITH LAST
	this->patterns[this->pattern_count] = removed;
	//// DESELECT
	g_editor->pattern = NULL;
	g_editor->pattern_id = -1;
}

void Timeline::pattern_swap(PatternSource *pat_a, PatternSource *pat_b) {
	u8								buffer[sizeof(PatternSource)];
	list<PatternInstance>::iterator	it, it_end;
	int								i;

	/// [1] SWAP PATTERNS (INSTANCES)

	for (i = 0; i < 32; ++i) {
		it = this->timeline[i].begin();
		it_end = this->timeline[i].end();
		/// LOOP INSTANCES
		while (it != it_end) {
			/// CHECK INSTANCE
			if (it->source == pat_a)
				it->source = pat_b;
			else if (it->source == pat_b)
				it->source = pat_a;
			/// NEXT INSTANCE
			it = std::next(it);
		}
	}

	/// [2] SWAP PATTERNS (PATTERNS)

	// -> Use buffer to avoid PatternSource members destruction
	memcpy((void*)buffer, (void*)pat_a, sizeof(PatternSource));
	memcpy((void*)pat_a, (void*)pat_b, sizeof(PatternSource));
	memcpy((void*)pat_b, (void*)buffer, sizeof(PatternSource));
}

void Timeline::pattern_dup(PatternSource *pattern) {
	PatternSource	*pattern_new;
	PatternNoteCol	*col_note;
	PatternNoteCol	*col_note_new;
	PatternCVCol	*col_cv;
	PatternCVCol	*col_cv_new;
	int				i, j;

	/// [1] CREATE NEW PATTERN

	pattern_new = g_timeline->pattern_new(
	/**/ pattern->note_count, pattern->cv_count,
	/**/ pattern->beat_count, pattern->lpb);
	if (pattern_new == NULL)
		return;
	pattern_new->rename(pattern->name);
	pattern_new->color = pattern->color;

	/// [2] COPY NOTES

	for (i = 0; i < pattern->note_count; ++i) {
		/// GET COLUMNS
		col_note = &(pattern->notes[i]);
		col_note_new = &(pattern_new->notes[i]);
		/// COPY COLUMN
		col_note_new->fx_count = col_note->fx_count;
		/// COPY COLUMN LINES
		for (j = 0; j < pattern->line_count; ++j)
			col_note_new->lines[j] = col_note->lines[j];
	}

	/// [3] COPY CVS

	for (i = 0; i < pattern->cv_count; ++i) {
		/// GET COLUMNS
		col_cv = &(pattern->cvs[i]);
		col_cv_new = &(pattern_new->cvs[i]);
		/// COPY COLUMNS
		col_cv_new->mode = col_cv->mode;
		col_cv_new->synth = col_cv->synth;
		col_cv_new->channel = col_cv->channel;
		/// COPY COLUMNS LINES
		for (j = 0; j < pattern->line_count; ++j)
			col_cv_new->lines[j] = col_cv->lines[j];
	}
}
