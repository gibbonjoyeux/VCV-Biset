
#include "plugin.hpp"

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
		timeline.beat_count = 4;
		timeline.timeline.allocate(32, timeline.beat_count);
		for (i = 0; i < 32; ++i) {
			timeline.timeline[i][0].mode = 0;
		}
		timeline.timeline[0][0].mode = 1;
		timeline.timeline[0][0].pattern = 0;
		timeline.timeline[0][0].beat = 0;

		timeline.synths.resize(2);
		timeline.synths[0].init(0, 6);
		timeline.synths[1].init(1, 6);

		timeline.patterns.resize(1);
		pattern = &(timeline.patterns[0]);

		pattern->init(2, 4, 4);
		//pattern->line_count = 4;
		//pattern->row_count = 1;
		//pattern->lpb = 4;
		//pattern.lines.allocate(pattern.track_count, pattern.line_count);

		/// FILL PATTERN SOURCE
		pattern->cells[0][0].mode = 1;
		pattern->cells[0][0].synth = 0;
		pattern->cells[0][0].pitch = 64;
		pattern->cells[0][0].velocity = 255;
		pattern->cells[0][0].delay = 0;
		pattern->cells[0][0].chance = 255;
		for (i = 0; i < 8; ++i)
			pattern->cells[0][0].effects[i].type = PatternEffect::NONE;
		pattern->cells[0][0].effects[0].type = PatternEffect::TREMOLO_FREQ;
		pattern->cells[0][0].effects[0].value = 4;
		pattern->cells[0][0].effects[1].type = PatternEffect::TREMOLO_AMP;
		pattern->cells[0][0].effects[1].value = 128;
		pattern->cells[0][0].effects[2].type = PatternEffect::VIBRATO_FREQ;
		pattern->cells[0][0].effects[2].value = 4;
		pattern->cells[0][0].effects[3].type = PatternEffect::VIBRATO_AMP;
		pattern->cells[0][0].effects[3].value = 32;

		pattern->cells[0][4].mode = 1;
		pattern->cells[0][4].synth = 0;
		pattern->cells[0][4].pitch = 66;
		pattern->cells[0][4].velocity = 200;
		pattern->cells[0][4].delay = 0;
		pattern->cells[0][4].chance = 255;
		for (i = 0; i < 8; ++i)
			pattern->cells[0][4].effects[i].type = PatternEffect::NONE;

		pattern->cells[0][8].mode = 1;
		pattern->cells[0][8].synth = 0;
		pattern->cells[0][8].pitch = 68;
		pattern->cells[0][8].velocity = 128;
		pattern->cells[0][8].delay = 0;
		pattern->cells[0][8].chance = 255;
		for (i = 0; i < 8; ++i)
			pattern->cells[0][8].effects[i].type = PatternEffect::NONE;

		//pattern->cells[0][10].mode = -1;
		//pattern->cells[0][10].delay = 0;
		//for (i = 0; i < 8; ++i)
		//	pattern->cells[0][10].effects[i].type = PatternEffect::NONE;

		pattern->cells[0][12].mode = 1;
		pattern->cells[0][12].synth = 0;
		pattern->cells[0][12].pitch = 71;
		pattern->cells[0][12].velocity = 80;
		pattern->cells[0][12].delay = 0;
		pattern->cells[0][12].chance = 255;
		for (i = 0; i < 8; ++i)
			pattern->cells[0][12].effects[i].type = PatternEffect::NONE;


		//pattern->cells[1][0].mode = 1;
		//pattern->cells[1][0].synth = 1;
		//pattern->cells[1][0].pitch = 66 + 24;
		//pattern->cells[1][0].velocity = 10;
		//pattern->cells[1][0].delay = 0;
		//pattern->cells[1][0].chance = 255;
		//for (i = 0; i < 8; ++i)
		//	pattern->cells[1][0].effects[i].type = PatternEffect::NONE;

		//pattern->cells[1][2].mode = 1;
		//pattern->cells[1][2].synth = 1;
		//pattern->cells[1][2].pitch = 64 + 24;
		//pattern->cells[1][2].velocity = 10;
		//pattern->cells[1][2].delay = 0;
		//pattern->cells[1][2].chance = 255;
		//for (i = 0; i < 8; ++i)
		//	pattern->cells[1][2].effects[i].type = PatternEffect::NONE;

		//pattern->cells[1][6].mode = -1;
		//pattern->cells[1][6].delay = 0;
		//for (i = 0; i < 8; ++i)
		//	pattern->cells[1][6].effects[i].type = PatternEffect::NONE;
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
	Tracker*		module;
	ModuleWidget*		moduleWidget;
	std::string		font_path;

	TrackerDisplay() {
		font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
	}

	void drawLayer(const DrawArgs& args, int layer) override {
		std::shared_ptr<Font>	font;
		Rect			rect;
		Vec			p;
		Vec			corner_bl, corner_tl;
		Vec			corner_br, corner_tr;
		float			char_width;
		int			i;

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
			nvgFillColor(args.vg, module->colors[15]);
			nvgRect(args.vg, RECT_ARGS(rect));
			nvgFill(args.vg);

			// TMP TIME BEAT MARKER
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, module->colors[4]);
			//nvgRect(args.vg,
			///**/ p.x + 1.0, p.y + 3.5 + 8.5 * module->timeline.clock.beat, 20, 8.5);
			nvgRect(args.vg,
			/**/ p.x + 1.0, p.y + 3.5 + 8.5 * module->timeline.debug, 20, 8.5);
			nvgFill(args.vg);
			// TMP DEBUG ! ! !
			nvgFillColor(args.vg, module->colors[3]);
			nvgText(args.vg, p.x + 100, p.y + 11.0, module->timeline.debug_str, NULL);


			if (font) { 
				nvgFontSize(args.vg, 9);
				nvgFontFaceId(args.vg, font->handle);

				char_width = nvgTextBounds(args.vg, 0, 0, "X", NULL, NULL);

				//nvgFillColor(args.vg, module->colors[0]);
				//nvgText(args.vg, p.x + 2, p.y + 11.0, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", NULL);

				/// PITCH
				nvgFillColor(args.vg, module->colors[3]);
				for (i = 0; i < 32; ++i)
					nvgText(args.vg, p.x + 2, p.y + 11.0 + 8.5 * i, "C#", NULL);
				/// OCTAVE
				nvgFillColor(args.vg, module->colors[2]);
				for (i = 0; i < 32; ++i)
					nvgText(args.vg, p.x + 2.0 + char_width * 2.0, p.y + 11.0 + 8.5 * i, "4", NULL);
				/// VELOCITY
				nvgFillColor(args.vg, module->colors[5]);
				for (i = 0; i < 32; ++i)
					nvgText(args.vg, p.x + 2.0 + char_width * 3.0, p.y + 11.0 + 8.5 * i, "FF", NULL);
				/// SYNTH
				nvgFillColor(args.vg, module->colors[4]);
				for (i = 0; i < 32; ++i)
					nvgText(args.vg, p.x + 2.0 + char_width * 5.0, p.y + 11.0 + 8.5 * i, "00", NULL);
				/// DELAY
				nvgFillColor(args.vg, module->colors[9]);
				for (i = 0; i < 32; ++i)
					nvgText(args.vg, p.x + 2.0 + char_width * 7.0, p.y + 11.0 + 8.5 * i, "00", NULL);
				/// EFFECTS
				nvgFillColor(args.vg, module->colors[12]);
				for (i = 0; i < 32; ++i)
					nvgText(args.vg, p.x + 2.0 + char_width * 9.0, p.y + 11.0 + 8.5 * i, "V08", NULL);
				nvgFillColor(args.vg, module->colors[13]);
				for (i = 0; i < 32; ++i)
					nvgText(args.vg, p.x + 2.0 + char_width * 12.0, p.y + 11.0 + 8.5 * i, "v44", NULL);
			} else {
				nvgScissor(args.vg, RECT_ARGS(args.clipBox));
				//nvgSave(args.vg);

				/// FILL CANVAS
				//nvgFillColor(args.vg, nvgRGBA(0xff, 0x00, 0x00, 0x60));
				//nvgFill(args.vg);

				/// DRAW ON CANVAS
				nvgStrokeColor(args.vg, nvgRGBA(0xff, 0x8C, 0x00, 0xff));
				nvgStrokeWidth(args.vg, 1.5f);
				{
					nvgLineCap(args.vg, NVG_ROUND);
					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, corner_tl.x, corner_tl.y);
					nvgLineTo(args.vg, corner_br.x, corner_br.y);
					nvgStroke(args.vg);
					nvgClosePath(args.vg);
					nvgStroke(args.vg);
				}

				//nvgRestore(args.vg);
				nvgResetScissor(args.vg);
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
		display = createWidget<TrackerDisplay>(mm2px(Vec(20.25, 7.15)));
		//display->box.size = mm2px(Vec(181.65, 85.75));
		display->box.size = mm2px(Vec(181.65, 94.5));
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