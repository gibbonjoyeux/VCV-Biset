
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerDisplaySide::TrackerDisplaySide() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerDisplaySide::draw(const DrawArgs &args) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	float					off_x, off_y;

	LedDisplay::draw(args);

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[0]);
	nvgRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0);
	nvgFill(args.vg);
}

void TrackerDisplaySide::draw_synths(const DrawArgs &args, Rect rect) {
	char		str[256 + 5];
	float		x, y, w, h;
	Vec			p;
	int			i;

	p = rect.getTopLeft();
	x = p.x;
	w = CHAR_W * 26;
	h = CHAR_H * 3 - 4;
	for (i = 0; i < 13; ++i) {
		y = p.y + CHAR_H * 3 * i + 6;
		/// FILL
		nvgBeginPath(args.vg);
		if (g_editor.side_mouse_pos.y > y && g_editor.side_mouse_pos.y < y + h)
			nvgFillColor(args.vg, colors[1]);
		else
			nvgFillColor(args.vg, colors[2]);
		nvgRoundedRect(args.vg, x, y, w, h, 5);
		nvgFill(args.vg);
		/// TEXT
		sprintf(str, "%0.2d : %s",
		/**/ i,
		/**/ "Synth name");
		nvgFillColor(args.vg, colors[12]);
		nvgTextBox(args.vg,
		/**/ x + 3.0, y + 3.0,
		/**/ CHAR_W * 26,
		/**/ str, NULL);
	}
}

void TrackerDisplaySide::draw_patterns(const DrawArgs &args, Rect rect) {
}

void TrackerDisplaySide::drawLayer(const DrawArgs &args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;

	if (module == NULL || layer != 1)
		return;
	/// GET FONT
	font = APP->window->loadFont(font_path);
	if (font == NULL)
		return;
	/// SET FONT
	nvgFontSize(args.vg, 9);
	nvgFontFaceId(args.vg, font->handle);
	nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	/// GET CANVAS FORMAT
	rect = box.zeroPos();

	nvgScissor(args.vg, RECT_ARGS(rect));
	/// DRAW
	if (g_editor.mode == EDITOR_MODE_PATTERN) {
		this->draw_synths(args, rect);
	} else if (g_editor.mode == EDITOR_MODE_TIMELINE) {
		this->draw_patterns(args, rect);
	}
	nvgResetScissor(args.vg);
	LedDisplay::drawLayer(args, layer);
}

void TrackerDisplaySide::onHover(const HoverEvent &e) {
	g_editor.side_mouse_pos = e.pos;
}

void TrackerDisplaySide::onButton(const ButtonEvent &e) {
	g_editor.side_mouse_button = e.button;
	g_editor.side_mouse_action = e.action;
}
