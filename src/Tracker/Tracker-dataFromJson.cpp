
#include "Tracker.hpp"

/*

TRACKER BINARY SAVE FORMAT:
- File version									u8
- File endian									u8
- File size										u32
- Patterns
	- Pattern count								u16
	- Patterns
		- Name length							u8
		- Name string							chars
		- Color									u8
		- Swing ratio							u8
		- Swing signature						u8
		- Beat count							u16
		- Note count							u8
		- CV count								u8
		- lpb (lines per beat)					u8
		- Note columns										x N
			- Column muted						u8	NOT HANDLE YET
			- Column general effects
				- Fx velocity					u8	NOT HANDLE YET
				- Fx panning					u8	NOT HANDLE YET
				- Fx delay						u8	NOT HANDLE YET
				- Fx chance						u8	NOT HANDLE YET
				- Fx chance mode				u8	NOT HANDLE YET
				- Fx octave						u8	NOT HANDLE YET
				- Fx octave mode				u8	NOT HANDLE YET
				- Fx pitch						u8	NOT HANDLE YET
			- Column effect count				u8
			- Column set lines count			u16
			- Lines (only set lines)
				- Line number					u16
				- Note mode						i8
				- ? Mode NEW
					- Note pitch				u8
					- Note velocity				u8
					- Note panning				u8
					- Note synth				u8
					- Note delay				u8
					- Note glide				u8
					- Note effects							x N
						- Effect type			u8
						- Effect value			u8
				- ? Mode STOP
					- Note delay				u8
				- ? Mode GLIDE
					- Note pitch				u8
					- Note velocity				u8
					- Note panning				u8
					- Note glide				u8
					- Note effects							x N
						- Effect type			u8
						- Effect value			u8
		- CV columns										x N
			- Column mode						u8
			- Column synth						u8
			- Column channel					u8
			- Column set lines count			u16
			- Lines (only set lines)
				- Line number					u16
				- CV mode						u8
				- CV value						u16
				- CV delay						u8
				- CV curve						u8
- Synths
	- Synth count								u8
	- Synths
		- Synth name length						u8
		- Synth name string						chars
		- Synth color							u8
		- Synth mode							u8
		- Synth channel count					u8
- Timeline
	- Instance count							u16
	- Instances
		- Instance row							u8
		- Instance beat							u16
		- Pattern id							u16
		- Instance beat start					u16
		- Instance beat length					u16
		- Muted									u8
- Matrix
	- Cell count								u16
	- Cells
		- Cell column							u8
		- Cell line								u8
		- Cell pattern id						u16
		- Cell mode								u8
		- ? Mode LOOP
		- ? Mode NEXT
		- ? Mode NEXT CIRCULAR
		- ? Mode PREV
		- ? Mode PREV CIRCULAR
		- ? Mode RAND
		- ? Mode XRAND
		- ? Mode GOTO
			- Cell line							u8
		- ? Mode RAND AFTER
			- Cell line							u8
		- ? Mode RAND BEFORE
			- Cell line							u8

*/

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void Tracker::dataFromJson(json_t *root) {
	json_t		*obj;

	if (g_module != this)
		return;

	/// [1] RECOVER MIDI (DRIVER + DEVICE + CHANNEL)
	obj = json_object_get(root, "midi");
	if (obj)
		this->midi_input.fromJson(obj);
	/// RECOVER EDITOR (JUMP + OCTAVE)
	obj = json_object_get(root, "editor_jump");
	if (obj)
		g_editor->pattern_jump = json_integer_value(obj);
	obj = json_object_get(root, "editor_octave");
	if (obj)
		g_editor->pattern_octave = json_integer_value(obj);

	/// [2] WAIT FOR THREAD FLAG
	while (g_timeline->thread_flag.test_and_set()) {}

	/// [3] LOAD TRACK
	//g_timeline->clear();
	// ...

	/// [4] CLEAR THREAD FLAG
	g_timeline->thread_flag.clear();
}
