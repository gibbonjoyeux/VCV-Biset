
#include "plugin.hpp"

char	table_pitch[12][3] = {
	"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"
};
char	table_effect[] = "APOMSVTFfCcR";
char	table_hex[] = "0123456789ABCDEF";
int		table_row_note_width[] = {
	2,		// Pitch	(2)
	1,		// Octave	(1)
	2,		// Velocity	(2)
	2,		// Panning	(2)
	2,		// Synth	(2)
	2,		// Delay	(2)
	2,		// Glide	(2)
	1, 2,	// Effect 1	(3)
	1, 2,	// Effect 2	(3)
	1, 2,	// Effect 3	(3)
	1, 2,	// Effect 4	(3)
	1, 2,	// Effect 5	(3)
	1, 2,	// Effect 6	(3)
	1, 2,	// Effect 7	(3)
	1, 2	// Effect 8	(3)
};
int		table_row_note_pos[] = {
	0,		// Pitch	(2)
	2,		// Octave	(1)
	3,		// Velocity	(2)
	5,		// Panning	(2)
	7,		// Synth	(2)
	9,		// Delay	(2)
	11,		// Glide	(2)
	13, 14,	// Effect 1	(3)
	16,	17,	// Effect 2	(3)
	19,	20,	// Effect 3	(3)
	22,	23,	// Effect 4	(3)
	25,	26,	// Effect 5	(3)
	28,	29,	// Effect 6	(3)
	30,	31,	// Effect 7	(3)
	33,	34	// Effect 8	(3)
};
int		table_row_cv_width[] = {
	2,	// Value	(2)
	2,	// Curve	(2)
	2	// Delay	(2)
};
int		table_row_cv_pos[] = {
	0,	// Value	(2)
	2,	// Curve	(2)
	4	// Delay	(2)
};
int		table_keyboard[128];


Timeline		g_timeline;
Editor			g_editor;

void int_to_hex(char *str, int number, int width) {
	char		digit;
	int			i;

	str[width] = 0;
	for (i = width - 1; i >= 0; --i) {
		digit = number % 16;
		if (digit < 10)
			str[i] = '0' + digit;
		else
			str[i] = 'A' + (digit - 10);
		number /= 16;
	}
}



struct Tracker : Module {
	enum	ParamIds {
		PARAM_BPM,
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_CLOCK,
		ENUMS(OUTPUT_CV, 8),
		ENUMS(OUTPUT_GATE, 8),
		ENUMS(OUTPUT_VELO, 8),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	NVGcolor			colors[16];
	//Timeline			timeline;

	dsp::TTimer<float>	clock_timer;
	float				clock_time;
	float				clock_time_p;

	Tracker() {
		int	i;

		config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);
		configParam(PARAM_BPM, 30.0f, 300.0f, 120.f, "BPM");
		//configInput(INPUT_CLOCK, "Clock");
		configOutput(OUTPUT_CLOCK, "Clock");
		for (i = 0; i < 8; ++i) {
			configOutput(OUTPUT_CV + i, string::f("CV/PITCH %d", i + 1));
			configOutput(OUTPUT_GATE + i, string::f("GATE %d", i + 1));
			configOutput(OUTPUT_VELO + i, string::f("VELOCITY %d", i + 1));
		}

		/// DEFINE GLOBAL COLOR
		colors[0] = nvgRGBA(0x1A, 0x1C, 0x2C, 0xFF);
		colors[1] = nvgRGBA(0x5D, 0x27, 0x5D, 0xFF);
		colors[2] = nvgRGBA(0xB1, 0x3E, 0x53, 0xFF);
		colors[3] = nvgRGBA(0xEF, 0x7D, 0x57, 0xFF);
		colors[4] = nvgRGBA(0xFF, 0xCD, 0x75, 0xFF);
		colors[5] = nvgRGBA(0xAF, 0xF0, 0x70, 0xFF);
		colors[6] = nvgRGBA(0x38, 0xB7, 0x64, 0xFF);
		colors[7] = nvgRGBA(0x25, 0x71, 0x79, 0xFF);
		colors[8] = nvgRGBA(0x29, 0x36, 0x6F, 0xFF);
		colors[9] = nvgRGBA(0x3B, 0x5D, 0xC9, 0xFF);
		colors[10] = nvgRGBA(0x41, 0xA6, 0xF6, 0xFF);
		colors[11] = nvgRGBA(0x73, 0xEF, 0xF7, 0xFF);
		colors[12] = nvgRGBA(0xF4, 0xF4, 0xF4, 0xFF);
		colors[13] = nvgRGBA(0x94, 0xB0, 0xC2, 0xFF);
		colors[14] = nvgRGBA(0x56, 0x6C, 0x86, 0xFF);
		colors[15] = nvgRGBA(0x33, 0x3C, 0x57, 0xFF);

