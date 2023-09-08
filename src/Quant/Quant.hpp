
#ifndef QUANT_HPP
#define QUANT_HPP

#include "../plugin.hpp"

#define CHAR_W						6.302522
#define CHAR_H						8.5
#define QUANT_CENT	0
#define QUANT_RATIO	1

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct QuantNote {
	bool				type;		// Cent or Ratio;
	float				numerator;
	float				denominator;

	QuantNote(float cent);
	QuantNote(int numerator, int denominator);
	void to_ratio(int precision);
	void to_cent(void);
};

struct Quant : Module {
	enum				ParamIds {
		PARAM_COUNT
	};
	enum				InputIds {
		INPUT_PITCH,
		INPUT_WHEEL,
		INPUT_COUNT
	};
	enum				OutputIds {
		OUTPUT_PITCH,
		OUTPUT_COUNT
	};
	enum				LightIds {
		LIGHT_COUNT
	};
	vector<QuantNote>	notes;

	Quant();
	void process(const ProcessArgs& args) override;
	//void onReset(const ResetEvent &e) override;

	void note_add_menu(void);
	void note_add(float cent);
	void note_add(int numerator, int denominator);
	void note_del(int index);
};

struct QuantWidget : ModuleWidget {
	Quant				*module;
	int					note_id;
	QuantNote			*note;

	QuantWidget(Quant *_module);
	//void appendContextMenu(Menu *menu) override;
};

struct QuantDisplayList : LedDisplay {
	Quant*				module;
	QuantWidget*		moduleWidget;
	std::string			font_path;
	Vec					mouse_pos;
	int					mouse_button;
	int					mouse_action;
	float				scroll;

	QuantDisplayList();

	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs& args, int layer) override;
	void onHover(const HoverEvent &e) override;
	void onButton(const ButtonEvent &e) override;
	void onLeave(const LeaveEvent &e) override;
	void onHoverScroll(const HoverScrollEvent &e) override;
	void onSelectKey(const SelectKeyEvent &e) override;
	void draw_notes(const DrawArgs &args, Rect rect);
};

#endif
