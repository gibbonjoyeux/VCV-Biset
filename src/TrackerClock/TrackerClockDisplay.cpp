
#include "TrackerClock.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerClockDisplay::TrackerClockDisplay() {
	this->font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerClockDisplay::draw(const DrawArgs &args) {
	Rect					rect;

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// RECT BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[15]);
	nvgRect(args.vg, RECT_ARGS(rect));
	nvgFill(args.vg);
}

void TrackerClockDisplay::drawLayer(const DrawArgs &args, int layer) {
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
	knob_freq = this->module->params[TrackerClock::PARAM_FREQ + this->index].getValue();
	if (knob_freq > 1)
		sprintf(str, "x%d", knob_freq);
	else if (knob_freq < -1)
		sprintf(str, "/%d", -knob_freq);
	else
		sprintf(str, "x1");
	/// DRAW TEXT
	nvgFillColor(args.vg, colors[4]);
	nvgText(args.vg,
	/**/ rect.pos.x + 3.0,
	/**/ rect.pos.y + 3.0,
	/**/ str, NULL);
	//LedDisplayTextField::drawLayer(args, layer);
}

void TrackerClockDisplay::onButton(const ButtonEvent &e) {
}