		/// DEFINE GLOBAL KEYBOARD
		for (i = 0; i < 128; ++i)
			table_keyboard[i] = -1;
		//// OCTAVE LOW 1
		table_keyboard['Z'] = 0;		// C
			table_keyboard['S'] = 1;	// C#
		table_keyboard['X'] = 2;		// D
			table_keyboard['D'] = 3;	// D#
		table_keyboard['C'] = 4;		// E
		table_keyboard['V'] = 5;		// F
			table_keyboard['G'] = 6;	// F#
		table_keyboard['B'] = 7;		// G
			table_keyboard['H'] = 8;	// G#
		table_keyboard['N'] = 9;		// A
			table_keyboard['J'] = 10;	// A#
		table_keyboard['M'] = 11;		// B
		//// OCTAVE LOW 2
		table_keyboard[','] = 12;		// C
			table_keyboard['L'] = 13;	// C#
		table_keyboard['.'] = 14;		// D
			table_keyboard[';'] = 15;	// D#
		table_keyboard['/'] = 16;		// E
		//// OCTAVE HIGH 2
		table_keyboard['Q'] = 12;		// C
			table_keyboard['2'] = 13;	// C#
		table_keyboard['W'] = 14;		// D
			table_keyboard['3'] = 15;	// D#
		table_keyboard['E'] = 16;		// E
		table_keyboard['R'] = 17;		// F
			table_keyboard['5'] = 18;	// F#
		table_keyboard['T'] = 19;		// G
			table_keyboard['6'] = 20;	// G#
		table_keyboard['Y'] = 21;		// A
			table_keyboard['7'] = 22;	// A#
		table_keyboard['U'] = 23;		// B
		//// OCTAVE HIGH 3
		table_keyboard['I'] = 24;		// C
			table_keyboard['9'] = 25;	// C#
		table_keyboard['O'] = 26;		// D
			table_keyboard['0'] = 27;	// D#
		table_keyboard['P'] = 28;		// E
		table_keyboard['['] = 29;		// F
			table_keyboard['='] = 30;	// F#
		table_keyboard[']'] = 31;		// G


		clock_timer.reset();

		/// INIT PATTERN SOURCE
		PatternSource	*pattern;

		g_timeline.debug = 0;
		g_timeline.resize(16);
		g_timeline.timeline[0][0].mode = 1;
		g_timeline.timeline[0][0].pattern = 0;
		g_timeline.timeline[0][0].beat = 0;

		g_timeline.synths[0].init(0, 6);
		g_timeline.synths[1].init(1, 6);

		g_timeline.patterns.resize(1);
		pattern = &(g_timeline.patterns[0]);
		//this->editor_pattern = pattern;
		g_editor.pattern = pattern;

		pattern->resize(3, 1, 16, 4);
		pattern->notes[0]->effect_count = 2;

		/// FILL PATTERN SOURCE NOTES
		pattern->notes[0]->lines[0].mode = PATTERN_NOTE_NEW;
		pattern->notes[0]->lines[0].synth = 0;
		pattern->notes[0]->lines[0].pitch = 63;
		pattern->notes[0]->lines[0].velocity = 255;
		pattern->notes[0]->lines[8].mode = PATTERN_NOTE_NEW;
		pattern->notes[0]->lines[8].synth = 0;
		pattern->notes[0]->lines[8].pitch = 61;
		pattern->notes[0]->lines[8].velocity = 255;
		pattern->notes[0]->lines[16].mode = PATTERN_NOTE_NEW;
		pattern->notes[0]->lines[16].synth = 0;
		pattern->notes[0]->lines[16].pitch = 66;
		pattern->notes[0]->lines[16].velocity = 255;
		pattern->notes[0]->lines[24].mode = PATTERN_NOTE_NEW;
		pattern->notes[0]->lines[24].synth = 0;
		pattern->notes[0]->lines[24].pitch = 70;
		pattern->notes[0]->lines[24].velocity = 255;

		pattern->notes[0]->lines[32].mode = PATTERN_NOTE_NEW;
		pattern->notes[0]->lines[32].synth = 0;
		pattern->notes[0]->lines[32].pitch = 68;
		pattern->notes[0]->lines[32].velocity = 255;
		pattern->notes[0]->lines[40].mode = PATTERN_NOTE_NEW;
		pattern->notes[0]->lines[40].synth = 0;
		pattern->notes[0]->lines[40].pitch = 63;
		pattern->notes[0]->lines[40].velocity = 255;
		pattern->notes[0]->lines[48].mode = PATTERN_NOTE_NEW;
		pattern->notes[0]->lines[48].synth = 0;
		pattern->notes[0]->lines[48].pitch = 70;
		pattern->notes[0]->lines[48].velocity = 255;

