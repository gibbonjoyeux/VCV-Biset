
#include "plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerInfoDisplay::TrackerInfoDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerInfoDisplay::drawLayer(const DrawArgs& args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	Vec						p;
	int						bpm;
	int						synth;
	int						pattern;

	if (module == NULL || layer != 1)
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
	p = rect.getTopLeft();
	p.y -= 1.0;
	/// BACKGROUND
	//// BACKGROUND BPM
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[1]);
	//nvgRect(args.vg, RECT_ARGS(rect));
	nvgRect(args.vg, p.x, p.y, CHAR_W * 3 + 5.0, CHAR_H + 4.0);
	nvgFill(args.vg);
	//// BACKGROUND SYNTH
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[1]);
	nvgRect(args.vg, p.x + 33.0, p.y, CHAR_W * 3 + 5.0, CHAR_H + 4.0);
	nvgFill(args.vg);
	//// BACKGROUND PATTERN
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[1]);
	nvgRect(args.vg, p.x + 66.0, p.y, CHAR_W * 3 + 5.0, CHAR_H + 4.0);
	nvgFill(args.vg);

	/// DRAW BPM
	bpm = module->params[Tracker::PARAM_BPM].getValue();
	itoaw(str_bpm, bpm, 3);
	nvgFillColor(args.vg, colors[13]);
	nvgText(args.vg, p.x + 2.0, p.y + 9.5, str_bpm, NULL);
	/// DRAW ACTIVE SYNTH
	synth = module->params[Tracker::PARAM_SYNTH].getValue();
	itoaw(str_bpm, synth, 2);
	nvgFillColor(args.vg, colors[13]);
	nvgText(args.vg, p.x + 6.0 + 33.0, p.y + 9.5, str_bpm, NULL);
	/// DRAW ACTIVE PATTERN
	pattern = module->params[Tracker::PARAM_PATTERN].getValue();
	itoaw(str_bpm, pattern, 3);
	nvgFillColor(args.vg, colors[13]);
	nvgText(args.vg, p.x + 2.0 + 66.5, p.y + 9.5, str_bpm, NULL);

	LedDisplay::drawLayer(args, layer);
}
