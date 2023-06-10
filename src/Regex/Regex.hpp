
#ifndef REGEX_HPP
#define REGEX_HPP

#include "../plugin.hpp"

#define REGEX_VALUE			0
#define REGEX_SEQUENCE		1

struct Regex;
struct RegexItem;
struct RegexSeq;
struct RegexDisplay;
struct RegexWidget;

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct RegexItem {
	u8						type;				// Item type (Value OR Sequence)
	u8						value;				// Value
	u8						state_a;			// Seq state a
	u8						state_b;			// Seq state b
	u8						mode;				// Seq mode
	u8						modulator_mode;		// Seq modulator type (x,%)
	u8						modulator_value;	// Seq modulator value
	u8						length;				// Seq length
	bool					keep;				// Seq is in keep mode
	list<RegexItem>			sequence;			// Seq items
};

struct RegexSeq {
	RegexItem				*seq;
	RegexItem				*seq_next;
	RegexDisplay			*display_rythm;
	RegexDisplay			*display_pitch;

	RegexSeq();
	~RegexSeq();
};

struct Regex : Module {
	enum	ParamIds {
		PARAM_SYNTH,
		PARAM_BPM,
		ENUMS(PARAM_MODE, 4),
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_CLOCK,
		INPUT_BPM,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_CLOCK,
		OUTPUT_BPM,
		ENUMS(OUTPUT_GATE, 4),
		ENUMS(OUTPUT_PITCH, 4),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};
	RegexWidget				*widget;
	RegexSeq				sequences[4];

	Regex();
	void	process(const ProcessArgs& args) override;
};

struct RegexDisplay : LedDisplayTextField {
	Regex					*module;
	ModuleWidget			*moduleWidget;
	RegexSeq				*sequence;
	RegexDisplay			*display_prev;
	RegexDisplay			*display_next;
	bool					syntax;

	RegexDisplay();

	void draw(const DrawArgs &args) override;
	void onSelectText(const SelectTextEvent &e) override;
	void onSelectKey(const SelectKeyEvent &e) override;

	bool check_syntax(void);
	bool check_syntax_seq(char *str, int &i);
};

struct RegexWidget : ModuleWidget {
	Regex					*module;
	RegexDisplay			*display_rythm[4];
	RegexDisplay			*display_pitch[4];

	RegexWidget(Regex * _module);
	//void onSelect(const SelectEvent &e) override;
	//void appendContextMenu(Menu *menu) override;
};

#endif
