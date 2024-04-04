
#include "Igc.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

IgcIoWidget::IgcIoWidget(void) {
	std::shared_ptr<window::Svg>	svg;

	fb = new widget::FramebufferWidget;
	addChild(fb);

	sw_in = new widget::SvgWidget;
	fb->addChild(sw_in);
	sw_out = new widget::SvgWidget;
	fb->addChild(sw_out);

	svg = Svg::load(asset::plugin(pluginInstance, "res/Igc-Input-Mono.svg"));
	sw_in->setSvg(svg);
	fb->box.size = sw_in->box.size;
	box.size = sw_in->box.size;
	fb->setDirty();

	svg = Svg::load(asset::plugin(pluginInstance, "res/Igc-Output-Stereo.svg"));
	sw_out->setSvg(svg);
	fb->box.size = sw_out->box.size;
	box.size = sw_out->box.size;
	fb->setDirty();

	this->mode_input = 0;
	this->mode_output = 0;
}

void IgcIoWidget::step(void) {
	std::shared_ptr<window::Svg>	svg;
	int								mode;

	if (this->module == NULL)
		return;

	/// HANDLE INPUT MODE (MONO vs STEREO)
	mode = (this->module->inputs[Igc::INPUT_R].isConnected());
	if (mode != this->mode_input) {
		this->mode_input = mode;

		if (mode == 0)
			svg = Svg::load(asset::plugin(pluginInstance, "res/Igc-Input-Mono.svg"));
		else
			svg = Svg::load(asset::plugin(pluginInstance, "res/Igc-Input-Stereo.svg"));
		sw_in->setSvg(svg);
		fb->box.size = sw_in->box.size;
		box.size = sw_in->box.size;
		fb->setDirty();
	}

	/// HANDLE OUTPUT MODE (STEREO vs STEREO SPREAD)
	mode = (this->module->params[Igc::PARAM_MODE_OUTPUT].getValue());
	if (mode != this->mode_output) {
		this->mode_output = mode;

		if (mode < IGC_MODE_OUT_STEREO_SPREAD)
			svg = Svg::load(asset::plugin(pluginInstance, "res/Igc-Output-Stereo.svg"));
		else
			svg = Svg::load(asset::plugin(pluginInstance, "res/Igc-Output-Spread.svg"));
		sw_out->setSvg(svg);
		fb->box.size = sw_out->box.size;
		box.size = sw_out->box.size;
		fb->setDirty();
	}
}
