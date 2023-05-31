
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
		/// EDIT INSTANCE
		} else {
			/// SELECT INSTANCE
			g_editor.instance = instance;
			g_editor.instance_row = row;
			g_editor.instance_beat = instance->beat;
			/// SELECT INSTANCE PATTERN
			g_editor.pattern = instance->source;
			g_editor.pattern_id = ((intptr_t)instance->source
			/**/ - (intptr_t)g_timeline.patterns) / sizeof(PatternSource);
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

void TrackerDisplay::on_drag_start_timeline(const DragStartEvent& e) {
	//APP->window->cursorLock();
}

void TrackerDisplay::on_drag_move_timeline(const DragMoveEvent& e) {
	Vec		delta;
	int		row;
	int		beat;

	if (g_editor.instance == NULL)
		return;
	/// [1] COMPUTE INSTANCE POSITION
	//// COMPUTE BASE POSITION
	delta.x = g_editor.mouse_pos.x - g_editor.mouse_pos_drag.x;
	delta.y = g_editor.mouse_pos.y - g_editor.mouse_pos_drag.y;
	row = ((g_editor.mouse_pos.y - 3.0) / (CHAR_H * 3)) + g_editor.timeline_cam_y - 1;
	beat = g_editor.instance_beat + (delta.x / CHAR_W);
	//// LOCK AXIS
	if (APP->window->getMods() & GLFW_MOD_SHIFT) {
		/// ONLY X (BEAT)
		if (std::abs(delta.x) > std::abs(delta.y))
			row = g_editor.instance_row;
		/// ONLY Y (ROW)
		else
			beat = g_editor.instance_beat;
	}
	//// BOUND MOVEMENT
	if (row < 0)
		row = 0;
	if (row > 31)
		row = 31;
	if (beat < 0)
		beat = 0;
	/// [2] MOVE INSTANCE
	if (g_editor.instance->row != row || g_editor.instance->beat != beat)
		g_timeline.instance_move(g_editor.instance, row, beat);
}

void TrackerDisplay::on_drag_end_timeline(const DragEndEvent& e) {
	//APP->window->cursorUnlock();
}
