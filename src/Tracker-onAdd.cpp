
#include "plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void Tracker::onAdd(const AddEvent &e) {
	/// [1] INIT EDITION
	g_editor.set_synth(this->params[PARAM_SYNTH].getValue(), false);
	g_editor.set_pattern(this->params[PARAM_PATTERN].getValue(), false);
	g_editor.set_song_length(g_timeline.beat_count, true);
}