		/// FILL PATTERN SOURCE CVS
		// TODO: use ArrayExt and set SYNTH & CHANNEL on row
		pattern->cvs[0]->synth = 1;
		pattern->cvs[0]->channel = 0;
		pattern->cvs[0]->lines[0].mode = PATTERN_CV_SET;
		pattern->cvs[0]->lines[0].value = 0;
		pattern->cvs[0]->lines[8].mode = PATTERN_CV_SET;
		pattern->cvs[0]->lines[8].value = 255;
		pattern->cvs[0]->lines[15].mode = PATTERN_CV_SET;
		pattern->cvs[0]->lines[15].delay = 255;
		pattern->cvs[0]->lines[15].value = 0;

		//pattern->notes[1][6].mode = -1;
		//pattern->notes[1][6].delay = 0;
		//for (i = 0; i < 8; ++i)
		//	pattern->notes[1][6].effects[i].type = PATTERN_EFFECT_NONE;
	}

	void process(const ProcessArgs& args) override {
		float	dt_sec, dt_beat;
		float	bpm;

		/// COMPUTE CLOCK
		bpm = params[PARAM_BPM].getValue();
		dt_sec = args.sampleTime;
		dt_beat = (bpm * dt_sec) / 60.0f;
		clock_time_p = clock_timer.time;
		clock_timer.process(dt_beat);
		if (clock_timer.time >= 64.0f)
			clock_timer.time -= 64.0f;
		clock_time = clock_timer.time;

		if (clock_time_p - (int)clock_time_p > clock_time - (int)clock_time)
			outputs[OUTPUT_CLOCK].setVoltage(10.0f);
		else
			outputs[OUTPUT_CLOCK].setVoltage(0.0f);

		g_timeline.process(dt_sec, dt_beat);


		/// USE / MODIFY EXPANDERS
		//if (rightExpander.module) {
		//	rightExpander.module->params[0].setValue(0);
		//}
	}
};

struct TrackerDisplay : LedDisplay {
	Tracker*				module;
	ModuleWidget*			moduleWidget;
	std::string				font_path;

	TrackerDisplay() {
		font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
	}

	void drawLayer(const DrawArgs& args, int layer) override {
		std::shared_ptr<Font>	font;
		Rect					rect;
		Vec						p;
		Vec						corner_bl, corner_tl;
		Vec						corner_br, corner_tr;
		float					char_width;
		int						i, j, k;
		float					x, y, w;
		float					x_row;
		PatternSource			*pattern;
		PatternNote				*note;
		PatternNoteRow			*note_row;
		PatternCV				*cv;
		PatternCVRow			*cv_row;
		PatternEffect			*effect;
		char					str[32];

		if (module == NULL || layer != 1)
			return;
		font = APP->window->loadFont(font_path);
		if (font == NULL)
			return;

		/// SET FONT
		nvgFontSize(args.vg, 9);
		nvgFontFaceId(args.vg, font->handle);

		/// GET CANVAS FORMAT
		rect = box.zeroPos();
		p = rect.getTopLeft();
		corner_bl = rect.getBottomLeft();
		corner_tl = rect.getTopLeft();
		corner_br = rect.getBottomRight();
		corner_tr = rect.getTopRight();

		/// BACKGROUND
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, module->colors[0]);
		nvgRect(args.vg, RECT_ARGS(rect));
		nvgFill(args.vg);
		/// EDITOR SELECTED BACKGROUND
		if (g_editor.selected) {
			nvgBeginPath(args.vg);
			nvgStrokeColor(args.vg, module->colors[8]);
			nvgStrokeWidth(args.vg, 3);
			nvgRect(args.vg,
			/**/ rect.pos.x + 1.5, rect.pos.y + 1.5,
			/**/ rect.size.x - 3, rect.size.y - 3);
			nvgStroke(args.vg);
		}

		//// TMP DEBUG ! ! !
		nvgFillColor(args.vg, module->colors[3]);
		nvgText(args.vg, p.x + 100, p.y + 11.0, g_timeline.debug_str, NULL);
		// TMP DEBUG ! ! !
		//char text[1024];
		//int test = 0x49;
		//itoa(sizeof(Test), text, 10);
		////int a1 = (test << 4) >> 4;
		////int a2 = (test >> 4);
		//sprintf(text, "%d %dx%d", g_editor.pattern_row,
		///**/ g_editor.pattern_line, g_editor.pattern_cell);
		nvgText(args.vg, p.x + 400, p.y + 11.0, g_editor.pattern_debug, NULL);
		// TMP DEBUG ! ! !

		nvgScissor(args.vg, RECT_ARGS(rect));

		char_width = nvgTextBounds(args.vg, 0, 0, "X", NULL, NULL);

		/// DRAW PATTERN ROWS
		if (g_editor.pattern) {
			pattern = g_editor.pattern;

			/// [1] LAYER 1 (MARKERS + NOTES + CURVES)

			/// DRAW BEAT CURSOR
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, module->colors[15]);
			nvgRect(args.vg,
			/**/ p.x, p.y + 3.5 + 8.5 * g_timeline.debug,
			/**/ rect.getWidth() + 0.5, 8.5);
			nvgFill(args.vg);

