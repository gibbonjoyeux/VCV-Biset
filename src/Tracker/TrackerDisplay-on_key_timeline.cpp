
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::on_key_timeline(const Widget::SelectKeyEvent &e) {
	if (g_timeline->play != TIMELINE_MODE_STOP)
		return;

	/// ON PRESS / REPEAT
	if (e.action == GLFW_PRESS
	|| e.action == GLFW_REPEAT) {
		/// EVENT CAMERA MOVE
		if (e.key == GLFW_KEY_LEFT) {
			g_editor->timeline_cam_x -= 1;
			if (g_editor->timeline_cam_x < 0)
				g_editor->timeline_cam_x = 0;
			e.consume(this);
		} else if (e.key == GLFW_KEY_RIGHT) {
			g_editor->timeline_cam_x += 1;
			e.consume(this);
		} else if (e.key == GLFW_KEY_UP) {
			g_editor->timeline_cam_y -= 1;
			if (g_editor->timeline_cam_y < 0)
				g_editor->timeline_cam_y = 0;
			g_editor->pattern_move_cursor_y(-1);
			e.consume(this);
		} else if (e.key == GLFW_KEY_DOWN) {
			g_editor->timeline_cam_y += 1;
			if (g_editor->timeline_cam_y > 32 - 12)
				g_editor->timeline_cam_y = 32 - 12;
			e.consume(this);
		/// REMOVE INSTANCE
		} else if (e.key == GLFW_KEY_DELETE
		|| e.key == GLFW_KEY_BACKSPACE) {
			if (g_editor->instance != NULL) {
				g_timeline->instance_del(g_editor->instance);
				g_editor->instance = NULL;
			}
			e.consume(this);
		}
	}
}
