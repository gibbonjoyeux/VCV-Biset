
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
	PatternInstance							instance(source, beat);

	/// INIT INSTANCE DATA
	instance.source = source;
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
	g_timeline.timeline[row].insert(it_next, instance);
	return NULL;
}

void Timeline::instance_del(PatternInstance *instance) {
}

void Timeline::instance_move(PatternInstance *instance, int row, int beat) {
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
