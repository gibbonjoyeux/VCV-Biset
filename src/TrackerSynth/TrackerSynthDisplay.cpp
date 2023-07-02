
#include "TrackerSynth.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerSynthDisplay::TrackerSynthDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerSynthDisplay::drawLayer(const DrawArgs& args, int layer) {
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
		synth = module->params[TrackerSynth::PARAM_SYNTH].getValue();
		synth_selected = g_editor.synth_id;
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
		/// DRAW MAPPING LEARN NOTIF
		if (this->module->map_learn) {
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, colors[2]);
			nvgRect(args.vg, rect.pos.x + rect.size.x - 6.5, rect.pos.y + 2, 5, 5);
			nvgFill(args.vg);
		}
	}
	LedDisplay::drawLayer(args, layer);
}

void TrackerSynthDisplay::onButton(const ButtonEvent &e) {
	Menu	*menu;
	Param	*param;
	int		i;

	if (e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS)
		return;

	param = &(this->module->params[TrackerSynth::PARAM_SYNTH]);

	menu = createMenu();
	/// ADD COLOR SUB-MENU
	for (i = 0; i < g_timeline.synth_count; ++i) {
		menu->addChild(new MenuCheckItem(g_timeline.synths[i].name, "",
			[=]() { return param->getValue() == i; },
			[=]() { param->setValue(i); }
		));
	}
}
