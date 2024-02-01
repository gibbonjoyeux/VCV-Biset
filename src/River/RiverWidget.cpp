
#include "River.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

RiverWidget::RiverWidget(River* _module) {
	RiverDisplay	*display;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tree.svg")));

	display = createWidget<RiverDisplay>(mm2px(Vec(0.0, 0.0)));
	display->box.size = mm2px(Vec(55.88, 60.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(10.0, 70.0)),
	/**/ module,
	/**/ River::PARAM_SELECT_WHEEL));
	addParam(
	/**/ createParamCentered<ButtonTrigger>(mm2px(Vec(20.0, 70.0)),
	/**/ module,
	/**/ River::PARAM_SELECT_CLICK));
}

void RiverWidget::onPathDrop(const PathDropEvent &e) {
	std::vector<std::string>::const_iterator	it, it_end;

	if (this->module == NULL)
		return;
	it = e.paths.begin();
	it_end = e.paths.end();
	if (it != it_end) {
		/// IS DIRECTORY
		/// IS FILE
	}
	e.consume(this);
}
