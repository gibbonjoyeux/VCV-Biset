
#include "Omega3.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void Omega::spread(
	Output	*output,
	int		channels,
	float	phase,
	float	shape,
	float	curve,
	bool	curve_order,
	float	from,
	float	to) {
	float	t;
	int		i;

	output->setChannels(channels);

	for (i = 0; i < channels; ++i) {

		/// [1] INIT PHASE
		t = (float)i / (float)channels;

		/// [2] HANDLE CURVE (PRE)
		if (curve_order == 0)
			t = std::pow(t, std::pow(2, curve * 2.0));

		/// [3] HANDLE PHASE
		t = fmod(fmod(t + phase, 1.0) + 1.0, 1.0);

		/// [4] HANDLE SHAPE
		//// WAVE UP
		if (t < shape) {
			/// AVOID ZERO DIVISION
			if (shape > 0.0001)
				t = 1.0 - (1.0 - (t / shape));
		//// WAVE DOWN
		} else {
			/// AVOID ZERO DIVISION
			if (shape < 0.9999)
				t = 1.0 - ((t - shape) / (1.0 - shape));
		}

		/// [5] HANDLE CURVE (POST)
		if (curve_order == 1)
			t = std::pow(t, std::pow(2, curve * 2.0));

		/// [6] HANDLE RANGE
		output->setVoltage(from * (1.0 - t) + to * t, i);
	}
}
