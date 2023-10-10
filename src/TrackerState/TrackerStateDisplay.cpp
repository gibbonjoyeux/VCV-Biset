
#include "TrackerState.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerStateDisplay::TrackerStateDisplay() {
}

void TrackerStateDisplay::draw(const DrawArgs &args) {
	Rect		rect;

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// RECT BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[0]);
	nvgRect(args.vg, RECT_ARGS(rect));
	nvgFill(args.vg);
}

void TrackerStateDisplay::drawLayer(const DrawArgs &args, int layer) {
	Synth		*synth;
	Rect		rect;
	float		pitch;
	float		scale;
	float		velo;
	float		pan;
	float		x, y;
	float		width;
	int			i, j;

	if (g_module == NULL || g_timeline == NULL)
		return;
	if (module == NULL || layer != 1)
		return;

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// DRAW VOICES
	/// FOR EACH SYNTH
	for (i = 0; i < g_timeline->synth_count; ++i) {
		synth = &(g_timeline->synths[i]);
		nvgFillColor(args.vg, colors_user[synth->color]);
		/// FOR EACH VOICE
		for (j = 0; j < synth->channel_count; ++j) {
			scale = this->module->scale[i * 16 + j] * 0.1;
			if (scale > 0.1) {
				pitch = synth->out_synth[j * 4 + 0];
				velo = synth->out_synth[j * 4 + 2];
				pan = synth->out_synth[j * 4 + 3];
				/// COMPUTE X AXIS
				x = rect.pos.x + rect.size.x / 2.0
				/**/ + ((pan / 5.0) * (rect.size.x / 2.0 - TSTATE_MAX));
				/// COMPUTE Y AXIS
				y = rect.pos.y + rect.size.y / 2.0
				/**/ - ((pitch / 5.0) * (rect.size.y / 2.0 - TSTATE_MAX));
				/// COMPUTE WIDTH
				width = (TSTATE_MIN + velo / 10.0 * TSTATE_RANGE) * scale;
				/// DRAW VOICE
				nvgBeginPath(args.vg);
				nvgCircle(args.vg, x, y, width);
				nvgFill(args.vg);
			}
		}
	}
	//LedDisplayTextField::drawLayer(args, layer);
}
