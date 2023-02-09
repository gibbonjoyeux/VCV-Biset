
#include "plugin.hpp"


struct TrackerNote {
	u8	pitch;
	float	beat;
	float	length;
	float	velocity;
	float	chance;
};

struct TrackerNotePlay {
	float	frequency;
	float	remaining;
	float	velocity;
};

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
	//dsp::SchmittTrigger	clock_trig;
	//dsp::PulseGenerator	pulse_gen;
	dsp::TTimer<float>	clock_timer;
	float			clock_time;
	float			clock_time_p;
	//u8	track_gate[16] = 	{1,  0,  1,  1,  0,  1,  0,  0,  1,  1,  1,  0,  1,  1,  0,  0};
	//u8	track_pitch[16] =	{69, 69, 71, 73, 73, 76, 76, 76, 73, 71, 80, 80, 80, 73, 73, 73};
	//int	track_step =		4;
	//int	track_i =		0;
	//int	track_i_step =		0;
	//bool	track_clock = false;

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

		clock_timer.reset();
	}

	void	process(const ProcessArgs& args) override {
		float	bpm, bpm_rate;

		/// COMPUTE CLOCK
		bpm = params[PARAM_BPM].getValue();
		bpm_rate = (bpm * args.sampleTime) / 60.0f;
		clock_time_p = clock_timer.time;
		clock_timer.process(bpm_rate);
		if (clock_timer.time >= 64.0f)
			clock_timer.time -= 64.0f;
		clock_time = clock_timer.time;

		if (clock_time_p - (int)clock_time_p > clock_time - (int)clock_time) {
			outputs[OUTPUT_CLOCK].setVoltage(10.0f);
		} else {
			outputs[OUTPUT_CLOCK].setVoltage(0.0f);
		}


		/// USE / MODIFY EXPANDERS
		//if (rightExpander.module) {
		//	rightExpander.module->params[0].setValue(0);
		//}
	}
};

struct TrackerDisplay : LedDisplay {
	Tracker*		module;
	ModuleWidget*		moduleWidget;

	TrackerDisplay() {
	}

	void drawLayer(const DrawArgs& args, int layer) override {
		Rect	rect;
		Vec	corner_bl, corner_tl;
		Vec	corner_br, corner_tr;

		if (layer == 1 && module) {
			/// GET CANVAS FORMAT
			rect = box.zeroPos();
			corner_bl = rect.getBottomLeft();
			corner_tl = rect.getTopLeft();
			corner_br = rect.getBottomRight();
			corner_tr = rect.getTopRight();

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
		//font_path = asset::system("res/FT88-Regular.ttf");
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
				nvgFillColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));
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
		display->box.size = mm2px(Vec(181.65, 85.75));
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
