
#include "Quant.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Quant::Quant(void) {
	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);
	/// CONFIG PARAMS
	/// CONFIG INPUTS / OUTPUTS
	configInput(INPUT_PITCH, "Pitch");
	configInput(INPUT_WHEEL, "Pitch wheel");
	//// CONFIG OUTPUTS KEYS
	configOutput(OUTPUT_PITCH, "Pitch");
}

void Quant::process(const ProcessArgs& args) {
}

//void Quant::onReset(const ResetEvent &e) {
//	Module::onReset(e);
//}

void Quant::note_add_menu(void) {
	Menu		*menu;

	menu = createMenu();
	/// ADD CENT SUB-MENU
	menu->addChild(rack::createSubmenuItem("Note as cent", "",
		[=](Menu *menu) {
			MenuTextField	*field;

			field = new MenuTextField((char*)"0.0",
				[=](char *value) {
					float	cent;

					cent = atof(value);
					this->note_add(cent);
				}, true
			);
			field->box.size.x = 120.0;
			menu->addChild(field);
			APP->event->setSelectedWidget(field);
			field->selectAll();
		}
	));
	/// ADD RATIO SUB-MENU
	menu->addChild(rack::createSubmenuItem("Note as ratio", "",
		[=](Menu *menu) {
			MenuTextField	*field;

			field = new MenuTextField((char*)"1:1",
				[=](char *value) {
					int		numerator;
					int		denominator;
					int		i;

					numerator = 0;
					i = 0;
					while (value[i] >= '0' && value[i] <= '9') {
						numerator = numerator * 10 + value[i] - '0';
						i += 1;
					}
					if (value[i] == ':' || value[i] == '/') {
						i += 1;
						denominator = 0;
						while (value[i] >= '0' && value[i] <= '9') {
							denominator = denominator * 10 + value[i] - '0';
							i += 1;
						}
						this->note_add(numerator, denominator);
					}
				}, true
			);
			field->box.size.x = 120.0;
			menu->addChild(field);
			APP->event->setSelectedWidget(field);
			field->selectAll();
		}
	));
}

void Quant::note_add(float cent) {
	this->notes.emplace_back(cent);
}

void Quant::note_add(int numerator, int denominator) {
	this->notes.emplace_back(numerator, denominator);
}

void Quant::note_del(int index) {
	this->notes.erase(this->notes.begin() + index);
}

Model* modelQuant = createModel<Quant, QuantWidget>("Biset-Quant");

