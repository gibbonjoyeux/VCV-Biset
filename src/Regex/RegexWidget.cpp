
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

RegexWidget::RegexWidget(Regex* _module) {
	RegexDisplay		*display_rythm;
	RegexDisplay		*display_pitch;
	RegexDisplay		*display_prev;
	int						i;

	this->module = _module;
	setModule(this->module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Regex.svg")));

	//addParam(
	///**/ createParamCentered<KnobMedium>(mm2px(Vec(5.5, 5.5)),
	///**/ module,
	///**/ Regex::PARAM_BPM));

	//addOutput(
	///**/ createOutputCentered<Outlet>(mm2px(Vec(12.85, 36.6)),
	///**/ module,
	///**/ Regex::OUTPUT_PITCH));
	//addOutput(
	///**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06, 36.6)),
	///**/ module,
	///**/ Regex::OUTPUT_GATE));
	//addOutput(
	///**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06 * 2.0, 36.6)),
	///**/ module,
	///**/ Regex::OUTPUT_VELOCITY));
	//addOutput(
	///**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06 * 3.0, 36.6)),
	///**/ module,
	///**/ Regex::OUTPUT_PANNING));

	//addOutput(
	///**/ createOutputCentered<Outlet>(mm2px(Vec(12.85, 15.0)),
	///**/ module,
	///**/ Regex::OUTPUT_CLOCK));
	//addOutput(
	///**/ createOutputCentered<Outlet>(mm2px(Vec(12.85, 30.0)),
	///**/ module,
	///**/ Regex::OUTPUT_BPM));
	for (i = 0; i < 4; ++i) {
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(104.5, 8.5 + 26.0 * i + 2.0)),
		/**/ module,
		/**/ Regex::OUTPUT_GATE + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(104.5, 8.5 + 26.0 * i + 13.0 - 2.0)),
		/**/ module,
		/**/ Regex::OUTPUT_PITCH + i));
	}

	/// MAIN LED DISPLAY
	display_prev = NULL;
	for (i = 0; i < 4; ++i) {
		/// DISPLAY RYTHM
		display_rythm = createWidget<RegexDisplay>(mm2px(Vec(3.0, 3.0 + 26.0 * i)));
		display_rythm->box.size = mm2px(Vec(95.0, 10.0));
		display_rythm->module = module;
		display_rythm->moduleWidget = this;
		if (this->module) {
			display_rythm->sequence = &(this->module->sequences[i]);
			this->module->sequences[i].display_rythm = display_rythm;
		}
		this->display_rythm[i] = display_rythm;
		addChild(display_rythm);
		/// DISPLAY PITCH
		display_pitch = createWidget<RegexDisplay>(mm2px(Vec(3.0, 3.0 + 26.0 * i + 13.0)));
		display_pitch->box.size = mm2px(Vec(95.0, 10.0));
		display_pitch->module = module;
		display_pitch->moduleWidget = this;
		display_pitch->bgColor = colors[14];
		if (this->module) {
			display_pitch->sequence = &(this->module->sequences[i]);
			this->module->sequences[i].display_pitch = display_pitch;
		}
		this->display_pitch[i] = display_pitch;
		addChild(display_pitch);
		/// SET PREV / NEXT
		display_rythm->display_prev = display_prev;
		display_rythm->display_next = display_pitch;
		display_pitch->display_prev = display_rythm;
		display_pitch->display_next = NULL;
		if (display_prev)
			display_prev->display_next = display_rythm;
		display_prev = display_pitch;
	}
}
