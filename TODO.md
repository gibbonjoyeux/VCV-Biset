
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

- [ ] Chance as effect (Cxx + cxx)
- [x] Handle delay on note stop / new for running note
- [ ] Define synth row order (PITCH-OCT VELO SYNTH DELAY GLIDE FX FX ...)


# Effects

[ ] Axx	Amplitude random		(xx: noise amplitude)
		Multiply base amplitude by noise (noise can only make it lower)
[ ] Pxx	Pan random				(xx: pan amplitude)
		Add noise to base panning (bipolar noise)
[ ] Oxy	Octave random			(x: mode y: amplitude)
		x = 0 : -+ offset
		x = 1 : + offset
		x = 2 : - offset
[ ] Mxy	Note random semitone	(x: semitone tone 1st offset y: 2nd offset)
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
[ ] Cxx	Chance keep				(xx: chance)
		Keep previous running note on if chance does not occur
[ ] cxx	Chance stop				(xx: chance)
		Stop previous running note even if chance does not occur
[ ] Rxx	Ratchet / Retrigger		(xx: retrigger interval) ? (x: amp fade y: interval)
		x<8 : decrease | x>8 : increase | x=8 : static
		y represents the interval between retriggers
