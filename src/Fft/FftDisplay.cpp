
#include "Fft.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

FftDisplay::FftDisplay(void) {
}

void FftDisplay::drawLayer(const DrawArgs& args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	int						k;
	int						x, h;

	LedDisplay::draw(args);

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[0]);
	nvgRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0);
	nvgFill(args.vg);

	if (this->module == NULL)
		return;

	nvgScissor(args.vg, RECT_ARGS(rect));

	nvgFillColor(args.vg, colors[14]);
	for (x = 0; x < rect.size.x; x += 2) {
		k = ((float)x / (float)rect.size.x) * (float)FFT_BUFFER;

		h = sqrt(this->module->buffer_out[k]
		/**/ * this->module->buffer_out[k]
		/**/ + this->module->buffer_out[k + 1]
		/**/ * this->module->buffer_out[k + 1]) * 0.5;
		if (h == 0)
			continue;

		nvgBeginPath(args.vg);
		nvgRect(args.vg,
		/**/ rect.pos.x + x, rect.pos.y + rect.size.y,
		/**/ 2, -h);
		nvgFill(args.vg);
	}

	nvgResetScissor(args.vg);
}
