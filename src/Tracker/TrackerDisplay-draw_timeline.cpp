
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::draw_timeline(const DrawArgs &args, Rect rect) {
	list<PatternInstance>::iterator	it, it_end;
	bool							visible;
	int								inst_x, inst_w;
	Vec								p;
	float							x, y;
	char							str[32];
	int								i, j, k;
	int								index;
	int								length;
	int								color;

	p = rect.getTopLeft();
	/// DRAW BEAT / BAR COUNT
	y = p.y + 11.0;
	for (i = 0; i < 85; ++i) {
		index = i + g_editor.timeline_cam_x;
		x = p.x + 2.0 + CHAR_W * (i + 2);
		if (index % 4 == 0) {
			nvgFillColor(args.vg, colors[13]);
			itoaw(str, index / 4, 3);
		} else {
			nvgFillColor(args.vg, colors[15]);
			itoaw(str, index % 4, 3);
		}
		nvgTextBox(args.vg, x, y, CHAR_W * 1.5, str, NULL);
	}
	/// DRAW ROWS COUNT
	for (i = 0; i < 12; ++i) {
		index = i + g_editor.timeline_cam_y;
		/// COL COUNT
		x = p.x + 2.0;
		y = p.y + 11.0 + CHAR_H * ((i * 3) + 3 + 1);
		if (index % 2 == 0)
			nvgFillColor(args.vg, colors[14]);
		else
			nvgFillColor(args.vg, colors[13]);
		itoaw(str, index, 2);
		nvgText(args.vg, x, y, str, NULL);
	}
	/// DRAW BAR MARKERS
	for (i = 4 - (int)g_editor.timeline_cam_x % 4; i < 85; i += 4) {
		x = p.x + 2.0 + CHAR_W * (i + 2);
		y = p.y + 13.0 + CHAR_H * 2;
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, colors[15]);
		nvgRect(args.vg, x, y, 1, CHAR_H * 12 * 3);
		nvgFill(args.vg);
	}

	/// DRAW PATTERNS
	//// FOR EACH ROW
	for (i = 0; i < 12; ++i) {
		index = i + g_editor.timeline_cam_y;
		y = p.y + 13.0 + CHAR_H * (i * 3 + 2);
		it = g_timeline.timeline[index].begin();
		it_end = g_timeline.timeline[index].end();
		while (it != it_end) {
			/// CHECK VISIBILITY
			//// PATTERN BEFORE
			if (it->beat < g_editor.timeline_cam_x) {
				if (it->beat + it->beat_length >= g_editor.timeline_cam_x)
					visible = true;
				else
					visible = false;
			//// PATTERN INSIDE
			} else if (it->beat < g_editor.timeline_cam_x + 85) {
				visible = true;
			//// PATTERN AFTER
			} else {
				break;
			}
			/// DRAW IF VISIBLE
			if (visible == true) {
				inst_x = it->beat - g_editor.timeline_cam_x;
				inst_w = it->beat_length;
				x = p.x + 2.0 + CHAR_W * (inst_x + 2);
				/// FILL
				nvgBeginPath(args.vg);
				nvgFillColor(args.vg, colors[2]);
				nvgRoundedRect(args.vg, x + 1, y + 2, CHAR_W * inst_w - 1, CHAR_H * 3 - 4, 5);
				nvgFill(args.vg);
				/// TEXT
				nvgFillColor(args.vg, colors[12]);
				nvgText(args.vg, x + 3.0, y + CHAR_H * 2 - 2.0, "Name", NULL);
			}
			/// NEXT INSTANCE
			it = std::next(it);
		}
	}
}
