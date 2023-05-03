
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerBPMDisplay::TrackerBPMDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerBPMDisplay::draw(const DrawArgs &args) {
	Rect					rect;

	LedDisplay::draw(args);

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[0]);
	//nvgRect(args.vg, RECT_ARGS(rect));
	nvgRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0);
	nvgFill(args.vg);

}

void TrackerBPMDisplay::drawLayer(const DrawArgs& args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	Vec						p;
	int						bpm;

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

	/// DRAW BPM
	bpm = module->params[Tracker::PARAM_BPM].getValue();
	itoaw(this->str, bpm, 3);
	nvgFillColor(args.vg, colors[13]);
	nvgText(args.vg, p.x + 2.0, p.y + 9.5, this->str, NULL);

	LedDisplay::drawLayer(args, layer);
}

void TrackerBPMDisplay::onButton(const ButtonEvent &e) {
	Menu			*menu;
	MenuLabel		*label;
	ParamQuantity	*quant_length;
	int				length;

	menu = createMenu();

	/// ADD LABEL
	label = new MenuLabel();
	label->text = "Edit Song";
	menu->addChild(label);

	/// ADD SONG LENGTH SLIDER
	length = g_timeline.beat_count;
	quant_length = g_module->paramQuantities[Tracker::PARAM_SONG_LENGTH];
	quant_length->setValue(length);
	quant_length->defaultValue = length;
	menu->addChild(new MenuSliderEdit(quant_length, 0));

	/// ADD SONG UPDATE BUTTON
	menu->addChild(new MenuItemStay("Update", "",
		[=]() {
			/// WAIT FOR THREAD FLAG
			while (g_timeline.thread_flag.test_and_set()) {}

			g_timeline.resize(g_module->params[Tracker::PARAM_EDIT].getValue());
			g_editor.timeline_clamp_cursor();

			/// CLEAR THREAD FLAG
			g_timeline.thread_flag.clear();
		}
	));
}
