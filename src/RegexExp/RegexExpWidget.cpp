
#include "RegexExp.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

RegexExpWidget::RegexExpWidget(RegexExp* _module) {
	PortWidget			*output;
	float				step;
	int					i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance,
	/**/ "res/Regex-Gate-Condensed.svg")));

	/// ADD INPUTS / OUTPUTS
	step = 8.265;
	for (i = 0; i < 12; ++i) {
		output =
		/**/ createOutputCentered<Outlet>(mm2px(Vec(7.62, 8.5 + step * i)),
		/**/ module,
		/**/ RegexExp::OUTPUT_EXP + i);
		addOutput(output);
	}

	this->mode = REGEXEXP_MODE_CONDENSED;
}

void RegexExpWidget::step(void) {
	PortWidget	*port;
	Module		*exp;
	float		step;
	int			i;

	if (this->module == NULL)
		return;

	exp = this->module->leftExpander.module;
	if (exp == NULL)
		return;

	/// EXPANDER OF REGULAR
	if (exp->model->slug == "Biset-Regex") {
		if (this->mode != REGEXEXP_MODE_REGULAR) {
			this->mode = REGEXEXP_MODE_REGULAR;
			step = 13.0;
			this->setPanel(createPanel(asset::plugin(pluginInstance,
			/**/ "res/Regex-Gate.svg")));
			for (i = 0; i < 12; ++i) {
				port = this->getOutput(i);
				port->box.pos.y = mm2px(8.5 + step * i)
				/**/ - port->box.size.y * 0.5;
				port->visible = (i < 8);
				if (i >= 8)
					APP->scene->rack->clearCablesOnPort(port);
			}
		}
	/// EXPANDER OF CONDENSED
	} else if (exp->model->slug == "Biset-Regex-Condensed") {
		if (this->mode != REGEXEXP_MODE_CONDENSED) {
			this->mode = REGEXEXP_MODE_CONDENSED;
			step = 8.265;
			this->setPanel(createPanel(asset::plugin(pluginInstance,
			/**/ "res/Regex-Gate-Condensed.svg")));
			for (i = 0; i < 12; ++i) {
				port = this->getOutput(i);
				port->box.pos.y = mm2px(8.5 + step * i)
				/**/ - port->box.size.y * 0.5;
				port->visible = true;
			}
		}
	}
}
