
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void TrackerSynthDisplay::onButton(const ButtonEvent &e) {
	Menu			*menu;
	MenuLabel		*label;
	int				channels;
	ParamQuantity	*quant_channel;

	menu = createMenu();

	/// ADD LABEL
	label = new MenuLabel();
	label->text = "Edit synth";
	menu->addChild(label);

	/// ADD SYNTH CHANNEL COUNT SLIDER
	channels = g_timeline.synths[g_editor.synth_id].channel_count;
	quant_channel = g_module->paramQuantities[Tracker::PARAM_SYNTH_CHANNEL_COUNT];
	quant_channel->setValue(channels);
	quant_channel->defaultValue = channels;
	menu->addChild(new MenuSliderEdit(quant_channel));

	/// ADD SYNTH UPDATE BUTTON
	menu->addChild(new MenuItemStay("Update synth", "",
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
