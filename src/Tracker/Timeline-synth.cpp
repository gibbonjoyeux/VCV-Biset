
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
	synth->init();
	this->synth_count += 1;
	/// SELECT NEW SYNTH
	g_editor->synth_id = this->synth_count - 1;
	g_editor->synth = synth;
	/// RETURN NEW SYNTH
	return synth;
}

void Timeline::synth_del(Synth *synth) {
	PatternSource	*pattern;
	PatternNote		*note;
	bool			found;
	int				i, j, k;

	/// REMOVE SYNTH (PATTERNS)
	for (i = 0; i < this->pattern_count; ++i) {
		pattern = &(this->patterns[i]);
		for (j = 0; j < pattern->note_count; ++j) {
			for (k = 0; k < pattern->line_count; ++k) {
				note = &(pattern->notes[j].lines[k]);
				if (note->synth == synth->index)
					note->synth = 0;
			}
		}
	}
	/// REMOVE SYNTH (TIMELINE)
	found = false;
	for (i = 0; i < this->synth_count; ++i) {
		// SYNTH FOUND
		if (&(this->synths[i]) == synth) {
			this->synth_count -= 1;
			found = true;
			if (g_editor->synth == synth) {
				g_editor->synth_id = -1;
				g_editor->synth = NULL;
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
	PatternSource	*pattern;
	PatternNote		*note;
	vector<i64>		module_ids;
	Module			*module;
	TrackerSynth	*module_synth;
	TrackerDrum		*module_drum;
	int				module_index;
	Synth			synth_tmp;
	int				index_tmp;
	u64				id;
	int				i, j, k;

	/// [1] SWAP SYNTH (PATTERNS)
	for (i = 0; i < this->pattern_count; ++i) {
		pattern = &(this->patterns[i]);
		for (j = 0; j < pattern->note_count; ++j) {
			for (k = 0; k < pattern->line_count; ++k) {
				note = &(pattern->notes[j].lines[k]);
				if (note->synth == synth_a->index)
					note->synth = synth_b->index;
				else if (note->synth == synth_b->index)
					note->synth = synth_a->index;
			}
		}
	}
	/// [2] SWAP SYNTH (TrackerSynth / TrackerDrum)
	module_ids = APP->engine->getModuleIds();
	for (id = 0; id < module_ids.size(); ++id) {
		module = APP->engine->getModule(module_ids[id]);
		if (module->model->slug == "Biset-Tracker-Synth") {
			module_synth = dynamic_cast<TrackerSynth*>(module);
			module_index = module_synth->params[TrackerSynth::PARAM_SYNTH].getValue();
			if (module_index == synth_a->index)
				module_synth->params[TrackerSynth::PARAM_SYNTH].setValue(synth_b->index);
			else if (module_index == synth_b->index)
				module_synth->params[TrackerSynth::PARAM_SYNTH].setValue(synth_a->index);
		}
		if (module->model->slug == "Biset-Tracker-Drum") {
			module_drum = dynamic_cast<TrackerDrum*>(module);
			module_index = module_drum->params[TrackerDrum::PARAM_SYNTH].getValue();
			if (module_index == synth_a->index)
				module_drum->params[TrackerDrum::PARAM_SYNTH].setValue(synth_b->index);
			else if (module_index == synth_b->index)
				module_drum->params[TrackerDrum::PARAM_SYNTH].setValue(synth_a->index);
		}
	}
	/// [3] SWAP SYNTH (TIMELINE)
	//// SWAP SYNTHS
	synth_tmp = *synth_a;
	*synth_a = *synth_b;
	*synth_b = synth_tmp;
	//// RE-SWAP SYNTHS INDEX
	index_tmp = synth_a->index;
	synth_a->index = synth_b->index;
	synth_b->index = index_tmp;
	//// RENAME SYNTHS
	synth_a->rename();
	synth_b->rename();
}
