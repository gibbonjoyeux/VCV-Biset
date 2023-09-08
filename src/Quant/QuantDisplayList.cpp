
#include "Quant.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

QuantDisplayList::QuantDisplayList() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
	this->scroll = 0.0;
}

void QuantDisplayList::draw_notes(const DrawArgs &args, Rect rect) {
	char		name[256];
	QuantNote	*note;
	int			color;
	float		x, y, w, h;
	Vec			p;
	int			length;
	int			index;
	int			i;

	p = rect.getTopLeft();
	x = p.x;
	w = CHAR_W * 20;
	h = CHAR_H * 3 - 4;
	length = this->module->notes.size();
	color = 3;
	/// GET ITEM NAME & COLOR & STATUS ?
	for (i = 0; i < 13; ++i) {
		y = p.y + CHAR_H * 3 * i + 6;
		index = i + (int)this->scroll;
		/// DRAW ITEM
		if (index < length) {
			note = &(this->module->notes[index]);
			/// FILL
			//// MAIN ROUND RECT
			nvgBeginPath(args.vg);
			if ((this->moduleWidget->note
			&& index == this->moduleWidget->note_id)
			|| (this->mouse_pos.y > y
			&& this->mouse_pos.y < y + h))
				nvgFillColor(args.vg, colors[13]);
			else
				nvgFillColor(args.vg, colors[14]);
			nvgRoundedRect(args.vg, x, y, w - 10, h, 5);
			nvgFill(args.vg);
			//// COLORED ROUND RECT
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, colors_user[color]);
			nvgRoundedRect(args.vg, x + w - 15, y, 15, h, 5);
			nvgFill(args.vg);
			//// COLORED SQUARE RECT
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, colors_user[color]);
			nvgRect(args.vg, x + w - 15, y, 5, h);
			nvgFill(args.vg);
			/// TEXT
			if (note->type == QUANT_CENT) {
				sprintf(name, "%d - %.6f", index + 1, note->numerator);
			} else {
				sprintf(name, "%d - %.0f:%.0f", index + 1,
				/**/ note->numerator, note->denominator);
			}
			nvgFillColor(args.vg, colors[12]);
			nvgTextBox(args.vg,
			/**/ x + 3.0, y + 3.0,
			/**/ CHAR_W * 23,
			/**/ name, NULL);
		/// DRAW ADD ITEM
		} else {
			/// FILL
			nvgBeginPath(args.vg);
			if (this->mouse_pos.y > y && this->mouse_pos.y < y + h)
				nvgFillColor(args.vg, colors[3]);
			else
				nvgFillColor(args.vg, colors[2]);
			nvgRoundedRect(args.vg, x, y, w, h, 5);
			nvgFill(args.vg);
			/// TEXT
			nvgFillColor(args.vg, colors[0]);
			nvgTextBox(args.vg,
			/**/ x + w / 2.0 - CHAR_W / 2.0, y + 6.0,
			/**/ CHAR_W * 20,
			/**/ "+", NULL);
			return;
		}
	}
}

void QuantDisplayList::draw(const DrawArgs &args) {
	std::shared_ptr<Font>	font;
	Rect					rect;

	LedDisplay::draw(args);

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[0]);
	nvgRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0);
	nvgFill(args.vg);
}

void QuantDisplayList::drawLayer(const DrawArgs &args, int layer) {
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

	/// DRAW
	nvgScissor(args.vg, RECT_ARGS(rect));
	this->draw_notes(args, rect);
	nvgResetScissor(args.vg);
	LedDisplay::drawLayer(args, layer);
}

void QuantDisplayList::onHover(const HoverEvent &e) {
	this->mouse_pos = e.pos;
}

