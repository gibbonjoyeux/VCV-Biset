
#include "../plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

struct TrackerOut: Module {
	enum	ParamIds {
		PARAM_SYNTH,
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_PITCH,
		OUTPUT_GATE,
		OUTPUT_VELOCITY,
		OUTPUT_PANNING,
		ENUMS(OUTPUT_CV, 8),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	TrackerOut() {
		int			i;

		config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);
		configParam(PARAM_SYNTH, 0, 64, 0, "Synth")->displayPrecision = 0;
		configOutput(OUTPUT_PITCH, "Pitch");
		configOutput(OUTPUT_GATE, "Gate");
		configOutput(OUTPUT_VELOCITY, "Velocity");
		configOutput(OUTPUT_PANNING, "Panning");
		for (i = 0; i < 8; ++i)
			configOutput(OUTPUT_CV + i, string::f("CV %d", i + 1));
	}

	void	process(const ProcessArgs& args) override {
		Synth		*synth;
		int			i;

		synth = &(g_timeline.synths[(int)this->params[PARAM_SYNTH].getValue()]);
		this->outputs[OUTPUT_PITCH].setChannels(synth->channel_count);
		this->outputs[OUTPUT_GATE].setChannels(synth->channel_count);
		this->outputs[OUTPUT_VELOCITY].setChannels(synth->channel_count);
		this->outputs[OUTPUT_PANNING].setChannels(synth->channel_count);
		/// SET OUTPUT SYNTH
		for (i = 0; i < synth->channel_count; ++i) {
			this->outputs[OUTPUT_PITCH].setVoltage(synth->out_synth[i * 4 + 0], i);
			this->outputs[OUTPUT_GATE].setVoltage(synth->out_synth[i * 4 + 1], i);
			this->outputs[OUTPUT_VELOCITY].setVoltage(synth->out_synth[i * 4 + 2], i);
			this->outputs[OUTPUT_PANNING].setVoltage(synth->out_synth[i * 4 + 3], i);
		}
		/// SET OUTPUT CV
		for (i = 0; i < 8; ++i)
			this->outputs[OUTPUT_CV + i].setVoltage(synth->out_cv[i]);
	}
};

struct TrackerOutDisplay : LedDisplay {
	TrackerOut			*module;
	ModuleWidget		*moduleWidget;
	std::string			font_path;
	char				str_synth[4];

	TrackerOutDisplay() {
		font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
	}

	void drawLayer(const DrawArgs& args, int layer) override {
		std::shared_ptr<Font>	font;
		Rect				rect;
		Vec					p;
		int					synth;

		font = APP->window->loadFont(font_path);
		if (layer == 1 && module && font) {
			/// GET CANVAS FORMAT
			rect = box.zeroPos();
			p = rect.getTopLeft();

			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
			nvgRect(args.vg, RECT_ARGS(rect));
			nvgFill(args.vg);

			synth = module->params[TrackerOut::PARAM_SYNTH].getValue();
			itoa(synth, str_synth, 10);
			nvgFontSize(args.vg, 42);
			nvgFontFaceId(args.vg, font->handle);
			nvgFillColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));
			nvgText(args.vg, p.x + 25.0, p.y + 36.0, str_synth, NULL);
		}
		LedDisplay::drawLayer(args, layer);
	}
};

struct TrackerOutWidget : ModuleWidget {
	TrackerOut					*module;

	TrackerOutWidget(TrackerOut* _module) {
		TrackerOutDisplay		*display;
		int						i;

		module = _module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-Out.svg")));

		addParam(
		/**/ createParamCentered<KnobMedium>(mm2px(Vec(25.0, 30.0)),
		/**/ module,
		/**/ TrackerOut::PARAM_SYNTH));

		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(10.5, 43.0)),
		/**/ module,
		/**/ TrackerOut::OUTPUT_PITCH));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(10.5 + 10.0, 43.0)),
		/**/ module,
		/**/ TrackerOut::OUTPUT_GATE));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(10.5 + 10.0 * 2.0, 43.0)),
		/**/ module,
		/**/ TrackerOut::OUTPUT_VELOCITY));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(10.5 + 10.0 * 3.0, 43.0)),
		/**/ module,
		/**/ TrackerOut::OUTPUT_PANNING));

		for (i = 0; i < 4; ++i) {
			addOutput(
			/**/ createOutputCentered<Outlet>(mm2px(Vec(20.5, 65.0 + 10.0 * i)),
			/**/ module,
			/**/ TrackerOut::OUTPUT_CV + i * 2));
			addOutput(
			/**/ createOutputCentered<Outlet>(mm2px(Vec(20.5 + 10.7, 65.0 + 10.0 * i)),
			/**/ module,
			/**/ TrackerOut::OUTPUT_CV + i * 2 + 1));
		}

		/// MAIN LED DISPLAY
		display = createWidget<TrackerOutDisplay>(mm2px(Vec(4.0, 4.0)));
		display->box.size = mm2px(Vec(39, 18.0));
		display->module = module;
		display->moduleWidget = this;
		addChild(display);
	}

	void onSelect(const SelectEvent &e) override {
		int		synth;

		synth = this->module->params[TrackerOut::PARAM_SYNTH].getValue();
		g_editor.set_synth(synth, true);
	}
};

Model* modelTrackerOut = createModel<TrackerOut, TrackerOutWidget>("TrackerOut");
