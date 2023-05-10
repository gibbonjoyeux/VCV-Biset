
#include "TrackerDrum.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerDrumDisplay::TrackerDrumDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerDrumDisplay::drawLayer(const DrawArgs& args, int layer) {
	std::shared_ptr<Font>	font;
	Rect				rect;
	Vec					p;
	int					synth;
	int					synth_selected;

	font = APP->window->loadFont(font_path);
	if (layer == 1 && module && font) {
		/// GET CANVAS FORMAT
		rect = box.zeroPos();
		p = rect.getTopLeft();

		/// GET SYNTH
		synth = module->params[TrackerDrum::PARAM_SYNTH].getValue();
		if (g_module)
			synth_selected = g_module->params[Tracker::PARAM_SYNTH].getValue();
		else
			synth_selected = -1;

		/// DRAW BACKGROUND
		nvgBeginPath(args.vg);
		if (synth == synth_selected)
			nvgFillColor(args.vg, colors[14]);
		else
			nvgFillColor(args.vg, colors[15]);
		nvgRect(args.vg, RECT_ARGS(rect));
		nvgFill(args.vg);

		/// DRAW SYNTH
		itoaw(this->str_synth, synth, 2);
		nvgFontSize(args.vg, 21);
		nvgFontFaceId(args.vg, font->handle);
		nvgFillColor(args.vg, colors[4]);
		nvgText(args.vg, p.x + 35.0 / 2, p.y + 40.0 / 2, this->str_synth, NULL);
	}
	LedDisplay::drawLayer(args, layer);
}
