
#include "TrackerOut.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerOutDisplay::TrackerOutDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerOutDisplay::drawLayer(const DrawArgs& args, int layer) {
	std::shared_ptr<Font>	font;
	Rect				rect;
	Vec					p;
	int					synth;

	font = APP->window->loadFont(font_path);
	if (layer == 1 && module && font) {
		/// GET CANVAS FORMAT
		rect = box.zeroPos();
		p = rect.getTopLeft();

		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, colors[0]);
		nvgRect(args.vg, RECT_ARGS(rect));
		nvgFill(args.vg);

		synth = module->params[TrackerOut::PARAM_SYNTH].getValue();
		//itoa(synth, str_synth, 10);
		itoaw(this->str_synth, synth, 2);
		nvgFontSize(args.vg, 42);
		nvgFontFaceId(args.vg, font->handle);
		nvgFillColor(args.vg, colors[4]);
		nvgText(args.vg, p.x + 35.0, p.y + 40.0, this->str_synth, NULL);
	}
	LedDisplay::drawLayer(args, layer);
}
