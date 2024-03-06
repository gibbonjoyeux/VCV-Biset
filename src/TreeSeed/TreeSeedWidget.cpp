
#include "TreeSeed.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TreeSeedWidget::TreeSeedWidget(TreeSeed* _module) {
	float		out_x, out_y;
	float		out_step;
	float		knob_y;
	int			output_id;
	int			i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tree-Seed.svg")));

	out_x = 20.32 / 2.0;
	out_y = 71.0 - 0.5;
	out_step = 8.95;
	knob_y = 15.0;//35.0;

	/// ADD PARAMS
	addParam(
	/**/ createParamCentered<KnobBig>(mm2px(Vec(20.32 / 2.0, knob_y)),
	/**/ module,
	/**/ TreeSeed::PARAM_THRESHOLD));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(20.32 / 2.0, knob_y + 10.5)),
	/**/ module,
	/**/ TreeSeed::PARAM_THRESHOLD_MOD));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(20.32 / 2.0, knob_y + 18.0)),
	/**/ module,
	/**/ TreeSeed::INPUT_THRESHOLD));

	for (i = 0; i < 5; ++i) {
		if (i == 0)
			output_id = TreeSeed::OUTPUT_GATE;
		else if (i == 1)
			output_id = TreeSeed::OUTPUT_PITCH;
		else
			output_id = TreeSeed::OUTPUT_CV + (i - 2);
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(out_x, out_y + out_step * i)),
		/**/ module,
		/**/ output_id));
	}

}

void TreeSeedWidget::appendContextMenu(Menu *menu) {
	Param		*param;

	menu->addChild(new MenuSeparator);

	param = &(this->module->params[TreeSeed::PARAM_GATE]);
	menu->addChild(rack::createSubmenuItem("Gate", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Trigger", "",
				[=]() { return param->getValue() == TREE_SEED_GATE_TRIGGER; },
				[=]() { param->setValue(TREE_SEED_GATE_TRIGGER); }
			));
			menu->addChild(new MenuCheckItem("Gate", "",
				[=]() { return param->getValue() == TREE_SEED_GATE_GATE; },
				[=]() { param->setValue(TREE_SEED_GATE_GATE); }
			));
		}
	));

	param = &(this->module->params[TreeSeed::PARAM_RANGE]);
	menu->addChild(rack::createSubmenuItem("Range", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("+/-10V", "",
				[=]() { return param->getValue() == TREE_SEED_RANGE_BI_10; },
				[=]() { param->setValue(TREE_SEED_RANGE_BI_10); }
			));
			menu->addChild(new MenuCheckItem("+/-5V", "",
				[=]() { return param->getValue() == TREE_SEED_RANGE_BI_5; },
				[=]() { param->setValue(TREE_SEED_RANGE_BI_5); }
			));
			menu->addChild(new MenuCheckItem("+/-3V", "",
				[=]() { return param->getValue() == TREE_SEED_RANGE_BI_3; },
				[=]() { param->setValue(TREE_SEED_RANGE_BI_3); }
			));
			menu->addChild(new MenuCheckItem("+/-2V", "",
				[=]() { return param->getValue() == TREE_SEED_RANGE_BI_2; },
				[=]() { param->setValue(TREE_SEED_RANGE_BI_2); }
			));
			menu->addChild(new MenuCheckItem("+/-1V", "",
				[=]() { return param->getValue() == TREE_SEED_RANGE_BI_1; },
				[=]() { param->setValue(TREE_SEED_RANGE_BI_1); }
			));
			menu->addChild(new MenuCheckItem("+10V", "",
				[=]() { return param->getValue() == TREE_SEED_RANGE_UNI_10; },
				[=]() { param->setValue(TREE_SEED_RANGE_UNI_10); }
			));
			menu->addChild(new MenuCheckItem("+5V", "",
				[=]() { return param->getValue() == TREE_SEED_RANGE_UNI_5; },
				[=]() { param->setValue(TREE_SEED_RANGE_UNI_5); }
			));
			menu->addChild(new MenuCheckItem("+3V", "",
				[=]() { return param->getValue() == TREE_SEED_RANGE_UNI_3; },
				[=]() { param->setValue(TREE_SEED_RANGE_UNI_3); }
			));
			menu->addChild(new MenuCheckItem("+2V", "",
				[=]() { return param->getValue() == TREE_SEED_RANGE_UNI_2; },
				[=]() { param->setValue(TREE_SEED_RANGE_UNI_2); }
			));
			menu->addChild(new MenuCheckItem("+1V", "",
				[=]() { return param->getValue() == TREE_SEED_RANGE_UNI_1; },
				[=]() { param->setValue(TREE_SEED_RANGE_UNI_1); }
			));
		}
	));

	param = &(this->module->params[TreeSeed::PARAM_POLYPHONY]);
	menu->addChild(rack::createSubmenuItem("Polyphony", "",
		[=](Menu *menu) {
			int		i;
			for (i = 0; i < 16; ++i) {
				menu->addChild(new MenuCheckItem(rack::string::f("%d", i + 1), "",
					[=]() { return param->getValue() == i + 1; },
					[=]() { param->setValue(i + 1); }
				));
			}
		}
	));
}
