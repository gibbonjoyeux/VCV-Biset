
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
	Synth			*synth;
	ParamQuantity	*quant_channel;

	menu = createMenu();

	/// ADD LABEL
	label = new MenuLabel();
	label->text = "Edit synth";
	menu->addChild(label);

	synth = &(g_timeline.synths[g_editor.synth_id]);
	/// ADD SYNTH MODE LIST
	menu->addChild(rack::createSubmenuItem("Mode", "",
		[=](Menu *menu) {
			ParamQuantity		*quant_mode;

			quant_mode = g_module->paramQuantities[Tracker::PARAM_SYNTH_MODE];
			quant_mode->setValue(synth->mode);
			menu->addChild(new MenuCheckItem("Gate", "",
				[=]() { return quant_mode->getValue() == SYNTH_MODE_GATE; },
				[=]() { quant_mode->setValue(SYNTH_MODE_GATE); }
			));
			menu->addChild(new MenuCheckItem("Trigger", "",
				[=]() { return quant_mode->getValue() == SYNTH_MODE_TRIGGER; },
				[=]() { quant_mode->setValue(SYNTH_MODE_TRIGGER); }
			));
			menu->addChild(new MenuCheckItem("Drum", "",
				[=]() { return quant_mode->getValue() == SYNTH_MODE_DRUM; },
				[=]() { quant_mode->setValue(SYNTH_MODE_DRUM); }
			));
		}
	));
	/// ADD SYNTH CHANNEL COUNT SLIDER
	quant_channel = g_module->paramQuantities[Tracker::PARAM_SYNTH_CHANNEL_COUNT];
	quant_channel->setValue(synth->channel_count);
	quant_channel->defaultValue = synth->channel_count;
	menu->addChild(new MenuSliderEdit(quant_channel));

	/// ADD SYNTH UPDATE BUTTON
	menu->addChild(new MenuItemStay("Update synth", "",
		[=]() {
			int	channels;
			int	mode;
			int	i;

			/// WAIT FOR THREAD FLAG
			while (g_timeline.thread_flag.test_and_set()) {}

			/// SET SYNTH MODE
			mode = g_module->params[Tracker::PARAM_SYNTH_MODE].getValue();
			g_timeline.synths[g_editor.synth_id].mode = mode;
			/// SET SYNTH CHANNEL COUNT
			channels = g_module->params[Tracker::PARAM_SYNTH_CHANNEL_COUNT].getValue();
			g_timeline.synths[g_editor.synth_id].channel_count = channels;
			for (i = channels; i < 16; ++i)
				g_timeline.synths[g_editor.synth_id].voices[i].stop();

			/// CLEAR THREAD FLAG
			g_timeline.thread_flag.clear();
		}
	));
}
