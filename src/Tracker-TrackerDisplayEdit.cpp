
#include "plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static void text(const Widget::DrawArgs &args, Vec p, char *title,
	int value_src, int value_new, int color, int y) {
	static char			string[16];

	/// SET COLOR
	nvgFillColor(args.vg, colors[color]);
	/// DRAW TITLE
	nvgText(args.vg,
	/**/ p.x + 3.5,
	/**/ p.y + 3.5 + CHAR_H * y * 3,
	/**/ title, NULL);
	/// DRAW VALUE
	//// VALUE UNCHANGED
	if (value_src == value_new) {
		itoa(value_src, string, 10);
		nvgText(args.vg,
		/**/ p.x + 3.5,
		/**/ p.y + 3.5 + CHAR_H * (y * 3 + 1),
		/**/ string, NULL);
	//// VALUE CHANGED
	} else {
		itoa(value_new, string, 10);
		nvgFillColor(args.vg, colors[2]);
		nvgText(args.vg,
		/**/ p.x + 3.5,
		/**/ p.y + 3.5 + CHAR_H * (y * 3 + 1),
		/**/ string, NULL);
	}
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerEditDisplay::TrackerEditDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerEditDisplay::drawLayer(const DrawArgs& args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	Vec						p;

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
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[0]);
	nvgRect(args.vg, RECT_ARGS(rect));
	nvgFill(args.vg);


	nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

	/// SONG LENGTH
	text(args, p, (char*)"Song length",
	/**/ g_timeline.beat_count,
	/**/ module->params[Tracker::PARAM_EDIT + 0].getValue(),
	/**/ 6, 0);
	/// SYNTH CHANNELS
	text(args, p, (char*)"Synth channel",
	/**/ g_timeline.synths[g_editor.synth_id].channel_count,
	/**/ module->params[Tracker::PARAM_EDIT + 1].getValue(),
	/**/ 5, 1);
	/// PATTERN LENGTH
	text(args, p, (char*)"Pattern lengt",
	/**/ g_editor.pattern->beat_count,
	/**/ module->params[Tracker::PARAM_EDIT + 2].getValue(),
	/**/ 10, 2);
	/// PATTERN LINES PER BEAT
	text(args, p, (char*)"Pattern lpb",
	/**/ g_editor.pattern->lpb,
	/**/ module->params[Tracker::PARAM_EDIT + 3].getValue(),
	/**/ 10, 3);
	/// PATTERN NOTE ROW
	text(args, p, (char*)"Pattern notes",
	/**/ g_editor.pattern->note_count,
	/**/ module->params[Tracker::PARAM_EDIT + 4].getValue(),
	/**/ 10, 4);
	/// PATTERN CV ROW
	text(args, p, (char*)"Pattern cv",
	/**/ g_editor.pattern->cv_count,
	/**/ module->params[Tracker::PARAM_EDIT + 5].getValue(),
	/**/ 10, 5);
	/// PATTERN MODE
	if (g_editor.mode == EDITOR_MODE_PATTERN) {
		/// NOTE ROW
		if (g_editor.pattern_row < g_editor.pattern->note_count) {
			text(args, p, (char*)"Pattern Note",
			/**/ 0, 0,
			/**/ 9, 6);
		/// CV ROW
		} else {
			text(args, p, (char*)"Pattern CV",
			/**/ 0, 0,
			/**/ 9, 6);
		}
	}
	

	//for (i = 0; i < 8; ++i) {
	//	nvgText(args.vg, p.x + 3.5, p.y + 10.0 + CHAR_H * i * 3, "line title", NULL);
	//	nvgText(args.vg, p.x + 3.5, p.y + 10.0 + CHAR_H * (i * 3 + 1), " line conten", NULL);
	//}

	LedDisplay::drawLayer(args, layer);
}