			/// DRAW PATTERN CURSOR
			x = 0;
			y = g_editor.pattern_line;
			w = 1;
			i = 0;
			while (i < pattern->note_count + pattern->cv_count) {
				/// ON NOTE
				if (i < pattern->note_count) {
					note_row = pattern->notes[i];
					if (i == g_editor.pattern_row) {
						x += table_row_note_pos[g_editor.pattern_cell];
						w = table_row_note_width[g_editor.pattern_cell];
						break;
					}
					x += (2 + 1 + 2 + 2 + 2 + 2 + 2
					/**/ + 3 * note_row->effect_count + 1);
				/// ON CV
				} else {
					cv_row = pattern->cvs[i - pattern->note_count];
					if (i == g_editor.pattern_row) {
						x += table_row_cv_pos[g_editor.pattern_cell];
						w = table_row_cv_width[g_editor.pattern_cell];
						break;
					}
					x += (2 + 2 + 2 + 1);
				}
				i += 1;
			}
			x = 2.0 * char_width + x * char_width + 2.0;
			y = y * 8.5 + 3.5;
			w = w * char_width;
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, module->colors[12]);
			nvgRect(args.vg, x, y, w, 8.5);
			nvgFill(args.vg);

			/// DRAW LINE / BEAT COUNT
			x = p.x + 2;
			for (i = 0; i < pattern->line_count; ++i) {
				y = p.y + 11.0 + i * 8.5;
				/// BEAT COUNT
				if (i % pattern->lpb == 0) {
					int_to_hex(str, i / pattern->lpb, 2);
					nvgFillColor(args.vg, module->colors[13]);
				/// LINE COUNT
				} else {
					int_to_hex(str, i % pattern->lpb, 2);
					nvgFillColor(args.vg, module->colors[15]);
				}
				nvgText(args.vg, x, y, str, NULL);
			}

			/// [2] LAYER 2 (TRACKER)

			/// FOR EACH NOTE ROW	
			x_row = p.x + 2.0 + 2.0 * char_width;
			for (i = 0; i < pattern->note_count; ++i) {
				note_row = pattern->notes[i];
				/// FOR EACH NOTE ROW LINE
				for (j = 0; j < pattern->line_count; ++j) {
					x = x_row;//p.x + 2.0;
					y = p.y + 11.0 + 8.5 * j;
					note = &(note_row->lines[j]);
					/// PITCH
					nvgFillColor(args.vg, module->colors[3]);
					if (note->mode == PATTERN_NOTE_KEEP) {
						nvgText(args.vg, x, y, "..", NULL);
					} else {
						nvgText(args.vg, x, y,
						/**/ table_pitch[note->pitch % 12], NULL);
					}
					x += char_width * 2.0;
					/// OCTAVE
					nvgFillColor(args.vg, module->colors[2]);
					if (note->mode == PATTERN_NOTE_KEEP) {
						str[0] = '.';
						str[1] = 0;
					} else {
						itoa(note->pitch / 12, str, 10);
					}
					nvgText(args.vg, x, y, str, NULL);
					x += char_width * 1.0;
					/// VELOCITY
					nvgFillColor(args.vg, module->colors[5]);
					if (note->mode == PATTERN_NOTE_KEEP) {
						str[0] = '.';
						str[1] = '.';
						str[2] = 0;
					} else {
						int_to_hex(str, note->velocity, 2);
					}
					nvgText(args.vg, x, y, str, NULL);
					x += char_width * 2.0;
					/// PANNING
					nvgFillColor(args.vg, module->colors[6]);
					if (note->mode == PATTERN_NOTE_KEEP) {
						str[0] = '.';
						str[1] = '.';
						str[2] = 0;
					} else {
						int_to_hex(str, note->panning, 2);
					}
					nvgText(args.vg, x, y, str, NULL);
					x += char_width * 2.0;
					/// SYNTH
					nvgFillColor(args.vg, module->colors[4]);
					if (note->mode == PATTERN_NOTE_KEEP) {
						str[0] = '.';
						str[1] = '.';
						str[2] = 0;
					} else {
						int_to_hex(str, note->synth, 2);
					}
					nvgText(args.vg, x, y, str, NULL);
					x += char_width * 2.0;
					/// DELAY
					nvgFillColor(args.vg, module->colors[10]);
					if (note->mode == PATTERN_NOTE_KEEP) {
						str[0] = '.';
						str[1] = '.';
						str[2] = 0;
					} else {
						int_to_hex(str, note->delay, 2);
					}
					nvgText(args.vg, x, y, str, NULL);
					x += char_width * 2.0;
					/// GLIDE
					nvgFillColor(args.vg, module->colors[11]);
					if (note->mode == PATTERN_NOTE_KEEP
					|| note->mode == PATTERN_NOTE_NEW) {
						str[0] = '.';
						str[1] = '.';
						str[2] = 0;
					} else {
						int_to_hex(str, note->glide, 2);
					}
					nvgText(args.vg, x, y, str, NULL);
					x += char_width * 2.0;
					/// EFFECTS
					for (k = 0; k < note_row->effect_count; ++k) {
						effect = &(note->effects[k]);
						/// COMPUTE STRINGS
						if (note->mode == PATTERN_NOTE_KEEP
						|| effect->type == PATTERN_EFFECT_NONE) {
							str[0] = '.';
							str[1] = 0;
							str[2] = '.';
							str[3] = '.';
							str[4] = 0;
						} else {
							str[0] = table_effect[effect->type - 1];
							str[1] = 0;
							int_to_hex(str + 2, note->effects[k].value, 2);
						}
						/// EFFECT TYPE
						nvgFillColor(args.vg, module->colors[13]);
						nvgText(args.vg, x, y, str, NULL);
						x += char_width * 1.0;
						/// EFFECT VALUE
						nvgFillColor(args.vg, module->colors[14]);
						nvgText(args.vg, x, y, str + 2, NULL);
						x += char_width * 2.0;
					}
				}
				if (pattern->line_count > 0)
					x_row = x + char_width;
			}
			/// FOR EACH CV ROW
			for (i = 0; i < pattern->cv_count; ++i) {
				cv_row = pattern->cvs[i];
				/// FOR EACH CV ROW LINE
				for (j = 0; j < pattern->line_count; ++j) {
					x = x_row;
					y = p.y + 11.0 + 8.5 * j;
					cv = &(cv_row->lines[j]);
					/// VALUE
					nvgFillColor(args.vg, module->colors[3]);
					if (cv->mode == PATTERN_CV_KEEP) {
						str[0] = '.';
						str[1] = '.';
						str[2] = 0;
					} else {
						int_to_hex(str, cv->value, 2);
					}
					nvgText(args.vg, x, y, str, NULL);
					x += char_width * 2.0;
					/// GLIDE
					nvgFillColor(args.vg, module->colors[5]);
					if (cv->mode == PATTERN_CV_KEEP) {
						str[0] = '.';
						str[1] = '.';
						str[2] = 0;
					} else {
						int_to_hex(str, cv->glide, 2);
					}
					nvgText(args.vg, x, y, str, NULL);
					x += char_width * 2.0;
					/// DELAY
					nvgFillColor(args.vg, module->colors[10]);
					if (cv->mode == PATTERN_CV_KEEP) {
						str[0] = '.';
						str[1] = '.';
						str[2] = 0;
					} else {
						int_to_hex(str, cv->delay, 2);
					}
					nvgText(args.vg, x, y, str, NULL);
					x += char_width * 2.0;
				}
				if (pattern->line_count > 0)
					x_row = x + char_width;
			}
		}

		nvgResetScissor(args.vg);

		//if (g_editor.selected == false) {
		//	nvgBeginPath(args.vg);
		//	nvgFillColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0x40));
		//	nvgRect(args.vg, RECT_ARGS(rect));
		//	nvgFill(args.vg);
		//}

		LedDisplay::drawLayer(args, layer);
	}

	//void onDragHover(const DragHoverEvent& e) override {
	//	module->params[0].setValue(0);
	//	module->params[1].setValue(0);
	//	module->params[2].setValue(0);
	//	module->params[3].setValue(0);
	//}
};

