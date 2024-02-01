
#include "River.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

RiverDisplay::RiverDisplay() {
	this->font_path = std::string(asset::plugin(pluginInstance,
	/**/ "res/FT88-Regular.ttf"));
}

void RiverDisplay::draw(const DrawArgs &args) {
}

void RiverDisplay::drawLayer(const DrawArgs &args, int layer) {
	Rect								rect;
	std::shared_ptr<Font>				font;
	std::vector<RiverFile>::iterator	it, it_end;
	int									i;

	if (this->module == NULL || layer != 1)
		return;

	rect = box.zeroPos();

	font = APP->window->loadFont(this->font_path);
	if (font == NULL)
		return;

	nvgScissor(args.vg, RECT_ARGS(box));
	/// DRAW SELECTOR
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[2]);
	nvgRect(args.vg,
	/**/ box.pos.x, box.pos.y + 12.0 * this->module->dir_selected,
	/**/ box.size.x, 12.0);
	nvgFill(args.vg);
	/// DRAW FILES
	it = this->module->dir.begin();
	it_end = this->module->dir.end();
	i = 0;
	while (it != it_end) {
		nvgFontSize(args.vg, 12.0);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
		if (it->type == RIVER_DIR)
			nvgFillColor(args.vg, colors[3]);
		else if (it->type == RIVER_WAV)
			nvgFillColor(args.vg, colors[4]);
		else
			nvgFillColor(args.vg, colors[13]);
		nvgText(args.vg,
		/**/ box.pos.x,
		/**/ box.pos.y + 12.0 * i,
		/**/ it->name.c_str(), NULL);
		/// LOOP
		++it;
		++i;
	}
	nvgResetScissor(args.vg);

	nvgText(args.vg,
	/**/ box.pos.x,
	/**/ box.pos.y + 12.0 * 18,
	/**/ this->module->dir_path.c_str(), NULL);
}
