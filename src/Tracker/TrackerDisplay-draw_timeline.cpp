
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
	float							x, y, h, w;
	char							str[32];
	int								i, j;
	int								index;
	int								corner;
	int								beat;

	p = rect.getTopLeft();
	/// [1] DRAW PAGE
	/// DRAW ACTIVE BEAT MARKER
	if (g_timeline->play == TIMELINE_MODE_PLAY_SONG
	|| g_timeline->play == TIMELINE_MODE_PLAY_PATTERN) {
		x = p.x + 2.0 + CHAR_W * (g_timeline->clock.beat + 2
		/**/ - (int)g_editor->timeline_cam_x);
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, colors[15]);
		nvgRect(args.vg, x, rect.pos.y, CHAR_W, rect.size.y);
		nvgFill(args.vg);
	}
	/// DRAW BEAT / BAR COUNT
	y = p.y + 11.0;
	for (i = 0; i < 85; ++i) {
		index = i + (int)g_editor->timeline_cam_x;
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
	/// DRAW BAR MARKERS
	for (i = 4 - (int)g_editor->timeline_cam_x % 4; i < 85; i += 4) {
		x = p.x + 2.0 + CHAR_W * (i + 2);
		y = p.y + 13.0 + CHAR_H * 2;
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, colors[15]);
		nvgRect(args.vg, x, y, 1, CHAR_H * 12 * 3);
		nvgFill(args.vg);
	}

	/// [2] DRAW PATTERN INSTANCES
	//// FOR EACH ROW
	h = CHAR_H * 3 - 4;
	for (i = 0; i < 12; ++i) {
		index = i + g_editor->timeline_cam_y;
		y = p.y + 13.0 + CHAR_H * (i * 3 + 2);
		it = g_timeline->timeline[index].begin();
		it_end = g_timeline->timeline[index].end();
		while (it != it_end) {
			/// CHECK VISIBILITY
			//// PATTERN BEFORE
			if (it->beat < g_editor->timeline_cam_x) {
				if (it->beat + it->beat_length >= (int)g_editor->timeline_cam_x)
					visible = true;
				else
					visible = false;
			//// PATTERN INSIDE
			} else if (it->beat < (int)g_editor->timeline_cam_x + 85) {
				visible = true;
			//// PATTERN AFTER
			} else {
				break;
			}
			/// DRAW IF VISIBLE
			if (visible == true) {
				inst_x = it->beat - (int)g_editor->timeline_cam_x;
				inst_w = it->beat_length;
				x = p.x + 2.0 + CHAR_W * (inst_x + 2);
				w = CHAR_W * inst_w - 1;
				corner = (it->beat_length > 1) ? 5 : 3;
				/// FILL
				nvgBeginPath(args.vg);
				if (it->muted == true) {
					nvgFillColor(args.vg, nvgTransRGBAf(colors_user[it->source->color], 0.5));
				} else {
					nvgFillColor(args.vg, colors_user[it->source->color]);
				}
				nvgRoundedRect(args.vg, x + 1, y + 2, w, h, corner);
				nvgFill(args.vg);
				/// HANDLES
				if (it->beat_length > 2) {
					/// HANDLE RIGHT
					nvgBeginPath(args.vg);
					nvgStrokeColor(args.vg, colors[0]);
					nvgStrokeWidth(args.vg, 1);
					nvgRect(args.vg, x + 1 + w - CHAR_W, y + 2, 0, h);
					nvgStroke(args.vg);
					/// HANDLE LEFT
					nvgBeginPath(args.vg);
					nvgStrokeColor(args.vg, colors[0]);
					nvgStrokeWidth(args.vg, 1);
					nvgRect(args.vg, x + 1 + CHAR_W, y + 2, 0, h);
					nvgStroke(args.vg);
				}
				/// LOOP TIME MARKERS
				for (j = 0; j < it->beat_length; ++j) {
					beat = (it->beat_start + j) % it->source->beat_count;
					if (beat == 0 && j > 1 && j < it->beat_length - 1) {
						nvgStrokeColor(args.vg, colors[0]);
						nvgStrokeWidth(args.vg, 1);
						nvgBeginPath(args.vg);
						nvgMoveTo(args.vg, x + 1 + CHAR_W * j, y + 2);
						nvgLineTo(args.vg, x + 1 + CHAR_W * j, y + 2 + h / 5);
						nvgMoveTo(args.vg, x + 1 + CHAR_W * j, y + 2 + (h / 5) * 2);
						nvgLineTo(args.vg, x + 1 + CHAR_W * j, y + 2 + (h / 5) * 3);
						nvgMoveTo(args.vg, x + 1 + CHAR_W * j, y + 2 + (h / 5) * 4);
						nvgLineTo(args.vg, x + 1 + CHAR_W * j, y + 2 + (h / 5) * 5);
						nvgStroke(args.vg);
						nvgClosePath(args.vg);
					}
				}
				/// STROKE (ON SELECT)
				if (&(*it) == g_editor->instance) {
					nvgBeginPath(args.vg);
					nvgStrokeColor(args.vg, colors[12]);
					nvgStrokeWidth(args.vg, 1);
					nvgRoundedRect(args.vg, x + 1, y + 2, w, h, corner);
					nvgStroke(args.vg);
				}
				/// TEXT
				nvgFillColor(args.vg, colors[12]);
				j = 0;
				while (j < it->beat_length - 1 && it->source->name[j] != 0)
					++j;
				nvgText(args.vg, x + 3.0, y + CHAR_H * 2 - 2.0,
				/**/ it->source->name, it->source->name + j);
			}
			/// NEXT INSTANCE
			it = std::next(it);
		}
	}

	/// [3] DRAW PAGE TOP
	//// DRAW ROWS COUNT
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[0]);
	nvgRect(args.vg,
	/**/ p.x + 0.0, p.y + 6.0 + CHAR_H * 3.0,
	/**/ p.x + 2.0 + CHAR_W * (2.0), p.y + 6.0 + CHAR_H * (35.0));
	nvgFill(args.vg);
	for (i = 0; i < 12; ++i) {
		index = i + g_editor->timeline_cam_y;
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
}
