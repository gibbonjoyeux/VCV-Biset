
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerDisplay::TrackerDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerDisplay::draw(const DrawArgs &args) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	float					off_x, off_y;

	LedDisplay::draw(args);

	/// CAPTURE FOCUS
	if (this->moduleWidget->hasChild(APP->event->getSelectedWidget()))
		APP->event->setSelectedWidget(this->moduleWidget);

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[0]);
	//nvgRect(args.vg, RECT_ARGS(rect));
	nvgRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0);
	nvgFill(args.vg);

	if (this->module == NULL) {
		/// GET FONT
		font = APP->window->loadFont(font_path);
		if (font == NULL)
			return;
		/// BACKGROUND RECT
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, colors[15]);
		nvgRect(args.vg, rect.pos.x, rect.pos.y + 150.0, rect.size.x, 50.0);
		nvgFill(args.vg);
		/// SET FONT
		nvgFontSize(args.vg, 16);
		nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
		nvgFontFaceId(args.vg, font->handle);
		/// DRAW TITLE
		off_x = (rect.size.x / 2) - 195.0 - 10.0;
		off_y = (rect.size.y / 2) + 30.0 - 30.0;
		nvgFillColor(args.vg, colors[4]);
		nvgText(args.vg, rect.pos.x + off_x,
		/**/ rect.pos.y + off_y,
		/**/ "T  -  R  -  A  -  C  -  K  -  E  -  R", NULL);
	}
}

void TrackerDisplay::drawLayer(const DrawArgs &args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;

	if (g_module != this->module || this->module == NULL || layer != 1)
		return;
	/// GET FONT
	font = APP->window->loadFont(font_path);
	if (font == NULL)
		return;
	/// SET FONT
	nvgFontSize(args.vg, 9);
	nvgFontFaceId(args.vg, font->handle);
	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	//nvgBeginPath(args.vg);
	//nvgFillColor(args.vg, colors[0]);
	//nvgRect(args.vg, RECT_ARGS(rect));
	//nvgFill(args.vg);

	// TMP DEBUG ! ! !
	//nvgFillColor(args.vg, colors[3]);
	//nvgText(args.vg, rect.pos.x + 300, rect.pos.y + 42.0, g_timeline->debug_str, NULL);
	// TMP DEBUG ! ! !

	nvgScissor(args.vg, RECT_ARGS(rect));

	/// DRAW PATTERN
	if (g_editor->mode == EDITOR_MODE_PATTERN) {
		this->draw_pattern(args, rect);
	} else if (g_editor->mode == EDITOR_MODE_TIMELINE) {
		this->draw_timeline(args, rect);
	} else if (g_editor->mode == EDITOR_MODE_TUNING) {
		this->draw_tuning(args, rect);
	}

	nvgResetScissor(args.vg);
	LedDisplay::drawLayer(args, layer);
}

void TrackerDisplay::onSelectKey(const SelectKeyEvent &e) {

	if (g_module != this->module)
		return;

	if ((e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL)
		return;
	if (g_editor->mode == EDITOR_MODE_PATTERN) {
		this->on_key_pattern(e);
	} else if (g_editor->mode == EDITOR_MODE_TIMELINE) {
		this->on_key_timeline(e);
	}
}

void TrackerDisplay::onButton(const ButtonEvent &e) {

	if (g_module != this->module)
		return;

	if (g_editor->mode == EDITOR_MODE_PATTERN)
		this->on_button_pattern(e);
	else if (g_editor->mode == EDITOR_MODE_TIMELINE)
		this->on_button_timeline(e);
}

void TrackerDisplay::onHover(const HoverEvent &e) {

	if (g_module != this->module)
		return;

	g_editor->mouse_pos = e.pos;
}

void TrackerDisplay::onDragStart(const DragStartEvent& e) {

	if (g_module != this->module)
		return;

	/// CURSOR LOCK
	//APP->window->cursorLock();
	/// RECORD DRAG
	g_editor->mouse_button = e.button;
	g_editor->mouse_pos_drag = g_editor->mouse_pos;
	/// CALL EVENT
	if (g_editor->mode == EDITOR_MODE_PATTERN)
		;
	else if (g_editor->mode == EDITOR_MODE_TIMELINE)
		this->on_drag_start_timeline(e);
}

void TrackerDisplay::onDragMove(const DragMoveEvent& e) {
	float		zoom;

	if (g_module != this->module)
		return;

	/// COMPUTE DELTA WITH ZOOM
	zoom = APP->scene->rackScroll->getZoom();
	g_editor->mouse_pos.x += e.mouseDelta.x / zoom;
	g_editor->mouse_pos.y += e.mouseDelta.y / zoom;
	/// CALL EVENT
	if (g_editor->mode == EDITOR_MODE_PATTERN)
		;
	else if (g_editor->mode == EDITOR_MODE_TIMELINE)
		this->on_drag_move_timeline(e);
}
void TrackerDisplay::onDragEnd(const DragEndEvent& e) {

	if (g_module != this->module)
		return;

	/// CURSOR UNLOCK
	//APP->window->cursorUnlock();
	/// CALL EVENT
	if (g_editor->mode == EDITOR_MODE_PATTERN)
		;
	else if (g_editor->mode == EDITOR_MODE_TIMELINE)
		this->on_drag_end_timeline(e);
}

void TrackerDisplay::onHoverScroll(const HoverScrollEvent &e) {
	Vec		delta;

	if (g_module != this->module)
		return;

	/// CONSUME EVENT
	e.consume(this);
	/// SCROLL PATTERN
	if (g_editor->mode == EDITOR_MODE_PATTERN) {
		/// SCROLL X
		if (g_editor->mod_shift) {
			/// MOVE CURSOR
			if (e.scrollDelta.y > 0)
				g_editor->pattern_move_cursor_x(-1);
			else if (e.scrollDelta.y < 0)
				g_editor->pattern_move_cursor_x(+1);
		/// SCROLL Y
		} else {
			/// MOVE CURSOR
			if (e.scrollDelta.y > 0)
				g_editor->pattern_move_cursor_y(-1);
			else if (e.scrollDelta.y < 0)
				g_editor->pattern_move_cursor_y(+1);
		}
	/// SCROLL TIMELINE
	} else if (g_editor->mode == EDITOR_MODE_TIMELINE) {
		/// HANDLE ORIENTION
		if (g_editor->mod_shift) {
			delta.x = e.scrollDelta.y;
			delta.y = e.scrollDelta.x;
		} else {
			delta = e.scrollDelta;
		}
		/// SCROLL X
		g_editor->timeline_cam_x -= delta.x / CHAR_W;
		if (g_editor->timeline_cam_x < 0)
			g_editor->timeline_cam_x = 0;
		/// SCROLL Y
		g_editor->timeline_cam_y -= delta.y / (CHAR_H * 3.0);
		if (g_editor->timeline_cam_y < 0)
			g_editor->timeline_cam_y = 0;
		if (g_editor->timeline_cam_y > 32 - 12)
			g_editor->timeline_cam_y = 32 - 12;
	}
}
