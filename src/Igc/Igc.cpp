
#include "Igc.hpp"

Igc *g_igc = NULL;

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Igc::Igc(void) {
	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configSwitch(PARAM_SCOPE_ENABLED, 0, 1, 1);
	configSwitch(PARAM_SCOPE_BACKGROUND, 0, 1, 1);
	configSwitch(PARAM_SCOPE_DETAILS, 0, 1, 1);
	configSwitch(PARAM_SCOPE_MODE, 0, 1, 0);
	configSwitch(PARAM_SCOPE_POSITION, 0, 4, 0);
	configParam(PARAM_SCOPE_SCALE, 0.02, 1, 0.2, "Scope scale", "%", 0, 100);
	configParam(PARAM_SCOPE_ALPHA, 0, 1, 0.8, "Scope alpha", "%", 0, 100);

	configSwitch(PARAM_CABLE_ENABLED, 0, 1, 1);
	configSwitch(PARAM_CABLE_BRIGHTNESS, 0, 1, 1);
	configSwitch(PARAM_CABLE_LED, 0, 1, 1);
	configSwitch(PARAM_CABLE_POLY_THICK, 0, 1, 0);
	configSwitch(PARAM_CABLE_POLY_MODE, 0, 2, 0);
	configParam(PARAM_CABLE_SCALE, 0.0, 2.0, 1.0, "Cable scale", "%", 0, 100);

	this->buffer_i = 0;

	this->display = NULL;
	this->scope = NULL;
}

Igc::~Igc(void) {
	if (this->display) {
		APP->scene->rack->removeChild(this->display);
		delete this->display;
	}
	if (this->scope) {
		APP->scene->removeChild(this->scope);
		delete this->scope;
	}
	if (this == g_igc) {
		g_igc = NULL;
		APP->scene->rack->getCableContainer()->show();
	}
}

void Igc::processBypass(const ProcessArgs& args) {
	if (this == g_igc) {
		APP->scene->rack->getCableContainer()->show();
		if (this->display)
			this->display->hide();
	}
}

void Igc::process(const ProcessArgs& args) {
	PortWidget					*port_widget;
	Port						*port;
	std::vector<CableWidget*>	cables;
	CableWidget					*widget;
	Cable						*cable;
	Output						*output;
	Widget						*hovered;
	bool						poly_thick;
	i8							poly_mode;
	int							channels;
	int							i;

	if (args.frame % 32 != 0)
		return;
	if (g_igc == NULL)
		g_igc = this;
	if (g_igc != this)
		return;

	/// [1] GET PARAMETERS
	poly_thick = this->params[Igc::PARAM_CABLE_POLY_THICK].getValue();
	poly_mode = this->params[Igc::PARAM_CABLE_POLY_MODE].getValue();
	if (this->params[Igc::PARAM_CABLE_ENABLED].getValue()) {
		APP->scene->rack->getCableContainer()->hide();
		if (this->display)
			this->display->show();
	} else {
		APP->scene->rack->getCableContainer()->show();
		if (this->display)
			this->display->hide();
	}

	/// [2] GET CABLES
	hovered = APP->event->hoveredWidget;
	cables = APP->scene->rack->getCompleteCables();
	this->cable_count = cables.size();
	if (this->cable_count >= IGC_CABLES)
		this->cable_count = IGC_CABLES - 1;

	/// [3] RECORD CABLES
	this->scope_index = -1;
	for (i = 0; i < this->cable_count; ++i) {
		widget = cables[i];
		cable = widget->cable;
		/// STORE CABLE POSITION
		this->cables[i].pos_out = widget->getInputPos();
		this->cables[i].pos_in = widget->getOutputPos();
		this->cables[i].color = widget->color;
		/// STORE CABLE BUFFER
		if (cable && cable->outputModule && cable->outputId >= 0) {
			output = &(cable->outputModule->outputs[cable->outputId]);
			channels = output->getChannels();
			if (channels == 0)
				channels = 1;
			this->cables[i].thick = (channels > 1 && poly_thick);
			if (poly_mode == IGC_CABLE_POLY_FIRST) {
				this->cables[i].buffer[this->buffer_i] =
				/**/ output->getVoltage();
			} else if (poly_mode == IGC_CABLE_POLY_SUM) {
				this->cables[i].buffer[this->buffer_i] =
				/**/ output->getVoltageSum();
			} else  {
				this->cables[i].buffer[this->buffer_i] =
				/**/ output->getVoltageSum() / channels;
			}
		}
		/// CHECK HOVER
		if (widget->outputPort == hovered || widget->inputPort == hovered)
			this->scope_index = i;
	}

	/// [4] RECORD HOVERED PORT
	if (this->scope_index < 0 && hovered) {
		port_widget = dynamic_cast<PortWidget*>(hovered);
		if (port_widget && port_widget->type == engine::Port::OUTPUT) {
			port = port_widget->getPort();
			if (port) {
				/// STORE PORT TO EXTRA CABLE
				this->scope_index = IGC_CABLES;
				this->cables[IGC_CABLES].color = {1, 1, 1, 1};
				this->cables[IGC_CABLES].buffer[this->buffer_i] =
				/**/ port->voltages[0];
			}
		}
	}

	/// [5] RECORD INCOMPLETE CABLE
	widget = APP->scene->rack->getIncompleteCable();
	this->cable_incomplete = IGC_CABLE_INCOMPLETE_OFF;
	if (widget) {
		if (widget->inputPort)
			this->cable_incomplete = IGC_CABLE_INCOMPLETE_IN;
		else
			this->cable_incomplete = IGC_CABLE_INCOMPLETE_OUT;
		this->scope_index = -1;
		this->cables[IGC_CABLES].pos_out = widget->getInputPos();
		this->cables[IGC_CABLES].pos_in = widget->getOutputPos();
		this->cables[IGC_CABLES].color = widget->color;
	}

	/// [6] STEP BUFFER
	this->buffer_i += 1;
	if (this->buffer_i >= IGC_BUFFER)
		this->buffer_i = 0;
}

Model* modelIgc = createModel<Igc, IgcWidget>("Biset-Igc");
