
#include "plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Tracker::Tracker() {
	int	i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configParam(PARAM_PLAY_SONG, 0.0f, 1.0f, 0.f, "Play song");
	configParam(PARAM_PLAY_PATTERN, 0.0f, 1.0f, 0.f, "Play pattern");
	configParam(PARAM_PLAY, 0.0f, 1.0f, 0.f, "Play");
	configParam(PARAM_STOP, 0.0f, 1.0f, 0.f, "Stop");

	configParam(PARAM_BPM, 30.0f, 300.0f, 120.f, "BPM");
	configParam(PARAM_SYNTH, 0.0f, 63.0f, 0.f, "Synth")->snapEnabled = true;
	configParam(PARAM_PATTERN, 0.0f, 255.0f, 0.f, "Pattern")->snapEnabled = true;

	configParam(PARAM_VIEW, 0.0f, 1.0f, 0.f, "View Velocity");
	configParam(PARAM_VIEW + 1, 0.0f, 1.0f, 0.f, "View Panning");
	configParam(PARAM_VIEW + 2, 0.0f, 1.0f, 0.f, "View Delay");
	configParam(PARAM_VIEW + 3, 0.0f, 1.0f, 0.f, "View Glide");
	configParam(PARAM_VIEW + 4, 0.0f, 1.0f, 0.f, "View Effects");

	for (i = 0; i < 8; ++i)
		configParam(PARAM_EDIT + i, 0.0f, 1.0f, 0.0f, "Select");
	configParam(PARAM_EDIT_SAVE, 0.0f, 1.0f, 0.0f, "Save");
	configParam(PARAM_EDIT_RESET, 0.0f, 1.0f, 0.0f, "Reset");

	configParam(PARAM_MODE + 0, 0.0f, 1.0f, 0.0f, "Mode pattern");
	configParam(PARAM_MODE + 1, 0.0f, 1.0f, 0.0f, "Mode timeline");
	configParam(PARAM_MODE + 2, 0.0f, 1.0f, 0.0f, "Mode parameters");

	configParam(PARAM_OCTAVE_UP, 0.0f, 1.0f, 0.0f, "Octave +");
	configParam(PARAM_OCTAVE_DOWN, 0.0f, 1.0f, 0.0f, "Octave -");

	configLight(LIGHT_FOCUS, "Focus");
	configLight(LIGHT_PLAY, "Play");

	configOutput(OUTPUT_CLOCK, "Clock");

	/// DEFINE GLOBAL COLOR
	colors[0] = nvgRGBA(0x1A, 0x1C, 0x2C, 0xFF);
	colors[1] = nvgRGBA(0x5D, 0x27, 0x5D, 0xFF);
	colors[2] = nvgRGBA(0xB1, 0x3E, 0x53, 0xFF);
	colors[3] = nvgRGBA(0xEF, 0x7D, 0x57, 0xFF);
	colors[4] = nvgRGBA(0xFF, 0xCD, 0x75, 0xFF);
	colors[5] = nvgRGBA(0xAF, 0xF0, 0x70, 0xFF);
	colors[6] = nvgRGBA(0x38, 0xB7, 0x64, 0xFF);
	colors[7] = nvgRGBA(0x25, 0x71, 0x79, 0xFF);
	colors[8] = nvgRGBA(0x29, 0x36, 0x6F, 0xFF);
	colors[9] = nvgRGBA(0x3B, 0x5D, 0xC9, 0xFF);
	colors[10] = nvgRGBA(0x41, 0xA6, 0xF6, 0xFF);
	colors[11] = nvgRGBA(0x73, 0xEF, 0xF7, 0xFF);
	colors[12] = nvgRGBA(0xF4, 0xF4, 0xF4, 0xFF);
	colors[13] = nvgRGBA(0x94, 0xB0, 0xC2, 0xFF);
	colors[14] = nvgRGBA(0x56, 0x6C, 0x86, 0xFF);
	colors[15] = nvgRGBA(0x33, 0x3C, 0x57, 0xFF);

	/// DEFINE GLOBAL KEYBOARD
	for (i = 0; i < 128; ++i)
		table_keyboard[i] = -1;
	//// OCTAVE LOW 1
	table_keyboard['Z'] = 0;		// C
		table_keyboard['S'] = 1;	// C#
	table_keyboard['X'] = 2;		// D
		table_keyboard['D'] = 3;	// D#
	table_keyboard['C'] = 4;		// E
	table_keyboard['V'] = 5;		// F
		table_keyboard['G'] = 6;	// F#
	table_keyboard['B'] = 7;		// G
		table_keyboard['H'] = 8;	// G#
	table_keyboard['N'] = 9;		// A
		table_keyboard['J'] = 10;	// A#
	table_keyboard['M'] = 11;		// B
	//// OCTAVE LOW 2
	table_keyboard[','] = 12;		// C
		table_keyboard['L'] = 13;	// C#
	table_keyboard['.'] = 14;		// D
		table_keyboard[';'] = 15;	// D#
	table_keyboard['/'] = 16;		// E
	//// OCTAVE HIGH 2
	table_keyboard['Q'] = 12;		// C
		table_keyboard['2'] = 13;	// C#
	table_keyboard['W'] = 14;		// D
		table_keyboard['3'] = 15;	// D#
	table_keyboard['E'] = 16;		// E
	table_keyboard['R'] = 17;		// F
		table_keyboard['5'] = 18;	// F#
	table_keyboard['T'] = 19;		// G
		table_keyboard['6'] = 20;	// G#
	table_keyboard['Y'] = 21;		// A
		table_keyboard['7'] = 22;	// A#
	table_keyboard['U'] = 23;		// B
	//// OCTAVE HIGH 3
	table_keyboard['I'] = 24;		// C
		table_keyboard['9'] = 25;	// C#
	table_keyboard['O'] = 26;		// D
		table_keyboard['0'] = 27;	// D#
	table_keyboard['P'] = 28;		// E
	table_keyboard['['] = 29;		// F
		table_keyboard['='] = 30;	// F#
	table_keyboard[']'] = 31;		// G


	clock_timer.reset();

	/// INIT PATTERN SOURCE
	PatternSource	*pattern;

	g_timeline.debug = 0;
	g_timeline.resize(16);
	g_timeline.timeline[0][0].mode = 1;
	g_timeline.timeline[0][0].pattern = 0;
	g_timeline.timeline[0][0].beat = 0;

	g_timeline.synths[0].init(0, 6);
	g_timeline.synths[1].init(1, 6);

	//g_timeline.patterns.resize(1);
	pattern = &(g_timeline.patterns[0]);
	//this->editor_pattern = pattern;
	//g_editor.pattern = pattern;

	pattern->resize(6, 1, 16, 4);
	pattern->notes[0]->effect_count = 2;

	/// FILL PATTERN SOURCE NOTES
	pattern->notes[0]->lines[0].mode = PATTERN_NOTE_NEW;
	pattern->notes[0]->lines[0].synth = 0;
	pattern->notes[0]->lines[0].pitch = 63;
	pattern->notes[0]->lines[0].velocity = 255;
	pattern->notes[0]->lines[8].mode = PATTERN_NOTE_NEW;
	pattern->notes[0]->lines[8].synth = 0;
	pattern->notes[0]->lines[8].pitch = 61;
	pattern->notes[0]->lines[8].velocity = 255;
	pattern->notes[0]->lines[16].mode = PATTERN_NOTE_NEW;
	pattern->notes[0]->lines[16].synth = 0;
	pattern->notes[0]->lines[16].pitch = 66;
	pattern->notes[0]->lines[16].velocity = 255;
	pattern->notes[0]->lines[24].mode = PATTERN_NOTE_NEW;
	pattern->notes[0]->lines[24].synth = 0;
	pattern->notes[0]->lines[24].pitch = 70;
	pattern->notes[0]->lines[24].velocity = 255;

	pattern->notes[0]->lines[32].mode = PATTERN_NOTE_NEW;
	pattern->notes[0]->lines[32].synth = 0;
	pattern->notes[0]->lines[32].pitch = 68;
	pattern->notes[0]->lines[32].velocity = 255;
	pattern->notes[0]->lines[40].mode = PATTERN_NOTE_NEW;
	pattern->notes[0]->lines[40].synth = 0;
	pattern->notes[0]->lines[40].pitch = 63;
	pattern->notes[0]->lines[40].velocity = 255;
	pattern->notes[0]->lines[48].mode = PATTERN_NOTE_NEW;
	pattern->notes[0]->lines[48].synth = 0;
	pattern->notes[0]->lines[48].pitch = 70;
	pattern->notes[0]->lines[48].velocity = 255;

	/// FILL PATTERN SOURCE CVS
	// TODO: use ArrayExt and set SYNTH & CHANNEL on row
	pattern->cvs[0]->synth = 1;
	pattern->cvs[0]->channel = 0;
	pattern->cvs[0]->lines[0].mode = PATTERN_CV_SET;
	pattern->cvs[0]->lines[0].value = 0;
	pattern->cvs[0]->lines[8].mode = PATTERN_CV_SET;
	pattern->cvs[0]->lines[8].value = 255;
	pattern->cvs[0]->lines[15].mode = PATTERN_CV_SET;
	pattern->cvs[0]->lines[15].delay = 255;
	pattern->cvs[0]->lines[15].value = 0;

	//pattern->notes[1][6].mode = -1;
	//pattern->notes[1][6].delay = 0;
	//for (i = 0; i < 8; ++i)
	//	pattern->notes[1][6].effects[i].type = PATTERN_EFFECT_NONE;
}

void Tracker::process(const ProcessArgs& args) {
	float	dt_sec, dt_beat;
	float	bpm;

	/// PROCESS EDITOR
	if (args.frame % 256 == 0)
		g_editor.process(this);

	/// COMPUTE CLOCK
	bpm = params[PARAM_BPM].getValue();
	dt_sec = args.sampleTime;
	dt_beat = (bpm * dt_sec) / 60.0f;
	clock_time_p = clock_timer.time;
	clock_timer.process(dt_beat);
	if (clock_timer.time >= 64.0f)
		clock_timer.time -= 64.0f;
	clock_time = clock_timer.time;
	/// OUTPUT CLOCK
	if (clock_time_p - (int)clock_time_p > clock_time - (int)clock_time)
		outputs[OUTPUT_CLOCK].setVoltage(10.0f);
	else
		outputs[OUTPUT_CLOCK].setVoltage(0.0f);

	/// PROCESS TIMELINE
	g_timeline.process(dt_sec, dt_beat);


	/// USE / MODIFY EXPANDERS
	//if (rightExpander.module) {
	//	rightExpander.module->params[0].setValue(0);
	//}
}

Model* modelTracker = createModel<Tracker, TrackerWidget>("Tracker");
