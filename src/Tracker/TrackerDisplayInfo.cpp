
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerDisplayInfo::TrackerDisplayInfo() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerDisplayInfo::draw(const DrawArgs &args) {
	Rect					rect;

	LedDisplay::draw(args);

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[0]);
	nvgRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0);
	nvgFill(args.vg);
}

void TrackerDisplayInfo::drawLayer(const DrawArgs &args, int layer) {
	char					buffer[5];
	PatternNote				*line_note;
	std::shared_ptr<Font>	font;
	Rect					rect;
	int						fx;

	if (g_module != this->module || this->module == NULL || layer != 1)
		return;

	/// GET FONT
	font = APP->window->loadFont(font_path);
	if (font == NULL)
		return;
	/// SET FONT
	nvgFontSize(args.vg, 9);
	nvgFontFaceId(args.vg, font->handle);
	nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	/// GET CANVAS FORMAT
	rect = box.zeroPos();

	nvgScissor(args.vg, RECT_ARGS(rect));
	/// DRAW
	if (g_editor->mode == EDITOR_MODE_PATTERN) {
		if (g_editor->pattern) {
			/// ON NOTE
			if (g_editor->pattern_col < g_editor->pattern->note_count) {
				line_note = &(g_editor->pattern->notes[g_editor->pattern_col]
				/**/ .lines[g_editor->pattern_line]);
				nvgFillColor(args.vg, colors[3]);
				switch (g_editor->pattern_cell) {
					/// PITCH
					case 0:
						nvgText(args.vg, 2, 2, "Pitch", NULL);
						break;
					/// OCTAVE
					case 1:
						nvgText(args.vg, 2, 2, "Octave", NULL);
						break;
					/// VELOCITY
					case 2:
						nvgText(args.vg, 2, 2, "Velocity", NULL);
						nvgFillColor(args.vg, colors[4]);
						nvgText(args.vg, 2, 2 + CHAR_H, "[0:99]", NULL);
						nvgFillColor(args.vg, colors[10]);
						nvgText(args.vg, 2, 2 + CHAR_H * 2, "00: 0v", NULL);
						nvgText(args.vg, 2, 2 + CHAR_H * 3, "99: 10v", NULL);
						break;
					/// PANNING
					case 3:
						nvgFillColor(args.vg, colors[3]);
						nvgText(args.vg, 2, 2, "Panning", NULL);
						nvgFillColor(args.vg, colors[4]);
						nvgText(args.vg, 2, 2 + CHAR_H, "[0:99]", NULL);
						nvgFillColor(args.vg, colors[10]);
						nvgText(args.vg, 2, 2 + CHAR_H * 2, "00: -5v", NULL);
						nvgText(args.vg, 2, 2 + CHAR_H * 3, "99: +5v", NULL);
						break;
					/// SYNTH
					case 4:
						nvgText(args.vg, 2, 2, "Synth", NULL);
						nvgFillColor(args.vg, colors[4]);
						nvgText(args.vg, 2, 2 + CHAR_H, "[0:99]", NULL);
						break;
					/// DELAY
					case 5:
						nvgText(args.vg, 2, 2, "Delay", NULL);
						nvgFillColor(args.vg, colors[4]);
						nvgText(args.vg, 2, 2 + CHAR_H, "[0:99]", NULL);
						break;
					/// GLIDE
					case 6:
						nvgText(args.vg, 2, 2, "Glide", NULL);
						nvgFillColor(args.vg, colors[4]);
						nvgText(args.vg, 2, 2 + CHAR_H, "[0:99]", NULL);
						nvgFillColor(args.vg, colors[13]);
						nvgText(args.vg, 2, 2 + CHAR_H * 2, "(Optional)", NULL);
						break;
					/// EFFECTS
					default:
						fx = line_note->effects
						/**/ [(g_editor->pattern_cell - 7) / 2].type;
						switch (fx) {
							case PATTERN_EFFECT_RAND_AMP:			//'A'	Axx
								nvgText(args.vg, 2, 2, "Rand Amp", NULL);
								nvgFillColor(args.vg, colors[4]);
								nvgText(args.vg, 2, 2 + CHAR_H, "Axx", NULL);
								nvgFillColor(args.vg, colors[10]);
								nvgText(args.vg, 2, 2 + CHAR_H * 2, "x: range", NULL);
								break;
							case PATTERN_EFFECT_RAND_PAN:			//'P'	Pxx
								nvgText(args.vg, 2, 2, "Rand Pan", NULL);
								nvgFillColor(args.vg, colors[4]);
								nvgText(args.vg, 2, 2 + CHAR_H, "Pxx", NULL);
								nvgFillColor(args.vg, colors[10]);
								nvgText(args.vg, 2, 2 + CHAR_H * 2, "x: range", NULL);
								break;
							case PATTERN_EFFECT_RAND_DELAY:			//'D'	Dxx
								nvgText(args.vg, 2, 2, "Rand Delay", NULL);
								nvgFillColor(args.vg, colors[4]);
								nvgText(args.vg, 2, 2 + CHAR_H, "Dxx", NULL);
								nvgFillColor(args.vg, colors[10]);
								nvgText(args.vg, 2, 2 + CHAR_H * 2, "x: length", NULL);
								break;
							case PATTERN_EFFECT_RAND_OCT:			//'O'	Oxy
								nvgText(args.vg, 2, 2, "Rand Oct", NULL);
								nvgFillColor(args.vg, colors[4]);
								nvgText(args.vg, 2, 2 + CHAR_H, "Oxy", NULL);
								nvgFillColor(args.vg, colors[10]);
								nvgText(args.vg, 2, 2 + CHAR_H * 2, "x: mode", NULL);
								nvgText(args.vg, 2, 2 + CHAR_H * 3, "y: range", NULL);
								break;
							case PATTERN_EFFECT_RAND_NOTE:			//'N'	Nxy
								nvgText(args.vg, 2, 2, "Rand Note", NULL);
								nvgFillColor(args.vg, colors[4]);
								nvgText(args.vg, 2, 2 + CHAR_H, "Nxy", NULL);
								nvgFillColor(args.vg, colors[10]);
								nvgText(args.vg, 2, 2 + CHAR_H * 2, "x: offset 1", NULL);
								nvgText(args.vg, 2, 2 + CHAR_H * 3, "y: offset 2", NULL);
								break;
							case PATTERN_EFFECT_VIBRATO:			//'V'	Vxy
								nvgText(args.vg, 2, 2, "Vibrato", NULL);
								nvgFillColor(args.vg, colors[4]);
								nvgText(args.vg, 2, 2 + CHAR_H, "Vxy", NULL);
								nvgFillColor(args.vg, colors[10]);
								nvgText(args.vg, 2, 2 + CHAR_H * 2, "x: speed", NULL);
								nvgText(args.vg, 2, 2 + CHAR_H * 3, "y: amp", NULL);
								break;
							case PATTERN_EFFECT_TREMOLO:			//'T'	Txy
								nvgText(args.vg, 2, 2, "Tremolo", NULL);
								nvgFillColor(args.vg, colors[4]);
								nvgText(args.vg, 2, 2 + CHAR_H, "Txy", NULL);
								nvgFillColor(args.vg, colors[10]);
								nvgText(args.vg, 2, 2 + CHAR_H * 2, "x: speed", NULL);
								nvgText(args.vg, 2, 2 + CHAR_H * 3, "y: amp", NULL);
								break;
							case PATTERN_EFFECT_VIBRATO_RAND:		//'v'	vxy
								nvgText(args.vg, 2, 2, "Vibrato rand", NULL);
								nvgFillColor(args.vg, colors[4]);
								nvgText(args.vg, 2, 2 + CHAR_H, "vxy", NULL);
								nvgFillColor(args.vg, colors[10]);
								nvgText(args.vg, 2, 2 + CHAR_H * 2, "x: max speed", NULL);
								nvgText(args.vg, 2, 2 + CHAR_H * 3, "y: max amp", NULL);
								break;
							case PATTERN_EFFECT_TREMOLO_RAND:		//'t'	txy
								nvgText(args.vg, 2, 2, "Tremolo rand", NULL);
								nvgFillColor(args.vg, colors[4]);
								nvgText(args.vg, 2, 2 + CHAR_H, "txy", NULL);
								nvgFillColor(args.vg, colors[10]);
								nvgText(args.vg, 2, 2 + CHAR_H * 2, "x: max speed", NULL);
								nvgText(args.vg, 2, 2 + CHAR_H * 3, "y: max amp", NULL);
								break;
							case PATTERN_EFFECT_CHANCE:				//'C'	Cxx
								nvgText(args.vg, 2, 2, "Chance keep", NULL);
								nvgFillColor(args.vg, colors[4]);
								nvgText(args.vg, 2, 2 + CHAR_H, "Cxx", NULL);
								nvgFillColor(args.vg, colors[10]);
								nvgText(args.vg, 2, 2 + CHAR_H * 2, "x: chance", NULL);
								break;
							case PATTERN_EFFECT_CHANCE_STOP:		//'c'	cxx
								nvgText(args.vg, 2, 2, "Chance stop", NULL);
								nvgFillColor(args.vg, colors[4]);
								nvgText(args.vg, 2, 2 + CHAR_H, "cxx", NULL);
								nvgFillColor(args.vg, colors[10]);
								nvgText(args.vg, 2, 2 + CHAR_H * 2, "x: chance", NULL);
								break;
							default:
								nvgFillColor(args.vg, colors[13]);
								nvgText(args.vg, 2, 2, "Axx Pxx Dxx", NULL);
								nvgText(args.vg, 2, 2 + CHAR_H, "Oxy Nxy Vxy", NULL);
								nvgText(args.vg, 2, 2 + CHAR_H * 2, "Txy vxy txy", NULL);
								nvgText(args.vg, 2, 2 + CHAR_H * 3, "Cxx cxx", NULL);
								break;
						}
						break;
				}
			/// ON CV
			} else {
				nvgFillColor(args.vg, colors[3]);
				switch (g_editor->pattern_cell) {
					/// VALUE
					case 0:
						nvgText(args.vg, 2, 2, "Value", NULL);
						nvgFillColor(args.vg, colors[4]);
						nvgText(args.vg, 2, 2 + CHAR_H, "[0:999]", NULL);
						break;
					/// CURVE
					case 1:
						nvgText(args.vg, 2, 2, "Curve", NULL);
						nvgFillColor(args.vg, colors[4]);
						nvgText(args.vg, 2, 2 + CHAR_H, "[0:99]", NULL);
						break;
					/// DELAY
					case 2:
						nvgText(args.vg, 2, 2, "Delay", NULL);
						nvgFillColor(args.vg, colors[4]);
						nvgText(args.vg, 2, 2 + CHAR_H, "[0:99]", NULL);
						break;
				}
			}
		}
	} else if (g_editor->mode == EDITOR_MODE_TIMELINE) {
		buffer[4] = 0;

		/// BEAT CURRENT
		nvgFillColor(args.vg, colors[4]);
		itoaw(buffer, g_timeline->clock.beat + 1, 4);
		nvgText(args.vg, 2, 2, buffer, NULL);

		/// DIVIDER
		nvgFillColor(args.vg, colors[3]);
		nvgText(args.vg, 2 + CHAR_W * 5.25, 2, "/", NULL);

		/// BEAT LENGTH
		nvgFillColor(args.vg, colors[4]);
		itoaw(buffer, g_timeline->timeline_length, 4);
		nvgText(args.vg, CHAR_W * 8 - 2, 2, buffer, NULL);
	}
	nvgResetScissor(args.vg);
	LedDisplay::drawLayer(args, layer);
}

