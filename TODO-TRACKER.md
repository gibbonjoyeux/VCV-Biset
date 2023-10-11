
# BETA TESTERS

@d’Composer
@CuteFox
@Sparvnätter

# CONSTRUCTION / DESTRUCTION ORDER
Construction: [Module - dataFromJson - onAdd] - [Widget]
Destruction: [dataToJson], [Widget, onRemove, Module]

# BUGS

- ! ! ! Crash: Crash on pattern when play whole song

- [ ] Pattern instances can have a negative `beat_start` but should not

- [ ] Check if `thread_flag` has a big CPU impact

- [x] Drum synth should always have 12 "channels" (on process, the synth
		should process the 12 channels).
- [ ] If clone Tracker module accidentaly, the last module takes previous one
		place in `g_module`. If the new one is removed, issue.
- [x] On saved and then modified file, if re-open saved filed without saving
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

# THINK ABOUT

- ! ! ! Tilde instead of Delete for stop note
- ! ! ! Check for 'TODO:' in files

- What happens to overriden notes / voices when synth is used with many notes ?
- Live play
	- Light notifying caps lock ?
- Tuning
	- Multiple tuning - switch between them (via CV ?)
	- Tuning view

# TODO

- [ ] MODULE Tracker
	- [ ] BACKEND
		- [ ] PLAY
			- [x] Play song
			- [x] Play pattern solo
			- [x] Play pattern
			- [ ] Play matrix
		- [x] TIMELINE
		- [ ] PATTERN
			- [ ] SWING
			- [x] NOTE
				- [x] Mode gate
				- [x] Mode trigger
				- [x] Mode drum
				- [x] Note new		(run new note)
				- [x] Note keep		(keep active note)
				- [x] Note stop		(stop active note)
				- [x] Note glide	(glide note, vel, pan)
					- [x] Glide pitch
					- [x] Glide velocity + panning
					- [ ] Glide effects (only microtonal tuning for MPE)
				- [x] Pitch temperament scale
				- [x] Pitch base offset (from 440hz)
			- [ ] CV
				- [x] Mode CV
				- [x] Mode Gate
				- [x] Mode BPM
				- [ ] Mode tuning ?
				- [ ] CV set		(set cv point)
					- [x] CV value
					- [ ] CV curve
				- [x] CV keep		(continue interpolation)
			- [ ] FX Column (effects active on entire column, from context menu)
		- [ ] LIVE
			- [x] Play live (with caps lock, write in any case)
			- [ ] Record live (with play and caps lock on)
				- [ ] Move cursor (if caps lock)
				- [ ] Record notes (on available columns)
				- [ ] Record delays
				- [ ] Record MPE (with effects)
			- [x] External midi keyboard
				- [x] Select keyboard (from context menu)
				- [x] Play keyboard
	- [ ] FRONTEND
		- [x] Buttons
			- [x] Play
				- [x] Play song
				- [x] Play pattern
				- [x] Pause
				- [x] Stop
			- [x] View
				- [x] View pattern
				- [x] View timeline
				- [x] View matrix
				- [x] View tuning
			- [x] Pattern view modes
				- [x] View velocity
				- [x] View panning
				- [x] View delay
				- [x] View glide
				- [x] View effects
			- [x] Pattern values
				- [x] Change octave
				- [x] Change line jump
		- [ ] Display
			- [x] Screen mode (Tracker + Timeline + Matrix + Tuning)
			- [ ] Screen Tracker
				- [x] View modes (amp + pan + ...)
				- [x] Layer tracker (text only)
				- [ ] Layer visual (CV)
					- [x] Mode CV
					- [x] Mode BPM
					- [x] Mode Gate
					- [x] Interpolation
					- [x] Delay
					- [ ] Curve
				- [x] Layer user (cursor + beat cursor)
				- [ ] Interaction
					- [x] Keyboard
						- [x] Edit pattern
						- [x] Jump line
					- [x] Left click (cursor)
					- [ ] Right click
						- [x] Edit pattern
						- [x] Edit column
						- [ ] Edit column effects
			- [x] Screen Timeline
				- [x] Lines
				- [x] Columns
				- [x] Add instance
				- [x] Del instance
				- [x] Select instance
				- [x] Move instance
				- [x] Resize instance (fix negative `beat_start`)
			- [ ] Screen Matrix
			- [ ] Screen Tuning
				- [x] Default tuning
				- [x] User tuning
				- [ ] Clean ui
			- [x] Context menu general
				- [x] Frame rate (audio, audio / 2, audio / 4, ...)
				- [x] Base pitch (default: 440)
				- [x] Tuning
					- [x] Presets temperament (equal, just, ...)
					- [x] Presets scales (major, minor, modes, ...)
					- [x] Presets EDO (2edo, 3edo, ..., 11edo)
					- [x] Manual
						- [x] Cent
						- [x] Ratio
						- [x] Edo
			- [x] Context menu specific
				- [x] Synth
					- [x] Synth channel count
					- [x] Synth mode (gate | trigger | drum)
				- [x] Pattern
					- [x] Pattern length
					- [x] Pattern lpb
					- [x] Pattern note column count
					- [x] Pattern CV column count
					- [x] Pattern color
				- [ ] Column
					- [ ] Mute
					- [x] Note
						- [x] Effect count
					- [x] CV
						- [x] Mode
						- [x] Synth
						- [x] Index
		- [x] Display Side
			- [x] Screen Side Synths (Tracker)
				- [x] Draw synths
				- [x] Select synth
				- [x] Edit Synth
					- [x] Basics (name + color)
					- [x] Synth mode & channel count
				- [x] Add synth
				- [x] Del synth
				- [x] Move synth (move & replace in patterns)
				- [x] Scroll
			- [x] Screen Side Patterns (Timeline)
				- [x] Draw patterns
				- [x] Select pattern
				- [x] Double click : open pattern tracker
				- [x] Edit pattern
					- [x] Basics (name + color)
					- [x] Pattern specs (length, lpb, etc.)
				- [x] Add pattern
				- [x] Del pattern (remove instances)
				- [x] Move pattern (move & replace in instances)
				- [x] Duplicate pattern
				- [x] Scroll
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


