
#ifndef ACRO_HPP
#define ACRO_HPP

#include "../plugin.hpp"

#define ACRO_W				64
#define ACRO_H				24
#define ACRO_CHAR_H			12.0
#define ACRO_CHAR_W			8.37 //8.32461165625

#define ACRO_FLAG_ZERO			0b00000000	// No flag
#define ACRO_FLAG_OWNED			0b00000001	// Is owned by another ope
#define ACRO_FLAG_OWNED_FOCUS	0b00000010	// Is owned (focus) by another ope
#define ACRO_FLAG_OWNED_OUTPUT	0b00000100	// Is owned (focus) by another ope
#define ACRO_FLAG_BANG			0b00001000	// Is banged
#define ACRO_FLAG_PROCESSED		0b00010000	// Has already been computed
#define ACRO_FLAG_OPE			0b00100000	// Is ope

//#define ACRO_FLAG_FREE		0
//#define ACRO_FLAG_BANG		1
//#define ACRO_FLAG_OWNED		2

#define ACRO_C_0			0
#define ACRO_C_A			10
#define ACRO_C_B			11
#define ACRO_C_C			12
#define ACRO_C_D			13
#define ACRO_C_E			14
#define ACRO_C_F			15
#define ACRO_C_G			16
#define ACRO_C_H			17

#define ACRO_C_Z			35

#define ACRO_C_BANG			36
#define ACRO_C_MIDI			37
#define ACRO_C_CV			38
#define ACRO_C_COMMENT		39

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

extern char	acro_chars[64];

struct Acro;
struct AcroWidget;
struct AcroDisplay;

struct AcroChar {
	i8		enabled;		// Cell written
	i8		value;			// Cell value
	i8		upper;			// Cell uppercase
	u8		flags;			// Cell playing flag
};

struct Acro: Module {
	enum	ParamIds {
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_CLOCK,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	dsp::TSchmittTrigger<float>	trigger_clock;

	AcroChar					grid[ACRO_H][ACRO_W];
	i8							cur_x;
	i8							cur_y;
	u64							beat;

	Acro();

	void		process(const ProcessArgs& args) override;
	AcroChar	*get(int x, int y);
	void		set(int x, int y, int value, int upper = -1, int flag = -1);
	void		reset(int x, int y);
	void		own(int x, int y, int flag = 0);
	void		bang(int x, int y);
};

struct AcroWidget : ModuleWidget {
	Acro			*module;
	AcroDisplay		*display;

	AcroWidget(Acro* _module);
};

struct AcroDisplay : LedDisplay {
	Acro					*module;
	ModuleWidget			*moduleWidget;
	std::string				font_path;

	AcroDisplay();

	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
	//void onSelectText(const SelectTextEvent &e) override;
	void onSelectKey(const SelectKeyEvent &e) override;
	void onButton(const ButtonEvent &e) override;
	//void onDragHover(const DragHoverEvent &e) override;
	//void onChange(const ChangeEvent &e) override;
};

#endif
