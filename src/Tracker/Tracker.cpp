
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Tracker::Tracker() {
	int	i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configButton(PARAM_PLAY_SONG, "Play song");
	configButton(PARAM_PLAY_PATTERN, "Play pattern");
	configButton(PARAM_PLAY, "Play");
	configButton(PARAM_STOP, "Stop");

	configParam(PARAM_BPM, 30.0f, 300.0f, 120.f, "BPM");
	configParam(PARAM_SYNTH, 0.0f, 63.0f, 0.f, "Synth")->snapEnabled = true;
	configParam(PARAM_PATTERN, 0.0f, 255.0f, 0.f, "Pattern")->snapEnabled = true;

	configSwitch(PARAM_VIEW, 0, 1, 0, "View Velocity");
	configSwitch(PARAM_VIEW + 1, 0, 1, 0, "View Panning");
	configSwitch(PARAM_VIEW + 2, 0, 1, 0, "View Delay");
	configSwitch(PARAM_VIEW + 3, 0, 1, 0, "View Glide");
	configSwitch(PARAM_VIEW + 4, 0, 1, 0, "View Effects");

	for (i = 0; i < 9; ++i)
		configParam(PARAM_EDIT + i, 0.0f, 1.0f, 0.0f, "Select")->snapEnabled = true;
	configParam(PARAM_EDIT + 0, 1.0f, 9999.0f, 0.0f, "Song length")->snapEnabled = true;
	configParam(PARAM_EDIT + 1, 1.0f, 16.0f, 0.0f, "Synth channels")->snapEnabled = true;
	configParam(PARAM_EDIT + 2, 1.0f, 999.0f, 0.0f, "Pattern length")->snapEnabled = true;
	configParam(PARAM_EDIT + 3, 1.0f, 32.0f, 0.0f, "Pattern lpb")->snapEnabled = true;
	configParam(PARAM_EDIT + 4, 0.0f, 32.0f, 0.0f, "Pattern notes")->snapEnabled = true;
	configParam(PARAM_EDIT + 5, 0.0f, 32.0f, 0.0f, "Pattern cv")->snapEnabled = true;
	configButton(PARAM_EDIT_SAVE, "Save");

	configButton(PARAM_MODE + 0, "Mode pattern");
	configButton(PARAM_MODE + 1, "Mode timeline");
	configButton(PARAM_MODE + 2, "Mode parameters");

	configButton(PARAM_OCTAVE_UP, "Octave +");
	configButton(PARAM_OCTAVE_DOWN, "Octave -");
	configButton(PARAM_JUMP_UP, "Jump +");
	configButton(PARAM_JUMP_DOWN, "Jump -");

	configLight(LIGHT_FOCUS, "Focus");
	configLight(LIGHT_PLAY, "Play");

	configOutput(OUTPUT_CLOCK, "Clock");

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

	/// SET ACTIVE SYNTH & PATTERN
	g_editor.module = this;
}

void Tracker::process(const ProcessArgs& args) {
	float	dt_sec, dt_beat;
	float	bpm;

	/// PROCESS EDITOR
	g_editor.process(args.frame);

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
	g_timeline.process(args.frame, dt_sec, dt_beat);


	/// USE / MODIFY EXPANDERS
	//if (rightExpander.module) {
	//	rightExpander.module->params[0].setValue(0);
	//}
}

Model* modelTracker = createModel<Tracker, TrackerWidget>("Tracker");
