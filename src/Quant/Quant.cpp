
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

			/// ADD TEXT FIELD
			field = new MenuTextField((char*)"0.0");
			field->box.size.x = 120.0;
			menu->addChild(field);
			APP->event->setSelectedWidget(field);
			field->selectAll();
			/// ADD CONFIRM BUTTON
			menu->addChild(rack::createMenuItem("Create note", "",
				[=](void) {
					float	cent;

					cent = atof(field->text.c_str());
					this->note_add(cent);
				}
			));
		}
	));
	/// ADD RATIO SUB-MENU
	menu->addChild(rack::createSubmenuItem("Note as ratio", "",
		[=](Menu *menu) {
			MenuTextField	*field;

			/// ADD TEXT FIELD
			field = new MenuTextField((char*)"1:1");
			field->box.size.x = 120.0;
			menu->addChild(field);
			APP->event->setSelectedWidget(field);
			field->selectAll();
			/// ADD CONFIRM BUTTON
			menu->addChild(rack::createMenuItem("Create note", "",
				[=](void) {
					char	*str;
					int		numerator;
					int		denominator;
					int		i;

					str = (char*)field->text.c_str();
					numerator = 0;
					i = 0;
					while (str[i] >= '0' && str[i] <= '9') {
						numerator = numerator * 10 + str[i] - '0';
						i += 1;
					}
					if (str[i] == ':' || str[i] == '/') {
						i += 1;
						denominator = 0;
						while (str[i] >= '0' && str[i] <= '9') {
							denominator = denominator * 10 + str[i] - '0';
							i += 1;
						}
						this->note_add(numerator, denominator);
					}
				}
			));
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

