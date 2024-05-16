
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
	bool	curve_reverse;
	float_4	curve_ratio;
	float_4	t;
	int		i, j;

	output->setChannels(channels);

	if (curve >= 0) {
		curve_ratio = std::pow(2, curve * 2.0);
		curve_reverse = true;
	} else {
		curve_ratio = std::pow(2, -curve * 2.0);
		curve_reverse = false;
	}

	for (i = 0; i < channels; i += 4) {

		/// [1] INIT PHASE
		for (j = 0; j < 4; ++j) {
			t[j] = (float)(i + j) / (float)channels;
			if (t[j] < 0.00001)
				t[j] = 0.00001;
		}

		/// [2] HANDLE CURVE (PRE)
		if (curve_order == 0) {
			if (curve_reverse)
				t = 1.0 - simd::pow(1.0 - t, curve_ratio);
			else
				t = simd::pow(t, curve_ratio);
		}

		/// [3] HANDLE PHASE
		t = simd::fmod(simd::fmod(t + phase, 1.0) + 1.0, 1.0);

		/// [4] HANDLE SHAPE
		for (j = 0; j < 4; ++j) {
			//// WAVE UP
			if (t[j] < shape) {
				/// AVOID ZERO DIVISION
				if (shape > 0.0001)
					t[j] = 1.0 - (1.0 - (t[j] / shape));
			//// WAVE DOWN
			} else {
				/// AVOID ZERO DIVISION
				if (shape < 0.9999)
					t[j] = 1.0 - ((t[j] - shape) / (1.0 - shape));
			}
		}

		/// [5] HANDLE CURVE (POST)
		if (curve_order == 1) {
			if (curve_reverse)
				t = 1.0 - simd::pow(1.0 - t, curve_ratio);
			else
				t = simd::pow(t, curve_ratio);
		}

		/// [6] HANDLE RANGE
		for (j = 0; j < 4; ++j)
			output->setVoltage(from * (1.0 - t[j]) + to * t[j], i + j);
	}
}