void QuantDisplayList::onButton(const ButtonEvent &e) {
	int		length;
	int		index;

	this->mouse_pos = e.pos;

	this->mouse_button = e.button;
	this->mouse_action = e.action;

	if (e.action != GLFW_PRESS)
		return;

	e.consume(this);

	index = (e.pos.y - 6) / (CHAR_H * 3) + (int)this->scroll;
	length = this->module->notes.size();
	/// CLICK ON NOTE
	if (index < length) {
		/// SELECT NOTE
		this->moduleWidget->note_id = index;
		this->moduleWidget->note = &(this->module->notes[index]);
		/// CLICK RIGHT
		if (e.button == GLFW_MOUSE_BUTTON_RIGHT)
			;//menu_synth(&(g_timeline.synths[index]));
	/// CLICK ON ADD NOTE
	} else if (index == length) {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
			//if ((int)this->scroll == length - 12)
			//	this->scroll = (int)(this->scroll) + 1;
			//this->module->note_add(0.0);
			this->module->note_add_menu();
		}
	}
}

void QuantDisplayList::onLeave(const LeaveEvent &e) {
	this->mouse_pos = {0, 0};
}

void QuantDisplayList::onHoverScroll(const HoverScrollEvent &e) {
	int			length;

	/// CONSUME EVENT
	e.consume(this);
	/// UPDATE SCROLL
	length = this->module->notes.size();
	this->scroll -= e.scrollDelta.y / (CHAR_H * 3.0);
	if (this->scroll > length - 12)
		this->scroll = length - 12;
	if (this->scroll < 0)
		this->scroll = 0;
}

void QuantDisplayList::onSelectKey(const SelectKeyEvent &e) {
	//Synth			*synth_a;
	//Synth			*synth_b;
	//PatternSource	*pattern_a;
	//PatternSource	*pattern_b;

	//if ((e.action == GLFW_PRESS || e.action == GLFW_REPEAT)
	//&& (e.key == GLFW_KEY_UP || e.key == GLFW_KEY_DOWN)) {
	//	e.consume(this);
	//	if (g_timeline.play != TIMELINE_MODE_STOP)
	//		return;
	//	/// MOVE SYNTH
	//	if (g_editor.mode == EDITOR_MODE_PATTERN) {
	//		if (g_editor.synth) {
	//			synth_a = g_editor.synth;
	//			/// MOVE UP
	//			if (e.key == GLFW_KEY_UP) {
	//				if (g_editor.synth_id <= 0)
	//					return;
	//				synth_b = &(g_timeline.synths[g_editor.synth_id - 1]);
	//				g_editor.synth_id -= 1;
	//			/// MOVE DOWN
	//			} else {
	//				if (g_editor.synth_id >= g_timeline.synth_count - 1)
	//					return;
	//				synth_b = &(g_timeline.synths[g_editor.synth_id + 1]);
	//				g_editor.synth_id += 1;
	//			}
	//			g_timeline.synth_swap(synth_a, synth_b);
	//			/// RE-SELECT MOVED SYNTH
	//			g_editor.synth = synth_b;
	//		}
	//	/// MOVE PATTERN
	//	} else if (g_editor.mode == EDITOR_MODE_TIMELINE) {
	//		if (g_editor.pattern) {
	//			pattern_a = g_editor.pattern;
	//			/// MOVE UP
	//			if (e.key == GLFW_KEY_UP) {
	//				if (g_editor.pattern_id <= 0)
	//					return;
	//				pattern_b = &(g_timeline.patterns[g_editor.pattern_id - 1]);
	//				g_editor.pattern_id -= 1;
	//			/// MOVE DOWN
	//			} else {
	//				if (g_editor.pattern_id >= g_timeline.pattern_count - 1)
	//					return;
	//				pattern_b = &(g_timeline.patterns[g_editor.pattern_id + 1]);
	//				g_editor.pattern_id += 1;
	//			}
	//			g_timeline.pattern_swap(pattern_a, pattern_b);
	//			/// RE-SELECT MOVED SYNTH
	//			g_editor.pattern = pattern_b;
	//		}
	//	}
	//}
}
