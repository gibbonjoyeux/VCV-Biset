
#include "TrackerDrum.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerDrumWidget::TrackerDrumWidget(TrackerDrum* _module) {
	TrackerDrumDisplay		*display;
	float					x, y;
	int						i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker-Drum.svg")));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(27.5, 20.5)),
	/**/ module,
	/**/ TrackerDrum::PARAM_SYNTH));

	for (i = 0; i < 12; ++i) {
		if (i % 2 == 0) {
			x = 6.25;
			y = 36.6;
		} else {
			x = 33.23;
			y = 36.6;
		}
		y += 9.05 * (i / 2);
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(x, y)),
		/**/ module,
		/**/ TrackerDrum::OUTPUT_TRIGGER + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(x + 8.2, y)),
		/**/ module,
		/**/ TrackerDrum::OUTPUT_VELOCITY + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(x + 8.2 * 2, y)),
		/**/ module,
		/**/ TrackerDrum::OUTPUT_PANNING + i));
	}

	for (i = 0; i < 4; ++i) {
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06 * i, 94.75)),
		/**/ module,
		/**/ TrackerDrum::OUTPUT_CV + i));
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(12.85 + 10.06 * i, 103.75)),
		/**/ module,
		/**/ TrackerDrum::OUTPUT_CV + 4 + i));
	}

	/// MAIN LED DISPLAY
	display = createWidget<TrackerDrumDisplay>(mm2px(Vec(14.5 + 2.5, 4.0)));
	display->box.size = mm2px(Vec(21.25, 9.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);
}

void TrackerDrumWidget::onSelect(const SelectEvent &e) {
	int		synth;

	if (g_module == NULL || g_editor == NULL)
		return;

	synth = this->module->params[TrackerDrum::PARAM_SYNTH].getValue();
	g_editor->set_synth(synth);
}

void TrackerDrumWidget::onDeselect(const DeselectEvent &e) {
	ParamWidget		*param;

	if (g_module == NULL)
		return;
	if (this->module->map_learn == false)
		return;

	param = APP->scene->rack->getTouchedParam();
	if (param) {
		APP->scene->rack->touchedParam = NULL;
		this->module->learn_map(param->module->id, param->paramId);
	} else {
		this->module->learn_disable();
	}
}

