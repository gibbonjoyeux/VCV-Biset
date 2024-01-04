
#include "Igc.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Igc::Igc(void) {
	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configSwitch(PARAM_SCOPE_ENABLED, 0, 1, 1);
	configSwitch(PARAM_SCOPE_DETAILS, 0, 1, 1);
	configSwitch(PARAM_SCOPE_MODE, 0, 1, 0);
	configSwitch(PARAM_SCOPE_POSITION, 0, 4, 0);
	configParam(PARAM_SCOPE_SCALE, 0.02, 1, 0.2, "Scope scale", "%", 0, 100);
	configParam(PARAM_SCOPE_ALPHA, 0, 1, 0.8, "Scope alpha", "%", 0, 100);

	this->buffer_i = 0;

	this->scope = NULL;
}

Igc::~Igc(void) {
	if (this->scope) {
		this->scope->requestDelete();
		this->scope = NULL;
	}
}

void Igc::onRemove(const RemoveEvent &e) {
}

void Igc::process(const ProcessArgs& args) {
	std::vector<CableWidget*>	cables;
	CableWidget					*widget;
	Cable						*cable;
	Output						*output;
	Widget						*hovered;
	int							i;

	if (args.frame % 32 != 0)
		return;

	/// [1] GET CABLES
	hovered = APP->event->hoveredWidget;
	cables = APP->scene->rack->getCompleteCables();
	this->count = cables.size();
	if (this->count >= IGC_CABLES)
		this->count = IGC_CABLES - 1;
	/// [2] RECORD CABLES
	this->scope_index = -1;
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
		/// CHECK HOVER
		if (widget->outputPort == hovered || widget->inputPort == hovered)
			this->scope_index = i;
	}

	/// [3] RECORD HOVERED CABLE / PORT
	// TODO: check if hovered is not cable but port

	/// [4] STEP BUFFER
	this->buffer_i += 1;
	if (this->buffer_i >= IGC_BUFFER)
		this->buffer_i = 0;
}

Model* modelIgc = createModel<Igc, IgcWidget>("Biset-Igc");