struct TrackerBPMDisplay : LedDisplay {
	Tracker*		module;
	ModuleWidget*		moduleWidget;
	std::string		font_path;
	char			str_bpm[4];

	TrackerBPMDisplay() {
		font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
	}

	void drawLayer(const DrawArgs& args, int layer) override {
		std::shared_ptr<Font>	font;
		Rect			rect;
		Vec			p;
		int			bpm;

		font = APP->window->loadFont(font_path);
		if (layer == 1 && module) {
			/// GET CANVAS FORMAT
			rect = box.zeroPos();
			p = rect.getTopLeft();

			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, module->colors[15]);
			nvgRect(args.vg, RECT_ARGS(rect));
			nvgFill(args.vg);

			if (font) { 
				bpm = module->params[Tracker::PARAM_BPM].getValue();
				if (bpm < 100) {
					itoa(bpm, str_bpm + 1, 10);
					str_bpm[0] = ' ';
				} else {
					itoa(bpm, str_bpm, 10);
				}
				nvgFontSize(args.vg, 22);
				nvgFontFaceId(args.vg, font->handle);
				nvgFillColor(args.vg, module->colors[12]);
				nvgText(args.vg, p.x + 1.25, p.y + 22.5, str_bpm, NULL);
			} else {
				nvgStrokeColor(args.vg, nvgRGBA(0xff, 0xff, 0x00, 0xff));
				nvgStrokeWidth(args.vg, 1.5f);
				{
					nvgLineCap(args.vg, NVG_ROUND);
					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, p.x, p.y);
					nvgLineTo(args.vg, p.x + 10.0, p.y + 10.0);
					nvgStroke(args.vg);
					nvgClosePath(args.vg);
					nvgStroke(args.vg);
				}
			}
		}
		LedDisplay::drawLayer(args, layer);
	}
};