void TrackerDrumWidget::appendContextMenu(Menu *menu) {
	int				i;

	if (g_module == NULL)
		return;

	menu->addChild(new MenuSeparator());

	for (i = 0; i < 8; ++i) {
		menu->addChild(rack::createSubmenuItem(string::f("CV %d", i + 1), "",
			[=](Menu *menu) {
				ParamHandleRange	*handle;
				MenuSliderEdit		*slider;
				rack::Widget		*holder;
				MenuItem			*item;
				int					j;

				/// [1] CV RANGE
				//// SLIDER MIN
				slider = new MenuSliderEdit(this->module->paramQuantities[TrackerDrum::PARAM_OUT_MIN + i]);
				slider->box.size.x = 200.f;
				menu->addChild(slider);
				//// SLIDER MAX
				slider = new MenuSliderEdit(this->module->paramQuantities[TrackerDrum::PARAM_OUT_MAX + i]);
				slider->box.size.x = 200.f;
				menu->addChild(slider);

				/// PRESETS
				//// HOLDER
				holder = new rack::Widget();
				holder->box.size.x = 200.0f;
				holder->box.size.y = 20.0f;
				//// +/-10
				item = new MenuItemStay("+/-10", "",	
					[=]() {
						this->module->params[TrackerDrum::PARAM_OUT_MIN + i].setValue(-10);
						this->module->params[TrackerDrum::PARAM_OUT_MAX + i].setValue(10);
					}
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				holder->addChild(item);
				//// +/-5
				item = new MenuItemStay("+/-5", "",
					[=]() {
						this->module->params[TrackerDrum::PARAM_OUT_MIN + i].setValue(-5);
						this->module->params[TrackerDrum::PARAM_OUT_MAX + i].setValue(5);
					}
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				item->box.pos.x = 50.0f;
				holder->addChild(item);
				//// +/-2
				item = new MenuItemStay("+/-2", "",
					[=]() {
						this->module->params[TrackerDrum::PARAM_OUT_MIN + i].setValue(-2);
						this->module->params[TrackerDrum::PARAM_OUT_MAX + i].setValue(2);
					}
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				item->box.pos.x = 100.0f;
				holder->addChild(item);
				//// +/-1
				item = new MenuItemStay("+/-1", "",
					[=]() {
						this->module->params[TrackerDrum::PARAM_OUT_MIN + i].setValue(-1);
						this->module->params[TrackerDrum::PARAM_OUT_MAX + i].setValue(1);
					}
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				item->box.pos.x = 150.0f;
				holder->addChild(item);
				menu->addChild(holder);

				//// HOLDER
				holder = new rack::Widget();
				holder->box.size.x = 200.0f;
				holder->box.size.y = 20.0f;
				//// +10
				item = new MenuItemStay("+10", "",	
					[=]() {
						this->module->params[TrackerDrum::PARAM_OUT_MIN + i].setValue(0);
						this->module->params[TrackerDrum::PARAM_OUT_MAX + i].setValue(10);
					}
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				holder->addChild(item);
				//// +5
				item = new MenuItemStay("+5", "",
					[=]() {
						this->module->params[TrackerDrum::PARAM_OUT_MIN + i].setValue(0);
						this->module->params[TrackerDrum::PARAM_OUT_MAX + i].setValue(5);
					}
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				item->box.pos.x = 50.0f;
				holder->addChild(item);
				//// +2
				item = new MenuItemStay("+2", "",
					[=]() {
						this->module->params[TrackerDrum::PARAM_OUT_MIN + i].setValue(0);
						this->module->params[TrackerDrum::PARAM_OUT_MAX + i].setValue(2);
					}
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				item->box.pos.x = 100.0f;
				holder->addChild(item);
				//// +1
				item = new MenuItemStay("+1", "",
					[=]() {
						this->module->params[TrackerDrum::PARAM_OUT_MIN + i].setValue(0);
						this->module->params[TrackerDrum::PARAM_OUT_MAX + i].setValue(1);
					}
				);
				item->box.size.x = 50.0f;
				item->box.size.y = 20.0f;
				item->box.pos.x = 150.0f;
				holder->addChild(item);

				menu->addChild(holder);

				menu->addChild(new MenuSeparator());

				/// [2] CV MAP
				for (j = 0; j < 4; ++j) {
					handle = &(this->module->map_handles[i][j]);
					menu->addChild(rack::createSubmenuItem(string::f("Map %d", j + 1),
						(handle->module != NULL) ? "Mapped" : "",
						[=](Menu *menu) {
							MenuLabel			*label;
							MenuItem			*item;
							MenuSlider			*slider;
							ParamQuantityLink	*param_link;
							ParamQuantity		*param;
							float				min;
							float				max;

							/// MAP EDIT
							if (handle->module != NULL) {
								param = handle->module->getParamQuantity(handle->paramId);
								/// MAPPED MODULE
								label = new MenuLabel();
								label->text = handle->module->model->name;
								menu->addChild(label);
								/// MAPPED PARAM
								label = new MenuLabel();
								label->text = "Mapped param";
								label->text = param->name;
								menu->addChild(label);
								/// MAP RANGE
								min = param->getMinValue();
								max = param->getMaxValue();
								//// MAP RANGE MIN
								param_link = (ParamQuantityLink*)this->module->paramQuantities[TrackerDrum::PARAM_MENU + 0];
								param_link->link = &(handle->min);
								param_link->minValue = min;
								param_link->maxValue = max;
								param_link->defaultValue = min;
								param_link->setValue(handle->min);
								param_link->name = "Min";
								slider = new MenuSlider(param_link);
								slider->box.size.x = 200.f;
								menu->addChild(slider);
								//// MAP RANGE MAX
								param_link = (ParamQuantityLink*)this->module->paramQuantities[TrackerDrum::PARAM_MENU + 1];
								param_link->link = &(handle->max);
								param_link->minValue = min;
								param_link->maxValue = max;
								param_link->defaultValue = max;
								param_link->setValue(handle->max);
								param_link->name = "Max";
								slider = new MenuSlider(param_link);
								slider->box.size.x = 200.f;
								menu->addChild(slider);
								/// EDIT
								menu->addChild(new MenuSeparator());
								item = createMenuItem("Unmap", "",
									[=]() {
										APP->engine->updateParamHandle(handle, -1, 0, true);
									}
								);
								menu->addChild(item);
							/// MAP LEARN
							} else {
								item = createMenuItem("Learn", "",
									[=]() {
										/// SELECT WIDGET
										APP->event->setSelectedWidget(this);
										APP->scene->rack->touchedParam = NULL;
										/// ENABLE LEARN
										this->module->learn_enable(i, j);
									}
								);
								menu->addChild(item);
							}
						}
					));
				}
			}
		));
	}
}