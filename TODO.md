
# BUGS

- [ ] On saved and then modified file, if re-open saved filed without saving
		last changes, theses changes seems to stay with weird behaviour.
- [ ] ! ! ! Sometimes, on multi channels synth, gate / trigger got issue and
		miss lot of notes.
- [x] Reduced frame rate on main process cause effects to be ultra slow
		-> Reduce main computation but keep SynthVoice
- [x] Synth gate never comming back to 0
- [x] Crash: on start when lot of note are added:
		-> Issue from onSave()
		-> File size starts to be wrong after few lines added.
		-> Seems that the file is completely offseted by one byte.
		-> One byte (0x0d) is added at 3nd byte position after 1st byte of
		   u32 file size.
		-> Buffer stays good even after write() and close() so issue does not
		   come from buffer filling.
- [x] Crash: Cursor on CV column, change CV column count to 0.
- [ ] Crash: On start / On add with template (due to template ?)
- [ ] Crash: once on pattern change (via knob)
	-> Might be due to cursor position that gets out of bound on pattern change
	to a smaller pattern (both line and column)
	-> Check on empty pattern !

# TODO

- [ ] MODULE Tracker
	- [ ] BACKEND
		- [ ] PLAY
			- [ ] Play song
			- [ ] Play pattern
		- [x] TIMELINE
			- [x] Pattern new		(run new pattern)
			- [x] Pattern keep		(keep active pattern)
			- [x] Pattern stop		(stop active pattern)
		- [ ] PATTERN
			- [ ] NOTE
				- [x] Mode gate
				- [x] Mode trigger
				- [x] Mode drum
				- [x] Note new		(run new note)
				- [x] Note keep		(keep active note)
				- [ ] Note change	(change active note vel or pan)
				- [x] Note stop		(stop active note)
				- [x] Note glide	(glide active note to new pitch)
				- [x] Pitch temperament scale
				- [x] Pitch base offset (from 440hz)
			- [ ] CV
				- [x] Mode CV
				- [ ] Mode Trigger
				- [ ] Mode Gate
				- [x] Mode BPM
				- [ ] CV set		(set cv point)
					- [x] CV value
					- [ ] CV curve
				- [x] CV keep		(continue interpolation)
	- [ ] FRONTEND
		- [ ] Screen mode (Tracker + Timeline + Parameters)
		- [ ] Screen Tracker
			- [x] View modes (amp + pan + ...)
			- [x] Layer tracker (text only)
			- [ ] Layer visual (delay + curve representation)
			- [x] Layer user (cursor + beat cursor)
		- [x] Screen Timeline
			- [x] Layer tracker (text only)
			- [ ] Layer visual
			- [x] Layer user (cursor + beat cursor)
		- [ ] Screen Parameters -> Can be accessed via context menu (right click)
			- [x] Frame rate (audio, audio / 2, audio / 4, ...)
			- [x] Base pitch (default: 440)
			- [x] Temperaments (can be used as scale)
				- [x] Temperament manual
				- [x] Temperament preset (equal, just, ...)
				- [ ] Temperament scale preset (major, minor, modes, ...)
		- [ ] Screen Edition
			- [x] Song
			- [x] Synth
			- [x] Pattern
			- [x] Column
				- [x] Note
				- [x] CV
					- [x] Mode
					- [x] Synth
					- [x] Index
	- [x] SAVE
		- [x] Save data
		- [x] Load data
		- [x] Load template on empty
	- [ ] HISTORY (UNDO / REDO)
		- [ ] ActionNote			(replace note cell by another)
		- [ ] ActionCV				(replace cv cell by another)
		- [ ] ActionSongResize		(resize song)
		- [ ] ActionPatternResize	(resize pattern length or notes or cv. Can contain multiple removed ActionNote & ActionCV)
		- [ ] ActionPatternColumn	(change pattern column)
		- [ ] ActionSynth			(change synth)
		- [ ] ActionSynthOut		(change synth output module)
- [ ] MODULE TrackerOut
	- [ ] FRONTEND
	- [ ] BACKEND
		- [ ] CV
			- [x] CV mode range (min / max sliders)
			- [ ] CV mode param map
- [ ] MODULE TrackerDrumOut
- [ ] MODULE TrackerClock
- [ ] MODULE TrackerState



- [x] Tracker play pattern
- [x] Tracker play pattern (with specified starting beat)
- [x] Pattern play notes
- [x] Pattern play notes delay
- [x] Pattern play velocity
- [ ] Pattern play effects
	- [x] Effect vibrato		Vxy (x: freq y: amp)
	- [x] Effect tremolo		Txy (x: freq y: amp)
	- [ ] Effect ratchet		Rxx (x: count)
	- [ ] Effect octave offset	Oxy (x: range y: chance)
- [x] Pattern play notes glide
- [x] Pattern play CV interpolation
- [x] Pattern play CV interpolation with delay
- [ ] Pattern play CV interpolation with curves

- [x] Chance as effect (Cxx + cxx)
- [x] Handle delay on note stop / new for running note
- [x] Define synth row order (PITCH-OCT VELO SYNTH DELAY GLIDE FX FX ...)
- [x] Line / Beat number on left
- [x] Scroll Y
- [x] Scroll X
- [x] Display modes (show VELO PAN DELAY GLIDE FX)
- [x] Pattern selecting knob
- [x] Synth selecting knob (also used in pattern editing)
- [x] Editing screen
	- [x] Song editing knob
	- [x] Synth editing knob(s)
	- [x] Pattern editing knobs
	- [x] Pattern Column editing knobs
	- [x] Update button

# Effects

[x] Axx	Amplitude random		(xx: noise amplitude)
		Multiply base amplitude by noise (noise can only make it lower)
[x] Pxx	Pan random				(xx: pan amplitude)
		Add noise to base panning (bipolar noise)
[x] Dxx Delay random			(xx: delay amplitude)
[x] Oxy	Octave random			(x: mode y: amplitude)
		x = 0 : -+ offset
		x = 1 : + offset
		x = 2 : - offset
[x] Mxy	Note random semitone	(x: semitone tone 1st offset y: 2nd offset)
		Peek random note between [base note], [base + x] and [base + y]
		x & y being semitone offset
		Can be cumulable
[ ] Sxy	Note random scale		(x: scale index y: root note)
		Peek random note in specified scale with root note
		Major, Dorian, Phrygian, Lydian, Mixolydian, Aeolian, Ionian & Locrian
[x] Vxy	Vibrato					(x: speed y: amplitude)
[x] Txy	Tremolo					(x: speed y: amplitude)
[ ] Fxx	Fade out				(x: speed)
[ ] fxx	Fade in					(x: speed)
[x] Cxx	Chance keep				(xx: chance)
		Keep previous running note on if chance does not occur
[ ] cxx	Chance stop				(xx: chance)
		Stop previous running note even if chance does not occur
[ ] Rxx	Ratchet / Retrigger		(xx: retrigger interval) ? (x: amp fade y: interval)
		x<5 : decrease | x>5 : increase | x=5 : static
		y represents the interval between retriggers
