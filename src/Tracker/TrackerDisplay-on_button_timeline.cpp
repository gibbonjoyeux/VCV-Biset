
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static void on_button_left(const rack::Widget::ButtonEvent &e) {
	PatternInstance	*instance;
	int				row;
	int				beat;

	if (e.action == GLFW_PRESS) {
		/// COMPUTE POSITION
		row = (int)((e.pos.y - 3) / (CHAR_H * 3)) + g_editor->timeline_cam_y - 1;
		beat = (int)((e.pos.x - 2) / CHAR_W - 2) + g_editor->timeline_cam_x;
		if (row < 0 || beat < 0) {
			g_editor->instance = NULL;
			return;
		}
		instance = g_timeline->instance_find(row, beat);
		/// CREATE INSTANCE
		if (instance == NULL) {
			if (g_timeline->play != TIMELINE_MODE_STOP)
				return;
			if (g_editor->pattern == NULL)
				return;
			g_timeline->instance_new(g_editor->pattern, row, beat);
			g_editor->instance_handle = INSTANCE_HANDLE_MIDDLE;
		/// EDIT INSTANCE
		} else {
			/// SELECT INSTANCE
			g_editor->instance = instance;
			g_editor->instance_row = row;
			g_editor->instance_beat = instance->beat;
			/// SELECT INSTANCE PATTERN
			g_editor->set_pattern(((intptr_t)instance->source
			/**/ - (intptr_t)g_timeline->patterns) / sizeof(PatternSource));
			if (g_timeline->play != TIMELINE_MODE_STOP)
				return;
			/// SELECT INSTANCE MODE
			if (beat >= instance->beat + instance->beat_length - 1)
				g_editor->instance_handle = INSTANCE_HANDLE_RIGHT;
			else if (beat <= instance->beat)
				g_editor->instance_handle = INSTANCE_HANDLE_LEFT;
			else
				g_editor->instance_handle = INSTANCE_HANDLE_MIDDLE;
		}
	}
}

static void on_button_right(const rack::Widget::ButtonEvent &e) {
	Menu			*menu;
	PatternInstance	*instance;
	int				row;
	int				beat;

	if (g_timeline->play != TIMELINE_MODE_STOP)
		return;
	if (e.action == GLFW_PRESS) {
		/// COMPUTE POSITION
		row = (int)((e.pos.y - 3) / (CHAR_H * 3)) + g_editor->timeline_cam_y - 1;
		beat = (int)((e.pos.x - 2) / CHAR_W - 2) + g_editor->timeline_cam_x;
		instance = g_timeline->instance_find(row, beat);
		/// EDIT INSTANCE
		if (instance != NULL) {
			/// SELECT INSTANCE
			g_editor->instance = instance;
			g_editor->instance_row = row;
			g_editor->instance_beat = instance->beat;
			/// SELECT INSTANCE PATTERN
			g_editor->set_pattern(((intptr_t)instance->source
			/**/ - (intptr_t)g_timeline->patterns) / sizeof(PatternSource));
			/// ADD INSTANCE CONTEXT MENU
			menu = createMenu();
			menu->addChild(new MenuCheckItem("Mute", "",
				[=]() { return instance->muted; },
				[=]() { instance->muted = !instance->muted; }
			));
			menu->addChild(rack::createMenuItem("Delete", "", [=]() {
					g_timeline->instance_del(g_editor->instance);
					g_editor->instance = NULL;
				}
			));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::on_button_timeline(const ButtonEvent &e) {
	e.consume(this);
	if (e.button == GLFW_MOUSE_BUTTON_LEFT)
		on_button_left(e);
	else if (e.button == GLFW_MOUSE_BUTTON_RIGHT)
		on_button_right(e);
}

void TrackerDisplay::on_drag_start_timeline(const DragStartEvent& e) {
	//APP->window->cursorLock();
}

void TrackerDisplay::on_drag_move_timeline(const DragMoveEvent& e) {
	Vec		delta;
	int		row;
	int		beat;
	int		start;
	int		length;

	if (g_timeline->play != TIMELINE_MODE_STOP)
		return;
	if (g_editor->instance == NULL)
		return;
	if (e.button != GLFW_MOUSE_BUTTON_LEFT)
		return;
	/// MOVE INSTANCE
	if (g_editor->instance_handle == INSTANCE_HANDLE_MIDDLE) {
		/// COMPUTE BASE POSITION
		delta.x = g_editor->mouse_pos.x - g_editor->mouse_pos_drag.x;
		delta.y = g_editor->mouse_pos.y - g_editor->mouse_pos_drag.y;
		row = (int)((g_editor->mouse_pos.y - 3.0) / (CHAR_H * 3)) + g_editor->timeline_cam_y - 1;
		beat = g_editor->instance_beat + (delta.x / CHAR_W);
		/// LOCK AXIS
		if (APP->window->getMods() & GLFW_MOD_SHIFT) {
			/// ONLY X (BEAT)
			if (std::abs(delta.x) > std::abs(delta.y))
				row = g_editor->instance_row;
			/// ONLY Y (ROW)
			else
				beat = g_editor->instance_beat;
		}
		/// BOUND MOVEMENT
		if (row < 0)
			row = 0;
		if (row > 31)
			row = 31;
		if (beat < 0)
			beat = 0;
		/// MOVE INSTANCE
		if (g_editor->instance->row != row || g_editor->instance->beat != beat)
			g_timeline->instance_move(g_editor->instance, row, beat);
	/// RESIZE INSTANCE (RIGHT)
	} else if (g_editor->instance_handle == INSTANCE_HANDLE_RIGHT) {
		beat = (int)((g_editor->mouse_pos.x - 2.0) / CHAR_W - 2) + g_editor->timeline_cam_x;
		length = beat - g_editor->instance->beat + 1;
		if (length < 1)
			length = 1;
		g_editor->instance->beat_length = length;
	/// RESIZE INSTANCE (LEFT)
	} else if (g_editor->instance_handle == INSTANCE_HANDLE_LEFT) {
		beat = (int)((g_editor->mouse_pos.x - 2.0) / CHAR_W - 2) + g_editor->timeline_cam_x;
		if (beat < 0)
			beat = 0;
		if (beat > g_editor->instance->beat + g_editor->instance->beat_length - 1)
			beat = g_editor->instance->beat + g_editor->instance->beat_length - 1;
		length = g_editor->instance->beat_length - (beat - g_editor->instance->beat);
		if (length < 1)
			length = 1;
		start = g_editor->instance->beat_start + (beat - g_editor->instance->beat);
		g_timeline->instance_move(g_editor->instance, g_editor->instance_row, beat);
		g_editor->instance->beat_start = start;
		g_editor->instance->beat_length = length;
	}
}

void TrackerDisplay::on_drag_end_timeline(const DragEndEvent& e) {
	//APP->window->cursorUnlock();
}