struct TrackerWidget : ModuleWidget {
	Tracker*	module;

	TrackerWidget(Tracker* _module) {
		TrackerDisplay*		display;
		TrackerBPMDisplay*	display_bpm;
		int			i;

		//
		// BUTTONS:
		// - TL1105
		//
		// KNOBS:
		// - RoundBlackKnob
		// - RoundKnob
		// - Round(Big / Huge / Large / Small)BlackKnob
		// - Trimpot (very small)
		// - Rogan(1 / 2 / 3 / 5 / 6)(P / PS)(Blue / Green / Red / White / Gray)
		// - Befaco(Big / Tiny)Knob
		// - Davies1900h(Large)(Black / Red / White)Knob
		//
		// INPUTS:
		// - PJ301MPort
		//
		// OUTPUTS:
		// - PJ301MPort
		//

		module = _module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker.svg")));

		addParam(
		/**/ createParamCentered<Rogan2PSWhite>(mm2px(Vec(10.125, 14.0)),
		/**/ module,
		/**/ Tracker::PARAM_BPM));

		addOutput(
		/**/ createOutputCentered<PJ301MPort>(mm2px(Vec(9.1, 119.35)), //135.05
		/**/ module,
		/**/ Tracker::OUTPUT_CLOCK));

		for (i = 0; i < 8; ++i) {
			addOutput(
			/**/ createOutputCentered<PJ301MPort>(mm2px(Vec(213.25, 9.45 + 15.7 * i)),
			/**/ module,
			/**/ Tracker::OUTPUT_CV + i));
			addOutput(
			/**/ createOutputCentered<PJ301MPort>(mm2px(Vec(213.25 + 10.7, 9.45 + 15.7 * i)),
			/**/ module,
			/**/ Tracker::OUTPUT_GATE + i));
			addOutput(
			/**/ createOutputCentered<PJ301MPort>(mm2px(Vec(213.25 + 10.7 * 2.0, 9.45 + 15.7 * i)),
			/**/ module,
			/**/ Tracker::OUTPUT_VELO + i));
		}

		/// MAIN LED DISPLAY
		display = createWidget<TrackerDisplay>(mm2px(Vec(20.25 + 8.15, 7.15)));
		//display->box.size = mm2px(Vec(181.65, 94.5));
		display->box.size = mm2px(Vec(173.5, 94.5));
		display->module = module;
		display->moduleWidget = this;
		addChild(display);

		/// BPM LED DISPLAY
		display_bpm = createWidget<TrackerBPMDisplay>(mm2px(Vec(2.0, 23.0)));
		display_bpm->box.size = mm2px(Vec(16.5, 10.0));
		display_bpm->module = module;
		display_bpm->moduleWidget = this;
		addChild(display_bpm);
	}