- [x] MODULE TrackerSynth / TrackerDrum
	- [x] FRONTEND
		- [x] Context menu
			- [x] CV mode range
			- [x] CV mode mapping
	- [x] BACKEND
		- [x] CV
			- [x] CV mode range (min / max sliders)
			- [x] CV mode mapping

- [] MODULE TrackerClock (mult & div clock)
	- [ ] Mode like TrackerPhase


- [ ] MODULE TrackerPhase (mult & div synced LFO)
	- [x] Mode fixed	(Restart on loop)
	- [ ] Mode Loop		(Keep going on loop)
	- ! ! ! Useless phase[] table


- [x] MODULE TrackerQuant (quantizer with Tracker pitch & temperament)
	- [x] Mode note nearest (input searched into scale table)
	- [x] Mode index (input used to index scale table)
		- [x] Mode index down
		- [x] Mode index up
		- [x] Mode index round


- [x] MODULE TrackerState (Playing gate & trigger)
	- [x] State outputs
		- [x] Play gate
		- [x] Play trigger
		- [x] Stop trigger
		- [x] Play + Stop trigger
	- [x] Visualizer display
		- [x] Synth voices note		(Y axis)
		- [x] Synth voices velocity	(width or alpha)
		- [x] Synth voices panning	(X axis)
		- [x] Synth voices color


# Effects

[x] Axx	Amplitude random		(xx: noise amplitude)
		Multiply base amplitude by noise (noise can only make it lower)
[x] Pxx	Pan random				(xx: pan amplitude)
		Add noise to base panning (bipolar noise)
[ ] pxx	Pitch (micro) random	(xx: pitch amplitude)
[ ] txx Tuning (micro) fixed	(xx: pitch amplitude up to 1 semitone)
		xx = 0  : Note n
		xx = 99 : Note n + 1
		Transition from 0 to 99 depends on temperament table
		Microtonal + MPE
[x] Dxx Delay random			(xx: delay amplitude)
[x] Oxy	Octave random			(x: mode y: amplitude)
		x = 0 : -+ offset
		x = 1 : + offset
		x = 2 : - offset
[x] Mxy	Note random semitone	(x: semitone tone 1st offset y: 2nd offset)
		Peek random note between [base note], [base + x] and [base + y]
		x & y being semitone offset
		Can be cumulable
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