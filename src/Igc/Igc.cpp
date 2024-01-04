
#include "Igc.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Igc::Igc(void) {
	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	this->buffer_i = 0;
}

void Igc::process(const ProcessArgs& args) {
	std::vector<CableWidget*>	cables;
	CableWidget					*widget;
	Cable						*cable;
	Output						*output;
	int							i;

	if (args.frame % 32 != 0)
		return;

	cables = APP->scene->rack->getCompleteCables();
	this->count = cables.size();
	if (this->count >= IGC_CABLES)
		this->count = IGC_CABLES - 1;
	for (i = 0; i < this->count; ++i) {
		widget = cables[i];
		cable = widget->cable;
		/// STORE CABLE POSITION
		this->cables[i].pos_out = widget->getInputPos();
		this->cables[i].pos_in = widget->getOutputPos();
		this->cables[i].color = widget->color;
		/// STORE CABLE BUFFER
		if (cable && cable->outputModule && cable->outputId >= 0) {
			output = &(cable->outputModule->outputs[cable->outputId]);
			this->cables[i].buffer[this->buffer_i] = output->getVoltageSum();
		}
	}
	this->buffer_i += 1;
	if (this->buffer_i >= IGC_BUFFER)
		this->buffer_i = 0;
}

Model* modelIgc = createModel<Igc, IgcWidget>("Biset-Igc");
