
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
			this->cables[i].buffer[this->buffer_i] =
			/**/ cable->outputModule->outputs[cable->outputId].getVoltage();
		}

		//pos_in = cables[i]->getInputPos();
		//pos_out = cables[i]->getOutputPos();
		//pos_slump = getSlumpPos(pos_out, pos_in);
		//this->cables[i].pos_out =
		///**/ pos_out.plus(pos_slump.minus(pos_out).normalize().mult(13.0));
		//this->cables[i].pos_in =
		///**/ pos_in.plus(pos_slump.minus(pos_in).normalize().mult(13.0));
	}
	this->buffer_i += 1;
	if (this->buffer_i >= IGC_BUFFER)
		this->buffer_i = 0;
}

Model* modelIgc = createModel<Igc, IgcWidget>("Biset-Igc");
