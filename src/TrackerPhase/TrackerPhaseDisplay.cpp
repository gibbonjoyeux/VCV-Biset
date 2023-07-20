
#include "TrackerPhase.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerPhaseDisplay::TrackerPhaseDisplay() {
	this->font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
	//LedDisplay();
}

//void TrackerPhaseDisplay::draw(const DrawArgs &args) {
//	Rect					rect;
//
//	//LedDisplay::draw(args);
//
//	/// GET CANVAS FORMAT
//	rect = box.zeroPos();
//	/// RECT BACKGROUND
//	nvgBeginPath(args.vg);
//	nvgFillColor(args.vg, colors[14]);
//	nvgRect(args.vg, RECT_ARGS(rect));
//	nvgFill(args.vg);
//}

void TrackerPhaseDisplay::drawLayer(const DrawArgs &args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	int						knob_freq;
	char					str[8];

	if (module == NULL || layer != 1)
		return;
	/// GET FONT
	font = APP->window->loadFont(this->font_path);
	if (font == NULL)
		return;
	/// SET FONT
	nvgFontSize(args.vg, 12);
	nvgFontFaceId(args.vg, font->handle);
	nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// GET TEXT
	knob_freq = this->module->params[TrackerPhase::PARAM_FREQ + this->index].getValue();
	if (knob_freq > 1)
		sprintf(str, "x%d", knob_freq);
	else if (knob_freq < -1)
		sprintf(str, "/%d", -knob_freq);
	else
		sprintf(str, "x1");
	/// DRAW BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[15]);
	nvgRect(args.vg, RECT_ARGS(rect));
	nvgFill(args.vg);
	/// DRAW TEXT
	nvgFillColor(args.vg, colors[4]);
	nvgText(args.vg,
	/**/ rect.pos.x + 3.0,
	/**/ rect.pos.y + 3.0,
	/**/ str, NULL);
	//LedDisplayTextField::drawLayer(args, layer);
}

void TrackerPhaseDisplay::onButton(const ButtonEvent &e) {
	//Vec		mouse;
	//Rect	rect;
	//int		max;
	//int		offset;

	//if (this->char_width == 0)
	//	return;
	//if (e.action != GLFW_PRESS)
	//	return;
	///// [1] UPDATE CURSOR
	//mouse = e.pos;
	//mouse.x -= 3.0;
	//mouse.y -= 3.0;
	//rect = box.zeroPos();
	//offset = 0;
	////// DISPLAY CONDENSED
	//if (this->condensed) {
	//	if (this->cursor >= 32)
	//		offset = this->cursor - 32;
	//	this->cursor = ((mouse.x - rect.pos.x) / this->char_width) + offset;
	////// DISPLAY REGULAR
	//} else {
	//	if (this->cursor >= 64)
	//		offset = ((this->cursor - 32) / 32) * 32;
	//	this->cursor = ((mouse.x - rect.pos.x) / this->char_width)
	//	/**/ + ((int)(mouse.y - rect.pos.y) / 12) * 32
	//	/**/ + offset;
	//}
	///// [2] CLAMP CURSOR
	//max = this->text.length();
	//if (this->cursor > max)
	//	this->cursor = max;
	//if (this->cursor < 0)
	//	this->cursor = 0;
	//this->selection = this->cursor;
	//e.consume(this);
}
