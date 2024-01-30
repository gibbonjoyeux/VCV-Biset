
#include "Blank.hpp"

Blank *g_blank = NULL;

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Blank::Blank(void) {
	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configSwitch(PARAM_SCOPE_ENABLED, 0, 1, 1);
	configSwitch(PARAM_SCOPE_MAJ, 0, 1, 0);
	configSwitch(PARAM_SCOPE_MODE, 0, 1, 0);
	configSwitch(PARAM_SCOPE_POSITION, 0, 4, 0);
	configParam(PARAM_SCOPE_SCALE, 0.02, 1, 0.2, "Scope scale", "%", 0, 100);
	configParam(PARAM_SCOPE_THICKNESS, 1, 10, 2, "Scope thickness", "");
	configParam(PARAM_SCOPE_BACK_ALPHA, 0, 1, 0.6, "Scope background alpha", "%", 0, 100);
	configParam(PARAM_SCOPE_VOLT_ALPHA, 0, 1, 0.3, "Scope voltage alpha", "%", 0, 100);
	configParam(PARAM_SCOPE_LABEL_ALPHA, 0, 1, 1, "Scope label alpha", "%", 0, 100);
	configParam(PARAM_SCOPE_ALPHA, 0, 1, 1, "Scope alpha", "%", 0, 100);

	configSwitch(PARAM_CABLE_ENABLED, 0, 1, 1);
	configSwitch(PARAM_CABLE_BRIGHTNESS, 0, 1, 1);
	configSwitch(PARAM_CABLE_POLY_THICK, 0, 1, 1);
	configSwitch(PARAM_CABLE_POLY_MODE, 0, 2, 0);
	configSwitch(PARAM_CABLE_FAST, 0, 1, 0);
	configParam(PARAM_CABLE_SLEW, 0.0, 1.0, 0.0, "Cable slew", "%", 0, 100);
	configParam(PARAM_CABLE_SCALE, 0.0, 2.0, 1.0, "Cable scale", "%", 0, 100);

	this->buffer_i = 0;

	this->display = NULL;
	this->scope = NULL;
}

Blank::~Blank(void) {
	if (this->display) {
		if (APP->scene->rack->hasChild(this->display))
			APP->scene->rack->removeChild(this->display);
		delete this->display;
	}
	if (this->scope) {
		if (APP->scene->hasChild(this->scope))
			APP->scene->removeChild(this->scope);
		delete this->scope;
	}
	if (this == g_blank) {
		g_blank = NULL;
		APP->scene->rack->getCableContainer()->show();
	}
}

void Blank::processBypass(const ProcessArgs& args) {
	if (this == g_blank) {
		APP->scene->rack->getCableContainer()->show();
		if (this->display)
			this->display->hide();
	}
}

