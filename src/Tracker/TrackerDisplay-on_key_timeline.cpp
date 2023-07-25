
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::on_key_timeline(const Widget::SelectKeyEvent &e) {
	e.consume(this);
	if (g_timeline->play != TIMELINE_MODE_STOP)
		return;
	if (e.action == GLFW_PRESS
	|| e.action == GLFW_REPEAT) {
		/// REMOVE INSTANCE
		if (e.key == GLFW_KEY_DELETE
		|| e.key == GLFW_KEY_BACKSPACE) {
			if (g_editor->instance != NULL) {
				g_timeline->instance_del(g_editor->instance);
				g_editor->instance = NULL;
			}
		}
	}
}
