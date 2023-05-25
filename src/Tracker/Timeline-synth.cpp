
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Synth *Timeline::synth_new(void) {
	Synth		*synth;

	if (this->synth_count >= 99)
		return NULL;

	/// GET NEW SYNTH
	synth = &(this->synths[this->synth_count]);
	synth->init(this->synth_count, 1);
	this->synth_count += 1;
	/// SELECT NEW SYNTH
	g_editor.synth_id = this->synth_count - 1;
	g_editor.synth = synth;
	/// RETURN NEW SYNTH
	return synth;
}

void Timeline::synth_del(Synth *synth) {
	bool	found;
	int		index;
	int		i;

	/// TODO: SET PATTERNS CELL USING SYNTH TO 0
	// ...
	found = false;
	for (i = 0; i < this->synth_count; ++i) {
		// SYNTH FOUND
		if (&(this->synths[i]) == synth) {
			this->synth_count -= 1;
			found = true;
			if (g_editor.synth == synth) {
				g_editor.synth_id = -1;
				g_editor.synth = NULL;
			}
		}
		/// SYNTH OFFSET
		if (found == true) {
			this->synths[i] = this->synths[i + 1];
			this->synths[i].index = i;
			this->synths[i].rename();
		}
	}
}

void Timeline::synth_swap(Synth *synth_a, Synth *synth_b) {
	Synth			synth_tmp;
	int				index_tmp;

	/// TODO: SWAP SYNTH IN PATTERNS CELLS
	// ...
	synth_tmp = *synth_a;
	*synth_a = *synth_b;
	*synth_b = synth_tmp;

	index_tmp = synth_a->index;
	synth_a->index = synth_b->index;
	synth_b->index = index_tmp;
}
