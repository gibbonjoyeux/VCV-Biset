
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
	//Rect		rect;

	///// GET CANVAS FORMAT
	//rect = box.zeroPos();
	///// RECT BACKGROUND
	//nvgBeginPath(args.vg);
	//nvgFillColor(args.vg, colors[0]);
	//nvgRect(args.vg, RECT_ARGS(rect));
	//nvgFill(args.vg);
}

void TreeDisplay::drawLayer(const DrawArgs &args, int layer) {
	TreeBranch	*branch;
	Rect		rect;
	Vec			center;
	int			i;

	if (this->module == NULL || layer != 1)
		return;

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	center.x = rect.pos.x + rect.size.x * 0.5;
	center.y = rect.pos.y + rect.size.y;

	//char	str[128];
	//sprintf(str, "count [%d]", this->module->branch_count);
	//nvgFillColor(args.vg, colors[3]);
	//nvgText(args.vg, rect.pos.x + 10, rect.pos.y + 10.0, str, NULL);

	/// SET COLOR
	nvgStrokeColor(args.vg, {0xec / 255.0, 0xae / 255.0, 0x52 / 255.0, 1.0});
	/// SET TRANSFORMATION
	//nvgScissor(args.vg, RECT_ARGS(rect));
	nvgTranslate(args.vg, center.x, center.y);
	nvgScale(args.vg, 1.0, 1.0);
	nvgRotate(args.vg, -M_PI / 2.0);
	nvgLineCap(args.vg, 1);
	/// DRAW BRANCHES
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
	//nvgResetScissor(args.vg);
}
