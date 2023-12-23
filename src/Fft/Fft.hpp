
#ifndef FFT_HPP
#define FFT_HPP

#include "../plugin.hpp"

#define FFT_BUFFER	2048

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct Fft: Module {
	enum	ParamIds {
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_PITCH,
		OUTPUT_VELOCITY,
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	dsp::ComplexFFT	*fft;
	float			buffer[FFT_BUFFER * 2];
	float			buffer_out[FFT_BUFFER * 2];
	int				buffer_i;

	Fft();

	void		process(const ProcessArgs& args) override;
};

struct FftWidget : ModuleWidget {
	Fft			*module;

	FftWidget(Fft* _module);
};

struct FftDisplay : LedDisplay {
	Fft*					module;
	ModuleWidget*			moduleWidget;

	FftDisplay();

	//void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs& args, int layer) override;
};

#endif
