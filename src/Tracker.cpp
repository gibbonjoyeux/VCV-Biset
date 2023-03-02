
#include "plugin.hpp"

char	table_pitch[12][3] = {
	"C.", "C#", "D.", "D#", "E.", "F.", "F#", "G.", "G#", "A.", "A#", "B."
};
char	table_effect[] = "VTFfR";
char	table_hex[] = "0123456789ABCDEF";

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
	Timeline			timeline;

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

		clock_timer.reset();

		/// INIT PATTERN SOURCE
		PatternSource	*pattern;

		timeline.debug = 0;
		//timeline.beat_count = 4;
		//timeline.timeline.allocate(32, timeline.beat_count);
		//for (i = 0; i < 32; ++i) {
		//	timeline.timeline[i][0].mode = 0;
		//}
		timeline.resize(4);
		timeline.timeline[0][0].mode = 1;
		timeline.timeline[0][0].pattern = 0;
		timeline.timeline[0][0].beat = 0;

		timeline.synths[0].init(0, 6);
		timeline.synths[1].init(1, 6);

		timeline.patterns.resize(1);
		pattern = &(timeline.patterns[0]);

		pattern->resize(3, 1, 4, 4);
		pattern->notes[0]->effect_count = 2;
		//pattern->line_count = 4;
		//pattern->row_count = 1;
		//pattern->lpb = 4;
		//pattern.lines.allocate(pattern.track_count, pattern.line_count);

		/// FILL PATTERN SOURCE NOTES
		pattern->notes[0]->lines[0].mode = PATTERN_NOTE_NEW;
		pattern->notes[0]->lines[0].synth = 0;
		pattern->notes[0]->lines[0].pitch = 64;
		pattern->notes[0]->lines[0].velocity = 255;
		pattern->notes[0]->lines[0].delay = 0;
		pattern->notes[0]->lines[0].chance = 255;
		for (i = 0; i < 8; ++i)
			pattern->notes[0]->lines[0].effects[i].type = PATTERN_EFFECT_NONE;
		pattern->notes[0]->lines[0].effects[0].type = PATTERN_EFFECT_TREMOLO;
		pattern->notes[0]->lines[0].effects[0].value = 0x49;
		pattern->notes[0]->lines[0].effects[1].type = PATTERN_EFFECT_VIBRATO;
		pattern->notes[0]->lines[0].effects[1].value = 0x42;

		pattern->notes[0]->lines[4].mode = PATTERN_NOTE_NEW;
		pattern->notes[0]->lines[4].glide = 100;
		pattern->notes[0]->lines[4].synth = 0;
		pattern->notes[0]->lines[4].pitch = 66;
		pattern->notes[0]->lines[4].velocity = 200;
		pattern->notes[0]->lines[4].delay = 0;
		pattern->notes[0]->lines[4].chance = 255;
		for (i = 0; i < 8; ++i)
			pattern->notes[0]->lines[4].effects[i].type = PATTERN_EFFECT_NONE;

		pattern->notes[0]->lines[8].mode = PATTERN_NOTE_NEW;
		pattern->notes[0]->lines[8].glide = 100;
		pattern->notes[0]->lines[8].synth = 0;
		pattern->notes[0]->lines[8].pitch = 68;
		pattern->notes[0]->lines[8].velocity = 128;
		pattern->notes[0]->lines[8].delay = 0;
		pattern->notes[0]->lines[8].chance = 255;
		for (i = 0; i < 8; ++i)
			pattern->notes[0]->lines[8].effects[i].type = PATTERN_EFFECT_NONE;

		//pattern->notes[0][10].mode = -1;
		//pattern->notes[0][10].delay = 0;
		//for (i = 0; i < 8; ++i)
		//	pattern->notes[0][10].effects[i].type = PATTERN_EFFECT_NONE;

		pattern->notes[0]->lines[12].mode = PATTERN_NOTE_NEW;
		pattern->notes[0]->lines[12].glide = 100;
		pattern->notes[0]->lines[12].synth = 0;
		pattern->notes[0]->lines[12].pitch = 71;
		pattern->notes[0]->lines[12].velocity = 80;
		pattern->notes[0]->lines[12].delay = 0;
		pattern->notes[0]->lines[12].chance = 255;
		for (i = 0; i < 8; ++i)
			pattern->notes[0]->lines[12].effects[i].type = PATTERN_EFFECT_NONE;

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

	void	process(const ProcessArgs& args) override {
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

		timeline.process(this, dt_sec, dt_beat);


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

	void itohex(char *string, int number, int width) {
		int						length;
		int						i;

		if (number < 16)
			length = 1;
		else if (number < 256)
			length = 2;
		else
			length = 3;
		/// FILL NUMBER
		i = 0;
		while (number > 0) {
			string[length - i - 1] = table_hex[number % 16];
			number /= 16;
			i += 1;
		}
		/// FILL WIDTH
		while (i < width) {
			string[i] = '0';
			i += 1;
		}
		string[length] = 0;
	}

	void drawLayer(const DrawArgs& args, int layer) override {
		std::shared_ptr<Font>	font;
		Rect					rect;
		Vec						p;
		Vec						corner_bl, corner_tl;
		Vec						corner_br, corner_tr;
		float					char_width;
		int						i, j, k;
		float					x, y;
		float					x_row;

		if (layer == 1 && module) {
			font = APP->window->loadFont(font_path);

			/// GET CANVAS FORMAT
			rect = box.zeroPos();
			p = rect.getTopLeft();
			corner_bl = rect.getBottomLeft();
			corner_tl = rect.getTopLeft();
			corner_br = rect.getBottomRight();
			corner_tr = rect.getTopRight();

			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, module->colors[0]);
			nvgRect(args.vg, RECT_ARGS(rect));
			nvgFill(args.vg);

			// TMP DEBUG ! ! !
			nvgFillColor(args.vg, module->colors[3]);
			nvgText(args.vg, p.x + 100, p.y + 11.0, module->timeline.debug_str, NULL);


			//char text[1024];
			//int test = 0x49;
			//itoa(sizeof(Test), text, 10);
			////int a1 = (test << 4) >> 4;
			////int a2 = (test >> 4);
			//sprintf(text, "%d %d", test / 16, test % 16);
			//nvgText(args.vg, p.x + 100, p.y + 11.0, text, NULL);


			if (font) { 
				nvgFontSize(args.vg, 9);
				nvgFontFaceId(args.vg, font->handle);

				char_width = nvgTextBounds(args.vg, 0, 0, "X", NULL, NULL);

				// TMP TIME BEAT MARKER
				nvgBeginPath(args.vg);
				nvgFillColor(args.vg, module->colors[15]);
				nvgRect(args.vg,
				/**/ p.x, p.y + 3.5 + 8.5 * module->timeline.debug,
				/**/ rect.getWidth() + 0.5, 8.5);
				nvgFill(args.vg);

				if (module->timeline.patterns.size() > 0) {
					PatternSource	*pattern = &(module->timeline.patterns[0]);
					PatternNote		*note;
					PatternNoteRow	*note_row;
					PatternCV		*cv;
					PatternCVRow	*cv_row;
					PatternEffect	*effect;
					char			str[32];

					/// FOR EACH NOTE ROW	
					x_row = p.x + 2.0;
					for (i = 0; i < pattern->note_count; ++i) {
						note_row = pattern->notes[i];
						/// FOR EACH NOTE ROW LINE
						for (j = 0; j < pattern->line_count; ++j) {
							x = x_row;//p.x + 2.0;
							y = p.y + 11.0 + 8.5 * j;
							note = &(note_row->lines[j]);
							/// GLIDE
							nvgFillColor(args.vg, module->colors[9]);
							if (note->mode == PATTERN_NOTE_KEEP
							|| note->mode == PATTERN_NOTE_NEW) {
								str[0] = '.';
								str[1] = '.';
								str[2] = 0;
							} else {
								itoa(note->glide, str, 16);
							}
							nvgText(args.vg, x, y, str, NULL);
							x += char_width * 2.0;
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
								itoa(note->velocity, str, 16);
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
								itoa(note->delay, str, 16);
							}
							nvgText(args.vg, x, y, str, NULL);
							x += char_width * 2.0;
							/// CHANCE
							nvgFillColor(args.vg, module->colors[11]);
							if (note->mode == PATTERN_NOTE_KEEP) {
								str[0] = '.';
								str[1] = '.';
								str[2] = 0;
							} else {
								itoa(note->chance, str, 16);
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
								itoa(note->synth, str, 16);
							}
							nvgText(args.vg, x, y, str, NULL);
							x += char_width * 2.0;
							/// EFFECTS
							for (k = 0; k < note_row->effect_count; ++k) {
								if (k & 1)
									nvgFillColor(args.vg, module->colors[14]);
								else
									nvgFillColor(args.vg, module->colors[13]);
								effect = &(note->effects[k]);
								if (note->mode == PATTERN_NOTE_KEEP
								|| effect->type == PATTERN_EFFECT_NONE) {
									str[0] = '.';
									str[1] = '.';
									str[2] = '.';
									str[3] = 0;
								} else {
									str[0] = table_effect[effect->type - 1];
									itoa(note->effects[k].value, str + 1, 16);
								}
								nvgText(args.vg, x, y, str, NULL);
								x += char_width * 3.0;
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
							/// GLIDE
							nvgFillColor(args.vg, module->colors[9]);
							if (cv->mode == PATTERN_CV_KEEP) {
								str[0] = '.';
								str[1] = '.';
								str[2] = 0;
							} else {
								itoa(cv->glide, str, 16);
							}
							nvgText(args.vg, x, y, str, NULL);
							x += char_width * 2.0;
							/// VALUE
							nvgFillColor(args.vg, module->colors[3]);
							if (cv->mode == PATTERN_CV_KEEP) {
								str[0] = '.';
								str[1] = '.';
								str[2] = 0;
							} else {
								itoa(cv->value, str, 16);
							}
							nvgText(args.vg, x, y, str, NULL);
							x += char_width * 2.0;
							/// DELAY
							nvgFillColor(args.vg, module->colors[10]);
							if (cv->mode == PATTERN_NOTE_KEEP) {
								str[0] = '.';
								str[1] = '.';
								str[2] = 0;
							} else {
								itoa(cv->delay, str, 16);
							}
							nvgText(args.vg, x, y, str, NULL);
							x += char_width * 2.0;
						}
						if (pattern->line_count > 0)
							x_row = x + char_width;
					}
				}
			}
		}
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
