
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void TrackerDisplayBPM::onButton(const ButtonEvent &e) {
	//Menu			*menu;
	//MenuLabel		*label;
	//ParamQuantity	*quant_length;
	//int				length;

	//if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
	//	menu = createMenu();

	//	/// ADD LABEL
	//	label = new MenuLabel();
	//	label->text = "Edit song";
	//	menu->addChild(label);

	//	/// ADD SONG LENGTH SLIDER
	//	length = g_timeline.beat_count;
	//	quant_length = g_module->paramQuantities[Tracker::PARAM_SONG_LENGTH];
	//	quant_length->setValue(length);
	//	quant_length->defaultValue = length;
	//	menu->addChild(new MenuSliderEdit(quant_length, 0));

	//	/// ADD SONG UPDATE BUTTON
	//	menu->addChild(new MenuItemStay("Update song", "",
	//		[=]() {
	//			/// WAIT FOR THREAD FLAG
	//			while (g_timeline.thread_flag.test_and_set()) {}

	//			g_timeline.resize(g_module->params[Tracker::PARAM_SONG_LENGTH].getValue());
	//			g_editor.timeline_clamp_cursor();

	//			/// CLEAR THREAD FLAG
	//			g_timeline.thread_flag.clear();
	//		}
	//	));
	//}
}