void Blank::process(const ProcessArgs& args) {
	std::list<Widget*>::iterator	it, it_end;
	Widget							*container;
	CableWidget						*widget;
	Cable							*cable;
	PortInfo						*port_info;
	PortWidget						*port_widget;
	Port							*port;
	Output							*output;
	PortWidget						*hovered;
	bool							scope;
	bool							poly_thick;
	i8								poly_mode;
	int								channels;
	int								i;

	if (args.frame % 32 != 0)
		return;
	if (g_blank == NULL)
		g_blank = this;
	if (g_blank != this)
		return;

	/// [1] GET PARAMETERS
	if (this->params[Blank::PARAM_SCOPE_MAJ].getValue())
		scope = ((APP->window->getMods() & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT);
	else
		scope = true;
	poly_thick = this->params[Blank::PARAM_CABLE_POLY_THICK].getValue();
	poly_mode = this->params[Blank::PARAM_CABLE_POLY_MODE].getValue();
	if (this->params[Blank::PARAM_CABLE_ENABLED].getValue()) {
		APP->scene->rack->getCableContainer()->hide();
		if (this->display)
			this->display->show();
	} else {
		APP->scene->rack->getCableContainer()->show();
		if (this->display)
			this->display->hide();
	}
	hovered = dynamic_cast<PortWidget*>(APP->event->hoveredWidget);
	this->scope_label[0] = 0;

	/// [2] GET CABLE CONTAINER
	container = APP->scene->rack->getCableContainer();
	it = container->children.begin();
	it_end = container->children.end();

	/// [3] RECORD CABLES
	i = 0;
	this->scope_index = -1;
	while (it != it_end) {
		/// GET CABLE
		widget = dynamic_cast<CableWidget*>(*it);
		if (widget == NULL || widget->isComplete() == false) {
			++it;
			continue;
		}
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
			if (poly_mode == BLANK_CABLE_POLY_FIRST) {
				this->cables[i].buffer[this->buffer_i] =
				/**/ output->getVoltage();
			} else if (poly_mode == BLANK_CABLE_POLY_SUM) {
				this->cables[i].buffer[this->buffer_i] =
				/**/ output->getVoltageSum();
			} else  {
				this->cables[i].buffer[this->buffer_i] =
				/**/ output->getVoltageSum() / channels;
			}
		}
		/// CHECK HOVER
		if (scope &&
		(widget->outputPort == hovered || widget->inputPort == hovered)) {
			/// SET INDEX
			this->scope_index = i;
			/// SET LABEL
			port_info = widget->outputPort->getPortInfo();
			if (port_info) {
				strncpy(this->scope_label,
				/**/ port_info->name.c_str(),
				/**/ BLANK_SCOPE_LABEL);
			}
			strcat(this->scope_label, " output to ");
			port_info = widget->inputPort->getPortInfo();
			if (port_info) {
				strncat(this->scope_label,
				/**/ port_info->name.c_str(),
				/**/ BLANK_SCOPE_LABEL);
			}
			strcat(this->scope_label, " input");
		}
		/// LOOP
		++it;
		++i;
		if (i >= BLANK_CABLES) {
			i = BLANK_CABLES - 1;
			return;
		}
	}
	this->cable_count = i;

	/// [4] RECORD HOVERED UNCONNECTED PORT
	if (this->scope_index < 0 && hovered && scope) {
		port_widget = dynamic_cast<PortWidget*>(hovered);
		if (port_widget && port_widget->type == engine::Port::OUTPUT) {
			port = port_widget->getPort();
			if (port) {
				/// STORE PORT TO EXTRA CABLE
				this->scope_index = BLANK_CABLES;
				this->cables[BLANK_CABLES].color = {1, 1, 1, 1};
				this->cables[BLANK_CABLES].buffer[this->buffer_i] =
				/**/ port->voltages[0];
				/// STORE PORT LABEL
				port_info = port_widget->getPortInfo();
				if (port_info) {
					strncpy(this->scope_label,
					/**/ port_info->name.c_str(),
					/**/ BLANK_SCOPE_LABEL - 1);
				}
			}
		}
	}

	/// [5] RECORD INCOMPLETE CABLE
	widget = APP->scene->rack->getIncompleteCable();
	this->cable_incomplete = BLANK_CABLE_INCOMPLETE_OFF;
	if (widget) {
		if (widget->inputPort)
			this->cable_incomplete = BLANK_CABLE_INCOMPLETE_IN;
		else
			this->cable_incomplete = BLANK_CABLE_INCOMPLETE_OUT;
		this->scope_index = -1;
		this->cables[BLANK_CABLES].pos_out = widget->getInputPos();
		this->cables[BLANK_CABLES].pos_in = widget->getOutputPos();
		this->cables[BLANK_CABLES].color = widget->color;
	}

	/// [6] STEP BUFFER
	this->buffer_i += 1;
	if (this->buffer_i >= BLANK_BUFFER)
		this->buffer_i = 0;
}

Model* modelBlank = createModel<Blank, BlankWidget>("Biset-Blank");
