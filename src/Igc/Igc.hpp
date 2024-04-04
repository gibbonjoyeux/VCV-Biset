
#ifndef IGC_HPP
#define IGC_HPP

#include "../plugin.hpp"

#define IGC_BUFFER					480000	// 10s at 48000Hz sample rate
#define IGC_BUFFER_PRECISION		96		// Display precision
#define IGC_CLICK_SAFE_LENGTH		64		// Anti-click algorithm length
#define IGC_CLICK_DIST_THRESHOLD	24		// Playhead jump threshold before removing

#define IGC_MODE_PHASE				0
#define IGC_MODE_PHASE_BEAT			1
#define IGC_MODE_SPEED				2
#define IGC_MODE_GRAIN				3

#define IGC_MODE_OUT_STEREO				0	// Simple stereo
#define IGC_MODE_OUT_STEREO_POLY		1	// Simple stereo
#define IGC_MODE_OUT_STEREO_SPREAD		2	// Stereo spread (L R)
#define IGC_MODE_OUT_STEREO_SPREAD_PP	3	// Stereo spread ping pong (L R L)

#define IGC_ROUND_NONE				0
#define IGC_ROUND_KNOB				1
#define IGC_ROUND_KNOB_INPUT		2
#define IGC_ROUND_ALL				3

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct IgcPlayhead {

	//
	// phase: Position (phase) relative to writting playhead in delay buffer
	// phase_prev: Previously drawn phase (only for display, updated on display)
	// index: Position (index) absolute in full buffer
	// index_rel: Position (index) relative in full buffer (used in speed mode)
	// level: Playhead level
	// speed: Playhead speed (index)
	// grain_phase: Position (phase) relative to writing playhead in delay buffer
	// grain_time: Time past (seconds) in the current grain (used in grain mode)
	//
	// click_prev_l: last output before click (left channel)
	// click_prev_r: last output before click (right channel)
	// click_remaining: Remaining samples (index) before anti-click ends
	//

	float_4						phase;
	float_4						phase_prev;
	float_4						index;
	float_4						index_rel;
	float_4						level;
	float_4						speed;

	float_4						grain_phase;
	float_4						grain_time;
	float_4						grain_length;
	float_4						grain_speed;

	float_4						click_prev_l;
	float_4						click_prev_r;
	float_4						click_remaining;

	float_4						wavetable_index;

	//dsp::TSchmittTrigger<float>	trigger;
};

struct Igc : Module {
	enum	ParamIds {
		PARAM_DELAY_TIME,
		PARAM_DELAY_TIME_MOD,
		PARAM_DELAY_PPQN,
		PARAM_MODE,
		PARAM_MODE_OUTPUT,
		PARAM_PHASE,
		PARAM_PHASE_MOD_1,
		PARAM_PHASE_MOD_2,
		PARAM_SPEED,
		PARAM_SPEED_MOD_1,
		PARAM_SPEED_MOD_2,
		PARAM_SPEED_REV,
		PARAM_SPEED_ROUND,
		PARAM_SPEED_SLEW,
		PARAM_GRAIN,
		PARAM_GRAIN_MOD_1,
		PARAM_GRAIN_MOD_2,
		PARAM_LVL,
		PARAM_LVL_MOD_1,
		PARAM_LVL_MOD_2,
		PARAM_LVL_SHAPE_FORCE,
		PARAM_LVL_SHAPE_FORCE_MOD,
		PARAM_LVL_SHAPE_WAVE,
		PARAM_LVL_SHAPE_WAVE_MOD,
		PARAM_MIX_LVL_IN,
		PARAM_MIX_LVL_IN_MOD,
		PARAM_MIX_LVL_OUT,
		PARAM_MIX_LVL_OUT_MOD,
		PARAM_MIX,
		PARAM_MIX_MOD,
		PARAM_MODE_HD,
		PARAM_MODE_ANTICLICK,
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_L,
		INPUT_R,
		INPUT_DELAY_TIME,
		INPUT_DELAY_CLOCK,
		INPUT_PHASE_1,
		INPUT_PHASE_2,
		INPUT_SPEED_1,
		INPUT_SPEED_2,
		INPUT_SPEED_REV,
		INPUT_GRAIN_1,
		INPUT_GRAIN_2,
		INPUT_LVL_1,
		INPUT_LVL_2,
		INPUT_LVL_SHAPE_FORCE,
		INPUT_LVL_SHAPE_WAVE,
		INPUT_MIX_LVL_IN,
		INPUT_MIX_LVL_OUT,
		INPUT_MIX,
		INPUT_RESET,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_L,
		OUTPUT_R,
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_MODE_PHASE,
		LIGHT_MODE_PHASE_BEAT,
		LIGHT_MODE_SPEED,
		LIGHT_MODE_GRAIN,
		LIGHT_MODE_OUT_STEREO,
		LIGHT_MODE_OUT_STEREO_POLY,
		LIGHT_MODE_OUT_STEREO_SPREAD,
		LIGHT_MODE_OUT_STEREO_SPREAD_PP,
		LIGHT_ROUND_KNOB,
		LIGHT_ROUND_INPUT_1,
		LIGHT_ROUND_INPUT_2,
		LIGHT_COUNT
	};

	//
	// table_pitch: Lookup table storing pitch function (v/oct to ratio
	//              conversion). Stores from -5V to +5V with
	//              IGC_LOOKUP_RESOLUTION octave (volt) resolution
	//
	// trigger_clock: Main clock controling delay time
	// trigger_reset: abs_phase phase reset (position absolute mode)
	// clock_trigger_count: Number of clock trigger past for PPQN count
	// clock_sample_count: Time (samples) past since last clock trigger
	// delay_time: Current delay time (in sec) - Slowly ease into delay_time_aim
	// delay_time_aim: Aimed delay time (in sec)
	//
	// playheads: Polyphonic playheads
	// playhead_count: Number of polyphonic playheads
	// abs_phase: For position absolute mode, writing position (phase) in delay
	//            buffer. Made to simulate non-circular buffer (compensation)
	//
	// audio: Audio buffer to read and write
	// audio_index: Writing playhead index in audio buffer (circular)
	//
	// is_stereo: State of input (0: mono - 1: stereo)
	//

	dsp::TSchmittTrigger<float>	trigger_clock;
	dsp::TSchmittTrigger<float>	trigger_reset;
	int							clock_trigger_count;
	int							clock_samples_count;
	float						delay_time;
	float						delay_time_aim;

	IgcPlayhead					playheads[4];
	int							playhead_count;
	float						abs_phase;

	float						audio[2][IGC_BUFFER];
	int							audio_index;

	bool						is_stereo;

	Igc();

	void	process(const ProcessArgs& args) override;
};

struct IgcWidget : ModuleWidget {
	Igc							*module;

	IgcWidget(Igc* _module);
	void appendContextMenu(Menu *menu) override;
};

struct IgcIoWidget : widget::Widget {
	Igc							*module;
	widget::FramebufferWidget	*fb;
	widget::SvgWidget			*sw_in;
	widget::SvgWidget			*sw_out;
	int							mode_input;
	int							mode_output;
	int							i;

	IgcIoWidget();
	void step(void) override;
	//void set_mode(int mode);
};

struct IgcDisplay : LedDisplay {
	Igc							*module;
	ModuleWidget				*moduleWidget;

	IgcDisplay();
	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
};

#endif
