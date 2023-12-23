
#include "Fft.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Fft::Fft() {
	int	i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	this->fft = new dsp::ComplexFFT(FFT_BUFFER);
	this->buffer_i = 0;
	for (i = 0; i < FFT_BUFFER; ++i)
		this->buffer[i] = 0.0;
}

void Fft::process(const ProcessArgs& args) {
	float	pitch;
	float	mag;
	float	mag_sqrt;
	float	max_mag[8];
	int		max_pos[8];
	int		i, j;

	this->buffer[this->buffer_i] = inputs[INPUT].getVoltage() / 5.0;
	this->buffer_i += 1;
	if (this->buffer_i >= FFT_BUFFER)
		this->buffer_i = 0;

	if (args.frame % (int)(FFT_BUFFER * 0.1) == 0) {

		this->fft->fftUnordered(this->buffer, this->buffer_out);

		//max_mag = 0.0;
		//max_pos = 0;
		for (i = 0; i < 8; ++i) {
			max_mag[i] = 0.0;
			max_pos[i] = 0;
		}

		for (i = 20; i < FFT_BUFFER; i += 2) {
			/// COMPUTE MAGNITUDE (SQUARED)
			mag = this->buffer_out[i] * this->buffer_out[i]
			/**/ + this->buffer_out[i + 1] * this->buffer_out[i + 1];

			/// STORE MAX MAGNITUDE
			//if (mag > max_mag) {
			//	max_mag = mag;
			//	max_pos = i;
			//}
			if (mag > max_mag[0]) {
				for (j = 7; j > 0; --j) {
					max_mag[j] = max_mag[j - 1];
					max_pos[j] = max_pos[j - 1];
				}
				max_mag[0] = mag;
				max_pos[0] = i;
			}
		}

		outputs[OUTPUT_PITCH].setChannels(8);
		outputs[OUTPUT_VELOCITY].setChannels(8);
		/// COMPUTE FREQUENCY
		// ref: http://howthefouriertransformworks.com/understanding-the-output-of-an-fft/
		for (i = 0; i < 8; ++i) {
			mag_sqrt = sqrt(max_mag[i]);
			float N = FFT_BUFFER;
			float R = N / ((float)FFT_BUFFER / 48000.0);
			float f = ((float)(max_pos[i] / 2) / N) * R;
			/* TMP */ f /= 2.0;
			pitch = log2(f / dsp::FREQ_C4);
			outputs[OUTPUT_PITCH].setVoltage(pitch, i);
			outputs[OUTPUT_VELOCITY].setVoltage(mag_sqrt * 0.02, i);
		}
	}
}

Model* modelFft = createModel<Fft, FftWidget>("Biset-Fft");
