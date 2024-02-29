
#include "TreeExp.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TreeExpWidget::TreeExpWidget(TreeExp* _module) {
	float		out_x, out_y;
	float		out_step;
	float		knob_y;
	int			output_id;
	int			i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/TreeExp.svg")));

	out_x = 20.32 / 2.0;
	out_y = 71.0 - 0.5;
	out_step = 8.95;
	knob_y = 15.0;//35.0;

	/// ADD PARAMS
	addParam(
	/**/ createParamCentered<KnobBig>(mm2px(Vec(20.32 / 2.0, knob_y)),
	/**/ module,
	/**/ TreeExp::PARAM_THRESHOLD));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(20.32 / 2.0, knob_y + 10.5)),
	/**/ module,
	/**/ TreeExp::PARAM_THRESHOLD_MOD));

	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(20.32 / 2.0, knob_y + 18.0)),
	/**/ module,
	/**/ TreeExp::INPUT_THRESHOLD));

	for (i = 0; i < 5; ++i) {
		if (i == 0)
			output_id = TreeExp::OUTPUT_GATE;
		else if (i == 1)
			output_id = TreeExp::OUTPUT_PITCH;
		else
			output_id = TreeExp::OUTPUT_CV + (i - 2);
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(out_x, out_y + out_step * i)),
		/**/ module,
		/**/ output_id));
	}

}

void TreeExpWidget::appendContextMenu(Menu *menu) {
	Param		*param;

	menu->addChild(new MenuSeparator);

	param = &(this->module->params[TreeExp::PARAM_GATE_MODE]);
	menu->addChild(rack::createSubmenuItem("Gate", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Trigger", "",
				[=]() { return param->getValue() == TREE_EXP_GATE_TRIGGER; },
				[=]() { param->setValue(TREE_EXP_GATE_TRIGGER); }
			));
			menu->addChild(new MenuCheckItem("Gate", "",
				[=]() { return param->getValue() == TREE_EXP_GATE_GATE; },
				[=]() { param->setValue(TREE_EXP_GATE_GATE); }
			));
		}
	));

	param = &(this->module->params[TreeExp::PARAM_RANGE]);
	menu->addChild(rack::createSubmenuItem("Range", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("+/-10V", "",
				[=]() { return param->getValue() == TREE_EXP_RANGE_BI_10; },
				[=]() { param->setValue(TREE_EXP_RANGE_BI_10); }
			));
			menu->addChild(new MenuCheckItem("+/-5V", "",
				[=]() { return param->getValue() == TREE_EXP_RANGE_BI_5; },
				[=]() { param->setValue(TREE_EXP_RANGE_BI_5); }
			));
			menu->addChild(new MenuCheckItem("+/-3V", "",
				[=]() { return param->getValue() == TREE_EXP_RANGE_BI_3; },
				[=]() { param->setValue(TREE_EXP_RANGE_BI_3); }
			));
			menu->addChild(new MenuCheckItem("+/-2V", "",
				[=]() { return param->getValue() == TREE_EXP_RANGE_BI_2; },
				[=]() { param->setValue(TREE_EXP_RANGE_BI_2); }
			));
			menu->addChild(new MenuCheckItem("+/-1V", "",
				[=]() { return param->getValue() == TREE_EXP_RANGE_BI_1; },
				[=]() { param->setValue(TREE_EXP_RANGE_BI_1); }
			));
			menu->addChild(new MenuCheckItem("+10V", "",
				[=]() { return param->getValue() == TREE_EXP_RANGE_UNI_10; },
				[=]() { param->setValue(TREE_EXP_RANGE_UNI_10); }
			));
			menu->addChild(new MenuCheckItem("+5V", "",
				[=]() { return param->getValue() == TREE_EXP_RANGE_UNI_5; },
				[=]() { param->setValue(TREE_EXP_RANGE_UNI_5); }
			));
			menu->addChild(new MenuCheckItem("+3V", "",
				[=]() { return param->getValue() == TREE_EXP_RANGE_UNI_3; },
				[=]() { param->setValue(TREE_EXP_RANGE_UNI_3); }
			));
			menu->addChild(new MenuCheckItem("+2V", "",
				[=]() { return param->getValue() == TREE_EXP_RANGE_UNI_2; },
				[=]() { param->setValue(TREE_EXP_RANGE_UNI_2); }
			));
			menu->addChild(new MenuCheckItem("+1V", "",
				[=]() { return param->getValue() == TREE_EXP_RANGE_UNI_1; },
				[=]() { param->setValue(TREE_EXP_RANGE_UNI_1); }
			));
		}
	));
}
