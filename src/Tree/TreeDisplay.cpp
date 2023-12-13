
#include "Tree.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TreeDisplay::TreeDisplay() {
}

void TreeDisplay::draw(const DrawArgs &args) {
}

void TreeDisplay::drawLayer(const DrawArgs &args, int layer) {
	Module		*exp_left;
	Module		*exp_right;
	Rect		zone;
	TreeBranch	*branch;
	Rect		rect;
	Vec			center;
	int			i;

	if (this->module == NULL || layer != 1)
		return;

	/// [1] GET EXPANDERS
	exp_left = this->module->leftExpander.module;
	exp_right = this->module->rightExpander.module;

	/// [2] COMPUTE CANVAS FORMAT
	rect = box.zeroPos();
	center.x = rect.pos.x + rect.size.x * 0.5;
	center.y = rect.pos.y + rect.size.y;
	zone = rect;
	if (exp_left) {
		if (exp_right) {
			zone.pos.x -= rect.size.x;
			zone.size.x += rect.size.x * 2.0;
		} else {
			zone.pos.x -= rect.size.x;
			zone.size.x += rect.size.x;
		}
	} else if (exp_right) {
		zone.size.x += rect.size.x;
	}

	/// [3] DRAW TREE
	//// SET COLOR
	nvgStrokeColor(args.vg, {0xec / 255.0, 0xae / 255.0, 0x52 / 255.0, 1.0});
	//// SET TRANSFORMATION
	nvgScissor(args.vg, RECT_ARGS(zone));
	nvgTranslate(args.vg, center.x, center.y);
	nvgScale(args.vg, 1.0, 1.0);
	nvgRotate(args.vg, -M_PI / 2.0);
	nvgLineCap(args.vg, 1);
	//// DRAW BRANCHES
	for (i = 0; i < this->module->branch_count; ++i) {
		branch = &(this->module->branches[i]);
		nvgStrokeWidth(args.vg, branch->width * 0.2);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, branch->wpos_root.x, branch->wpos_root.y);
		nvgLineTo(args.vg, branch->wpos_tail.x, branch->wpos_tail.y);
		nvgStroke(args.vg);
		nvgClosePath(args.vg);
	}
	nvgResetTransform(args.vg);
	nvgResetScissor(args.vg);
}
