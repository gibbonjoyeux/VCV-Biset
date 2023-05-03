
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerSynthDisplay::TrackerSynthDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerSynthDisplay::draw(const DrawArgs &args) {
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

void TrackerSynthDisplay::drawLayer(const DrawArgs& args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	Vec						p;
	int						synth;

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

	/// DRAW ACTIVE SYNTH
	synth = module->params[Tracker::PARAM_SYNTH].getValue();
	itoaw(this->str, synth, 2);
	nvgFillColor(args.vg, colors[13]);
	nvgText(args.vg, p.x + 6.0, p.y + 9.5, this->str, NULL);

	LedDisplay::drawLayer(args, layer);
}

void TrackerSynthDisplay::onButton(const ButtonEvent &e) {
	Menu			*menu;
	MenuLabel		*label;
	int				channels;
	ParamQuantity	*quant_channel;

	menu = createMenu();

	/// ADD LABEL
	label = new MenuLabel();
	label->text = "Edit Synth";
	menu->addChild(label);

	/// ADD SYNTH CHANNEL COUNT SLIDER
	channels = g_timeline.synths[g_editor.synth_id].channel_count;
	quant_channel = g_module->paramQuantities[Tracker::PARAM_SYNTH_CHANNEL_COUNT];
	quant_channel->setValue(channels);
	quant_channel->defaultValue = channels;
	menu->addChild(new MenuSliderEdit(quant_channel));

	/// ADD SYNTH UPDATE BUTTON
	menu->addChild(new MenuItemStay("Update", "",
		[=]() {
			int	channels;
			int	i;

			/// WAIT FOR THREAD FLAG
			while (g_timeline.thread_flag.test_and_set()) {}

			channels = g_module->params[Tracker::PARAM_SYNTH_CHANNEL_COUNT].getValue();
			g_timeline.synths[g_editor.synth_id].channel_count = channels;
			for (i = channels; i < 16; ++i)
				g_timeline.synths[g_editor.synth_id].voices[i].stop();

			/// CLEAR THREAD FLAG
			g_timeline.thread_flag.clear();
		}
	));
}