	void onSelectKey(const SelectKeyEvent &e) override {
		PatternSource	*pattern;
		PatternNoteRow	*row_note;
		PatternNote		*line_note;
		PatternEffect	*effect;
		PatternCVRow	*row_cv;
		PatternCV		*line_cv;
		int				fx_1, fx_2;
		int				key;
		int				i;

		e.consume(this);
		if (e.action == GLFW_PRESS
		|| e.action == GLFW_REPEAT) {
			if (g_editor.pattern) {
				/// EVENT CURSOR MOVE
				if (e.key == GLFW_KEY_LEFT) {
					g_editor.pattern_cell -= 1;
					g_editor.pattern_char = 0;
				} else if (e.key == GLFW_KEY_RIGHT) {
					g_editor.pattern_cell += 1;
					g_editor.pattern_char = 0;
				} else if (e.key == GLFW_KEY_UP) {
					g_editor.pattern_line -= 1;
					g_editor.pattern_char = 0;
				} else if (e.key == GLFW_KEY_DOWN) {
					g_editor.pattern_line += 1;
					g_editor.pattern_char = 0;
				/// EVENT KEYBOARD
				} else {
					pattern = g_editor.pattern;
					/// KEY ON NOTE
					if (g_editor.pattern_row < pattern->note_count) {
						row_note = pattern->notes[g_editor.pattern_row];
						line_note = &(row_note->lines[g_editor.pattern_line]);
						switch (g_editor.pattern_cell) {
							/// PITCH
							case 0:
								/// NOTE DELETE
								if (e.key == GLFW_KEY_DELETE
								|| e.key == GLFW_KEY_BACKSPACE) {
									line_note->mode = PATTERN_NOTE_KEEP;
								/// NOTE EDIT
								} else {
									key = this->key_midi(e);
									/// NOTE NEW
									if (key >= 0) {
										line_note->pitch = key;
										if (line_note->mode == PATTERN_NOTE_KEEP
										|| line_note->mode == PATTERN_NOTE_STOP) {
											line_note->mode = PATTERN_NOTE_NEW;
											line_note->velocity = 255;
											line_note->panning = 128;
										}
										strcpy(g_editor.pattern_debug,
										/**/ table_pitch[key % 12]);
									/// NOTE STOP
									} else if (key == -1) {
										line_note->mode = PATTERN_NOTE_STOP;
									}
								}
								break;
							/// OCTAVE
							case 1:
								key = this->key_hex(e);
								if (key >= 0 && key <= 9) {
									line_note->pitch =
									/**/ line_note->pitch % 12
									/**/ + key * 12;
								}
								break;
							/// VELOCITY
							case 2:
								key = this->key_hex(e);
								if (key >= 0) {
									if (g_editor.pattern_char == 0) {
										line_note->velocity =
										/**/ line_note->velocity % 16
										/**/ + key * 16;
										g_editor.pattern_char += 1;
									} else {
										line_note->velocity =
										/**/ (line_note->velocity / 16) * 16
										/**/ + key;
										g_editor.pattern_char = 0;
										g_editor.pattern_line += 1;
										g_editor.pattern_clamp_cursor();
									}
								}
								break;
							/// PANNING
							case 3:
								key = this->key_hex(e);
								if (key >= 0) {
									if (g_editor.pattern_char == 0) {
										line_note->panning =
										/**/ line_note->panning % 16
										/**/ + key * 16;
										g_editor.pattern_char += 1;
									} else {
										line_note->panning =
										/**/ (line_note->panning / 16) * 16
										/**/ + key;
										g_editor.pattern_char = 0;
										g_editor.pattern_line += 1;
										g_editor.pattern_clamp_cursor();
									}
								}
								break;
							/// SYNTH
							case 4:
								key = this->key_hex(e);
								if (key >= 0) {
									if (g_editor.pattern_char == 0) {
										line_note->synth =
										/**/ line_note->synth % 16
										/**/ + key * 16;
										g_editor.pattern_char += 1;
									} else {
										line_note->synth =
										/**/ (line_note->synth / 16) * 16
										/**/ + key;
										g_editor.pattern_char = 0;
										g_editor.pattern_line += 1;
										g_editor.pattern_clamp_cursor();
									}
								}
								break;
							/// DELAY
							case 5:
								key = this->key_hex(e);
								if (key >= 0) {
									if (g_editor.pattern_char == 0) {
										line_note->delay =
										/**/ line_note->delay % 16
										/**/ + key * 16;
										g_editor.pattern_char += 1;
									} else {
										line_note->delay =
										/**/ (line_note->delay / 16) * 16
										/**/ + key;
										g_editor.pattern_char = 0;
										g_editor.pattern_line += 1;
										g_editor.pattern_clamp_cursor();
									}
								}
								break;
							/// GLIDE
							case 6:
								/// GLIDE DELETE
								if (e.key == GLFW_KEY_DELETE
								|| e.key == GLFW_KEY_BACKSPACE) {
									if (line_note->mode == PATTERN_NOTE_GLIDE)
										line_note->mode = PATTERN_NOTE_NEW;
								/// GLIDE EDIT
								} else {
									key = this->key_hex(e);
									if (key >= 0) {
										if (line_note->mode == PATTERN_NOTE_NEW)
											line_note->mode = PATTERN_NOTE_GLIDE;
										if (g_editor.pattern_char == 0) {
											line_note->glide =
											/**/ line_note->glide % 16
											/**/ + key * 16;
											g_editor.pattern_char += 1;
										} else {
											line_note->glide =
											/**/ (line_note->glide / 16) * 16
											/**/ + key;
											g_editor.pattern_char = 0;
											g_editor.pattern_line += 1;
											g_editor.pattern_clamp_cursor();
										}
									}
								}
								break;
							/// EFFECT
							default:
								fx_1 = (g_editor.pattern_cell - 7) / 2;
								fx_2 = (g_editor.pattern_cell - 7) % 2;
								effect = &(line_note->effects[fx_1]);
								/// EFFECT DELETE
								if (e.key == GLFW_KEY_DELETE
								|| e.key == GLFW_KEY_BACKSPACE) {
									effect->type = PATTERN_EFFECT_NONE;
									effect->value = 0;
								/// EFFECT EDIT
								} else {
									/// EDIT EFFECT TYPE
									if (fx_2 == 0) {
										key = this->key_alpha(e);
										if (key > 0) {
											i = 0;
											/// FIND EFFECT TYPE
											while (i < (int)sizeof(table_effect)) {
												/// MATCH EFFECT TYPE
												if (key == table_effect[i]) {
													effect->type = i + 1;
													g_editor.pattern_line += 1;
													g_editor.pattern_clamp_cursor();
												}
												i += 1;
											}
										}
									/// EDIT EFFECT VALUE
									} else {
										key = this->key_hex(e);
										if (key >= 0) {
											if (g_editor.pattern_char == 0) {
												effect->value =
												/**/ effect->value % 16
												/**/ + key * 16;
												g_editor.pattern_char += 1;
											} else {
												effect->value =
												/**/ (effect->value / 16) * 16
												/**/ + key;
												g_editor.pattern_char = 0;
												g_editor.pattern_line += 1;
												g_editor.pattern_clamp_cursor();
											}
										}
									}
								}
								break;
						}
					/// KEY ON CV
					} else {
						row_cv = pattern->cvs[g_editor.pattern_row - pattern->note_count];
						line_cv = &(row_cv->lines[g_editor.pattern_line]);
						switch (g_editor.pattern_cell) {
							/// VALUE
							case 0:
								/// VALUE DELETE
								if (e.key == GLFW_KEY_DELETE
								|| e.key == GLFW_KEY_BACKSPACE) {
									line_cv->mode = PATTERN_CV_KEEP;
								/// VALUE EDIT
								} else {
									key = this->key_hex(e);
									if (key >= 0) {
										if (line_cv->mode == PATTERN_CV_KEEP)
											line_cv->mode = PATTERN_CV_SET;
										if (g_editor.pattern_char == 0) {
											line_cv->value =
											/**/ line_cv->value % 16
											/**/ + key * 16;
											g_editor.pattern_char += 1;
										} else {
											line_cv->value =
											/**/ (line_cv->value / 16) * 16
											/**/ + key;
											g_editor.pattern_char = 0;
										}
									}
								}
								break;
							/// GLIDE
							case 1:
								key = this->key_hex(e);
								if (key >= 0) {
									if (g_editor.pattern_char == 0) {
										line_cv->glide =
										/**/ line_cv->glide % 16
										/**/ + key * 16;
										g_editor.pattern_char += 1;
									} else {
										line_cv->glide =
										/**/ (line_cv->glide / 16) * 16
										/**/ + key;
										g_editor.pattern_char = 0;
									}
								}
								break;
							/// DELAY
							case 2:
								key = this->key_hex(e);
								if (key >= 0) {
									if (g_editor.pattern_char == 0) {
										line_cv->delay =
										/**/ line_cv->delay % 16
										/**/ + key * 16;
										g_editor.pattern_char += 1;
									} else {
										line_cv->delay =
										/**/ (line_cv->delay / 16) * 16
										/**/ + key;
										g_editor.pattern_char = 0;
									}
								}
								break;
						}
					}
				}
				/// CLAMP CURSOR
				g_editor.pattern_clamp_cursor();
			}
		}
	}

