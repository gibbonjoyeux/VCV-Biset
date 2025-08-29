
# Change log

## 2.0.15 (canceled for now)

- Tracker
	- [ ] Extra
		- [ ] New Fx restart pattern playhead (with chance)
	- [ ] Midi
		- [ ] Record polyphony
	- [ ] Shortcuts
		- [ ] Shortcut play / pause
- IGC
	- [ ] Freeze buffer switch (or expander)
	- [ ] Adaptative samplerate

## 2.0.14 (Sending)

- IGC
	- [x] Grain mode that can handle mono phase but poly speed or grain
	- [x] Grain shape control
	- [x] Extra shape control (so that fade in/out starts from further than center)
	- [x] Fix anti-click burst of noise (see simd)

## 2.0.13 (jun 24, 2024)

- Tracker
	- [x] Fixes
		- [x] Fix note fx save
		- [x] Fix New pattern crash / populated
		- [x] T-Synth/T-Drum CV mapping issue
		- [x] T-Synth/T-Drum velocity / panning jump clicking (slew limiter)
	- [x] New Information Display
		- [x] Pattern
			- [x] Display column synth
			- [x] Display column cell (note, velo, panning, ...)
			- [x] Display fx help
		- [x] Timeline
			- [x] Display current time / total time (beats)
	- [x] Midi
		- [x] Record midi velocity
	- [x] Shortcuts
		- [x] Arrows to -/+ note semitone / octave
		- [x] Arrows to -/+ note velo / panning / ...
		- [x] Arrows to -/+ note fx
		- [x] Arrows to -/+ CV
- Omega-3 + Omega-6
	- [x] Audiorate

## 2.0.12 (apr 8, 2024)

- Tracker
	- [x] Added CV interpolation curve controls
	- [x] Fixed key released crash
- [x] New **It's good cholesterol** effect / VCO module
- [x] New **Omega3** utility module
- [x] New **Omega6** utility module

## 2.0.11 (mar 30, 2024)

- Blank
	- [x] Added led + bloom option
- Regex
	- [x] Fixed clock `*` modulator
	- [x] Change cursor color for dark room (bloom light ?)
- Tree
	- [x] Added branch mutation chance (context menu)
- [x] New **Tree Seed** expander module !
	- [x] Sequence from Tree
	- [x] Polyphonic output

## 2.0.10 (feb 26, 2024)

- Tracker
	- [x] Switched to JSON saving format (more stable)
	- [x] Fixed Tracker deletion lost memory issue
- Blank
	- [x] Fixed closing crash 3
- Please Kill Me
	- [x] Added detune stereo rotation LFO (context menu)
	- [x] Fixed double mode

## 2.0.9 (feb 14, 2024)

- Blank
	- [x] Fixed closing crash 1 & 2
	- [x] Add new parameters
	- [x] Added cable slew limiter
	- [x] Added more scope controls
	- [x] Added panels

## 2.0.8 (jan 27, 2024)

- Tracker
	- [x] Fixed `c` effect crash
	- [x] Shortcut `space` to add note stop
	- [x] Shortcut `arrow` keys to move timeline camera
	- [x] Shortcut `delete` to remove pattern instance
	- [x] Shortcut `insert` / `suppr` to move lines
	- [x] Context menu help (shortcuts + effects)
- Regex
	- [x] Inline `x` modulator multiplication
	- [x] Fixed / enhanced bias knob
	- [x] Added `*` clock modulator !
	- [x] Can run on start-up (context menu)
	- [x] Gate output expander
- Please kill me
	- [x] Fixed no pitch input no output issue
- Blank
	- [x] Animated cables
	- [x] Hover scope

## 2.0.7 (jan 4, 2024)

- [x] New **Please kill me** synth module !
- [x] New **The good, the bad and the ugly** synth module !
- [x] New **Tree** sequencer module !
- [x] New **T-Control** tracker module !
- Tracker
	- [x] Fixed empty pattern crash
	- [x] Allows record on both playing modes
