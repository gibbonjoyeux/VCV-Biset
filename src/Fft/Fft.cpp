
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
	for (i = 0; i < FFT_CHANNELS; ++i) {
		this->out_mag[i] = 0.0;
		this->out_pitch[i] = 0.0;
	}
}

void Fft::process(const ProcessArgs& args) {
	float	pitch;
	float	mag;
	float	mag_sqrt;
	float	max_mag[FFT_CHANNELS];
	int		max_pos[FFT_CHANNELS];
	int		i, j;

	this->buffer[this->buffer_i] = inputs[INPUT].getVoltage() / 5.0;
	this->buffer_i += 1;
	if (this->buffer_i >= FFT_BUFFER)
		this->buffer_i = 0;

	//if (args.frame % (int)(FFT_BUFFER * 0.1) == 0) {
	if (args.frame % (int)(args.sampleRate * 0.02) == 0) { // every 20ms

		/// COMPUTE BUFFER INPUT
		for (i = 0; i < FFT_BUFFER; ++i)
			this->buffer_in[i] = this->buffer[(this->buffer_i + i) % FFT_BUFFER];
		dsp::hannWindow(this->buffer_in, FFT_BUFFER);

		/// COMPUTE BUFFER OUTPUT
		this->fft->fftUnordered(this->buffer_in, this->buffer_out);

		/// COMPUTE BUFFER MAXIMUMS
		for (i = 0; i < FFT_CHANNELS; ++i) {
			max_mag[i] = 0.0;
			max_pos[i] = 0;
		}
		for (i = 1; i < FFT_BUFFER * 0.125; i += 2) {
			/// COMPUTE MAGNITUDE (SQUARED)
			mag = this->buffer_out[i] * this->buffer_out[i]
			/**/ + this->buffer_out[i + 1] * this->buffer_out[i + 1];
			/// STORE MAX MAGNITUDE
			if (mag > max_mag[0]) {
				for (j = FFT_CHANNELS - 1; j > 0; --j) {
					max_mag[j] = max_mag[j - 1];
					max_pos[j] = max_pos[j - 1];
				}
				max_mag[0] = mag;
				max_pos[0] = i;
			}
		}

		outputs[OUTPUT_PITCH].setChannels(FFT_CHANNELS);
		outputs[OUTPUT_VELOCITY].setChannels(FFT_CHANNELS);
		/// COMPUTE FREQUENCY
		// ref: http://howthefouriertransformworks.com/understanding-the-output-of-an-fft/
		for (i = 0; i < FFT_CHANNELS; ++i) {
			mag_sqrt = sqrt(max_mag[i]);
			float N = FFT_BUFFER;
			float R = N / ((float)FFT_BUFFER / args.sampleRate);
			float f = ((float)max_pos[i] / N) * R;
			/* TMP */ f /= 8.0;
			pitch = log2(f / dsp::FREQ_C4);
			if (std::isfinite(pitch) == false) {
				pitch = 0.0;
				mag_sqrt = 0.0;
			}
			this->out_mag_aim[i] = mag_sqrt;
			this->out_pitch_aim[i] = pitch;
		}
	}

	for (i = 0; i < FFT_CHANNELS; ++i) {
		this->out_mag[i] = this->out_mag[i] * 0.999 + this->out_mag_aim[i] * 0.001;
		this->out_pitch[i] = this->out_pitch[i] * 0.999 + this->out_pitch_aim[i] * 0.001;
		outputs[OUTPUT_PITCH].setVoltage(this->out_pitch[i], i);
		outputs[OUTPUT_VELOCITY].setVoltage(this->out_mag[i] * 0.1, i);
	}
}

Model* modelFft = createModel<Fft, FftWidget>("Biset-Fft");
