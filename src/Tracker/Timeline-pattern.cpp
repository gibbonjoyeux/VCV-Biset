
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

void Timeline::pattern_del(PatternSource *pattern) {
	bool	found;
	int		index;
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
	PatternSource	pat_tmp;

	pat_tmp = *pat_a;
	*pat_a = *pat_b;
	*pat_b = pat_tmp;
}
