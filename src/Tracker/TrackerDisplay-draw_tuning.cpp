
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::draw_tuning(const DrawArgs &args, Rect rect) {
	Vec			p;
	float		cent;
	float		angle;
	float		length;
	float		center_x, center_y;
	float		x, y;
	int			i;

	p = rect.getTopLeft();
	center_x = rect.pos.x + rect.size.x * 0.5,
	center_y = rect.pos.y + rect.size.y * 0.5;

	nvgLineCap(args.vg, NVG_ROUND);

	/// DRAW DEFAULT LINES
	nvgStrokeColor(args.vg, colors[14]);
	nvgStrokeWidth(args.vg, 10);
	for (i = 0; i < 12; ++i) {
		/// COMPUTE LINE
		cent = i * 100.0;
		angle = (cent / 1200.0) * (2 * M_PI) - M_PI;
		x = center_x + sin(angle) * 155.0;
		y = center_y + cos(angle) * 155.0;
		/// DRAW LINE
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, center_x, center_y);
		nvgLineTo(args.vg, x, y);
		nvgStroke(args.vg);
	}
	nvgFillColor(args.vg, colors[15]);
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center_x, center_y, 150.0);
	nvgFill(args.vg);

	/// DRAW TUNING LINES
	nvgStrokeColor(args.vg, colors[4]);
	nvgStrokeWidth(args.vg, 2);
	for (i = 0; i < 12; ++i) {
		/// COMPUTE LINE
		cent = this->module->params[Tracker::PARAM_TUNING + i].getValue();
		length = 100.0 + abs(fmod(cent, 100.0) - 50.0) / 50.0 * 50.0;
		angle = (cent / 1200.0) * (2 * M_PI) - M_PI;
		x = center_x + sin(angle) * length;
		y = center_y + cos(angle) * length;
		/// DRAW LINE
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, center_x, center_y);
		nvgLineTo(args.vg, x, y);
		nvgStroke(args.vg);
	}
	nvgFillColor(args.vg, colors[14]);
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center_x, center_y, 20.0);
	nvgFill(args.vg);
}
