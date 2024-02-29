
#include "Tree.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TreeWidget::TreeWidget(Tree* _module) {
	TreeDisplay	*display;
	float		seq_x, seq_y;
	float		seq_step;
	float		tree_x, tree_y;
	float		tree_step;
	float		out_x, out_y;
	float		out_step;
	float		in_x, in_y;
	float		in_step;
	int			i;

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tree.svg")));

	display = createWidget<TreeDisplay>(mm2px(Vec(0.0, 0.0)));
	display->box.size = mm2px(Vec(55.88, 60.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);

	seq_x = 55.88 * 0.5 - 5.0;
	seq_y = 71.0;
	seq_step = 12.0;

	in_x = 55.88 * 0.5 - 5.0;
	in_y = 98.0;
	in_step = 12.0;

	out_x = 55.88 - 8.045;
	out_y = seq_y - 0.5;
	out_step = 8.95;

	tree_x = 55.88 * 0.5 - 5.0;
	tree_y = 108.0;
	tree_step = 8.5;

	/// ADD PARAMS
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(tree_x - tree_step * 1.5, tree_y)),
	/**/ module,
	/**/ Tree::PARAM_BRANCH_ANGLE_VARIATION));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(tree_x - tree_step * 0.5, tree_y)),
	/**/ module,
	/**/ Tree::PARAM_BRANCH_ANGLE_DIVISION));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(tree_x + tree_step * 0.5, tree_y)),
	/**/ module,
	/**/ Tree::PARAM_BRANCH_DIVISION));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(tree_x + tree_step * 1.5, tree_y)),
	/**/ module,
	/**/ Tree::PARAM_BRANCH_ANGLE_SUN_FORCE));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(seq_x - seq_step, seq_y)),
	/**/ module,
	/**/ Tree::PARAM_SEQ_LENGTH));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(seq_x - seq_step, seq_y + 8.0)),
	/**/ module,
	/**/ Tree::PARAM_SEQ_LENGTH_MOD));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(seq_x - seq_step, seq_y + 14.5)),
	/**/ module,
	/**/ Tree::INPUT_SEQ_LENGTH));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(seq_x, seq_y)),
	/**/ module,
	/**/ Tree::PARAM_SEQ_OFFSET));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(seq_x, seq_y + 8.0)),
	/**/ module,
	/**/ Tree::PARAM_SEQ_OFFSET_MOD));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(seq_x, seq_y + 14.5)),
	/**/ module,
	/**/ Tree::INPUT_SEQ_OFFSET));

	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(seq_x + seq_step, seq_y)),
	/**/ module,
	/**/ Tree::PARAM_SEQ_WIND_INFLUENCE));
	addParam(
	/**/ createParamCentered<KnobSmall>(mm2px(Vec(seq_x + seq_step, seq_y + 8.0)),
	/**/ module,
	/**/ Tree::PARAM_SEQ_WIND_INFLUENCE_MOD));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(seq_x + seq_step, seq_y + 14.5)),
	/**/ module,
	/**/ Tree::INPUT_SEQ_WIND_INFLUENCE));


	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(in_x - in_step, in_y)),
	/**/ module,
	/**/ Tree::INPUT_TREE_RESET));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(in_x, in_y)),
	/**/ module,
	/**/ Tree::INPUT_SEQ_RESET));
	addInput(
	/**/ createInputCentered<Outlet>(mm2px(Vec(in_x + in_step, in_y)),
	/**/ module,
	/**/ Tree::INPUT_SEQ_CLOCK));

	for (i = 0; i < 5; ++i) {
		addOutput(
		/**/ createOutputCentered<Outlet>(mm2px(Vec(out_x, out_y + out_step * i)),
		/**/ module,
		/**/ Tree::OUTPUT + i));
	}

}

void TreeWidget::appendContextMenu(Menu *menu) {
	MenuSlider	*slider;

	menu->addChild(new MenuSeparator);

	slider = new MenuSlider(
	/**/ this->module->paramQuantities[Tree::PARAM_SEQ_MUTATE_CHANCE]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);
}

