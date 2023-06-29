
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
	g_editor.synth_id = this->synth_count - 1;
	g_editor.synth = synth;
	/// RETURN NEW SYNTH
	return synth;
}

void Timeline::synth_del(Synth *synth) {
	bool	found;
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
	vector<i64>		module_ids;
	Module			*module;
	TrackerSynth	*module_synth;
	int				module_index;
	Synth			synth_tmp;
	int				index_tmp;
	u64				i;

	/// [1] TODO: SWAP SYNTH IN PATTERNS CELLS
	// ...
	/// [2] SWAP SYNTH IN TrackerSynth / TrackerDrum
	module_ids = APP->engine->getModuleIds();
	for (i = 0; i < module_ids.size(); ++i) {
		module = APP->engine->getModule(module_ids[i]);
		if (module->model->slug == "Biset-Tracker-Synth") {
			module_synth = dynamic_cast<TrackerSynth*>(module);
			module_index = module_synth->params[TrackerSynth::PARAM_SYNTH].getValue();
			if (module_index == synth_a->index)
				module_synth->params[TrackerSynth::PARAM_SYNTH].setValue(synth_b->index);
			else if (module_index == synth_b->index)
				module_synth->params[TrackerSynth::PARAM_SYNTH].setValue(synth_a->index);
		}
		//if (module->model->slug == "Biset-Tracker-Drum") {
		//	module_drum = dynamic_cast<TrackerDrum*>(module);
		//	module_index = module_drum->params[TrackerSynth::PARAM_SYNTH].getValue();
		//	if (module_index == synth_a->index)
		//		module_drum->params[TrackerSynth::PARAM_SYNTH].setValue(synth_b->index);
		//	else if (module_index == synth_b->index)
		//		module_drum->params[TrackerSynth::PARAM_SYNTH].setValue(synth_a->index);
		//}
	}
	/// [3] SWAP SYNTH IN TIMELINE
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
