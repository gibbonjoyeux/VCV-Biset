
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static void on_button_left(const rack::Widget::ButtonEvent &e) {
	PatternInstance	*instance;
	int				row;
	int				beat;

	if (g_editor.pattern == NULL)
		return;
	if (e.action == GLFW_PRESS) {
		/// COMPUTE POSITION
		row = ((e.pos.y - 3.0) / (CHAR_H * 3)) + g_editor.timeline_cam_y - 1;
		beat = ((e.pos.x - 2.0) / CHAR_W - 2) + g_editor.timeline_cam_x;
		instance = g_timeline.instance_find(row, beat);
		/// CREATE INSTANCE
		if (instance == NULL) {
			g_timeline.instance_new(g_editor.pattern, row, beat);
		/// SELECT INSTANCE
		} else {
		}
	}
}

static void on_button_right(const rack::Widget::ButtonEvent &e) {
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::on_button_timeline(const ButtonEvent &e) {
	if (g_editor.pattern == NULL)
		return;
	if (e.button == GLFW_MOUSE_BUTTON_LEFT)
		on_button_left(e);
	else if (e.button == GLFW_MOUSE_BUTTON_RIGHT)
		on_button_right(e);
	e.consume(this);
}