	int key_midi(const SelectKeyEvent &e) {
		int			midi;

		if (e.key < 0 || e.key >= 128)
			return -2;
		if (e.keyName[0] == 'o')
			return -1;
		midi = table_keyboard[e.key];
		if (midi < 0)
			return -2;
		return midi + 60;
	}

	int key_hex(const SelectKeyEvent &e) {
		if (e.key >= GLFW_KEY_0 && e.key <= GLFW_KEY_9)
			return e.key - GLFW_KEY_0;
		else if (e.keyName[0] >= 'a' && e.keyName[0] <= 'f')
			return 10 + e.keyName[0] - 'a';
		return -1;
	}

	int key_alpha(const SelectKeyEvent &e) {
		if (e.keyName[0] >= 'a' && e.keyName[0] <= 'z') {
			if (e.mods & GLFW_MOD_SHIFT)
				return e.keyName[0] + ('A' - 'a');
			else
				return e.keyName[0];
		}
		return -1;
	}

	void onHoverScroll(const HoverScrollEvent &e) override {
		if (g_editor.selected == false)
			return;
		if (g_editor.pattern) {
			/// SCROLL X
			if (APP->window->getMods() & GLFW_MOD_SHIFT) {
				/// MOVE CURSOR
				if (e.scrollDelta.y > 0)
					g_editor.pattern_cell -= 1;
				else
					g_editor.pattern_cell += 1;
				g_editor.pattern_char = 0;
			/// SCROLL Y
			} else {
				/// MOVE CURSOR
				if (e.scrollDelta.y > 0)
					g_editor.pattern_line -= 1;
				else
					g_editor.pattern_line += 1;
				g_editor.pattern_char = 0;
			}
			/// CLAMP CURSOR
			g_editor.pattern_clamp_cursor();
		}
		e.consume(this);
	}

	void onSelect(const SelectEvent &e) override {
		g_editor.selected = true;
	}

	void onDeselect(const DeselectEvent &e) override {
		g_editor.selected = false;
	}

	//void onDragStart(const DragStartEvent& e) override {
	//	APP->window->cursorLock();
	//}
	//void onDragMove(const DragMoveEvent& e) override {
	//	/// NOT CALLED = NOT MOVED
	//	ModuleWidget::onDragMove(e);
	//}
	//void onDragEnd(const DragEndEvent& e) override {
	//	APP->window->cursorUnlock();
	//}
};

Model* modelTracker = createModel<Tracker, TrackerWidget>("Tracker");
