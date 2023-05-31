
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

PatternInstance *Timeline::instance_new(PatternSource *source, int row, int beat) {
	std::list<PatternInstance>::iterator	it, it_end;
	std::list<PatternInstance>::iterator	it_next;
	PatternInstance							instance(source, row, beat);

	/// INIT INSTANCE DATA
	instance.source = source;
	instance.row = row;
	instance.beat = beat;
	instance.beat_start = 0;
	instance.beat_length = source->beat_count;
	instance.muted = false;

	it = g_timeline.timeline[row].begin();
	it_end = g_timeline.timeline[row].end();
	it_next = it_end;
	/// LOOP INSTANCES
	while (it != it_end) {
		/// CHECK INSTANCE
		if (it->beat > beat) {
			it_next = it;
			break;
		}
		/// NEXT INSTANCE
		it = std::next(it);
	}
	/// INSERT INSTANCE
	it = g_timeline.timeline[row].insert(it_next, instance);
	g_editor.instance = &(*it);
	g_editor.instance_row = row;
	g_editor.instance_beat = beat;
	return NULL;
}

void Timeline::instance_del(PatternInstance *instance) {
	list<PatternInstance>::iterator	it;
	list<PatternInstance>::iterator	it_end;

	/// FIND INSTANCE
	it = g_timeline.timeline[instance->row].begin();
	it_end = g_timeline.timeline[instance->row].end();
	while (it != it_end) {
		if (&(*it) == instance) {
			g_timeline.timeline[instance->row].erase(it);
			return;
		}
		it = std::next(it);
	}
}

void Timeline::instance_move(PatternInstance *instance, int row, int beat) {
	list<PatternInstance>::iterator	it_src;
	list<PatternInstance>::iterator	it_dst;
	list<PatternInstance>::iterator	it_end;

	/// FIND INSTANCE
	it_src = g_timeline.timeline[instance->row].begin();
	it_end = g_timeline.timeline[instance->row].end();
	while (it_src != it_end) {
		if (&(*it_src) == instance)
			break;
		it_src = std::next(it_src);
	}
	if (it_src == it_end)
		return;
	/// MOVE INSTANCE
	it_dst = g_timeline.timeline[row].begin();
	it_end = g_timeline.timeline[row].end();
	while (it_dst != it_end) {
		if (it_dst->beat >= beat)
			break;
		it_dst = std::next(it_dst);
	}
	g_timeline.timeline[row].splice(
	/**/ it_dst, g_timeline.timeline[instance->row], it_src);
	instance->row = row;
	instance->beat = beat;
}

PatternInstance *Timeline::instance_find(int row, int beat) {
	std::list<PatternInstance>::iterator	it, it_end;

	it = g_timeline.timeline[row].begin();
	it_end = g_timeline.timeline[row].end();
	/// LOOP INSTANCES
	while (it != it_end) {
		/// CHECK INSTANCE
		if (beat >= it->beat && beat < it->beat + it->beat_length)
			return &(*it);
		/// NEXT INSTANCE
		it = std::next(it);
	}
	return NULL;
}
