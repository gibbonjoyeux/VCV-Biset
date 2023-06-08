
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
	g_editor.pattern_id = this->pattern_count - 1;
	g_editor.pattern = pattern;
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
	g_editor.pattern_id = this->pattern_count - 1;
	g_editor.pattern = pattern;
	/// RETURN NEW PATTERN
	return pattern;
}

void Timeline::pattern_del(PatternSource *pattern) {
	bool	found;
	int		i;

	/// TODO: REMOVE TIMELINE PATTERN INSTANCES
	// ...
	found = false;
	for (i = 0; i < this->pattern_count; ++i) {
		// PATTERN FOUND
		if (&(this->patterns[i]) == pattern) {
			this->patterns[i].destroy();
			this->pattern_count -= 1;
			found = true;
			if (g_editor.pattern == pattern) {
				g_editor.pattern_id = -1;
				g_editor.pattern = NULL;
			}
		}
		/// PATTERN OFFSET
		if (found == true)
			this->patterns[i] = this->patterns[i + 1];
	}
}

void Timeline::pattern_swap(PatternSource *pat_a, PatternSource *pat_b) {
	u8				buffer[sizeof(PatternSource)];

	// TODO: Swap patterns in instances
	/// SWAP PATTERNS
	// -> Use buffer to avoid PatternSource members destroy (ArrayExt)
	memcpy((void*)buffer, (void*)pat_a, sizeof(PatternSource));
	memcpy((void*)pat_a, (void*)pat_b, sizeof(PatternSource));
	memcpy((void*)pat_b, (void*)buffer, sizeof(PatternSource));
}
