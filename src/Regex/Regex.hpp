
#ifndef REGEX_HPP
#define REGEX_HPP

#include "../plugin.hpp"

#define REGEX_VALUE			0
#define REGEX_SEQUENCE		1

#define IS_MODE(c)		(c == '#' || c == '@' || c == '?' || c == '!' || c == '$')
#define IS_DIGIT(c)		(c >= '0' && c <= '9')
#define IS_PITCH(c)		((c >= 'a' && c <= 'g') || (c >= 'A' && c <= 'G'))
#define IS_MODULATOR(c)	(c == 'x' || c == '%')

struct Regex;
struct RegexItem;
struct RegexSeq;
struct RegexDisplay;
struct RegexWidget;

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct RegexItem {
	u8								type;				// Item type (Value OR Sequence)
	struct {
		u8							value;				// Value
		u8							index;				// Value index on expression
	}								value;
	struct {
		u8							state_a;			// Seq state a
		u8							state_b;			// Seq state b
		char						mode;				// Seq mode
		char						modulator_mode;		// Seq modulator type (x,%)
		u8							modulator_value;	// Seq modulator value
		u8							length;				// Seq length
		list<RegexItem>::iterator	it;					// Seq iterator
		bool						keep;				// Seq is in keep mode
		list<RegexItem>				sequence;			// Seq items
	}								sequence;

	bool pull(int &value);
	bool pull_seq(int &value);
	bool pull_shuffle(int &value);
	bool pull_rand(int &value);
	bool pull_xrand(int &value);
	bool pull_walk(int &value);
	void shuffle(void);
};

struct RegexSeq {
	RegexItem				*sequence_rythm;
	RegexItem				*sequence_rythm_next;
	RegexItem				*sequence_pitch;
	RegexItem				*sequence_pitch_next;
	RegexDisplay			*display_rythm;
	RegexDisplay			*display_pitch;
	Output					*out_rythm;
	Output					*out_pitch;
	int						trigger_divider;
	int						trigger_count;
	dsp::PulseGenerator		trigger;

	RegexSeq();
	~RegexSeq();
	void process(float dt, bool trigger);
	void compile(void);
	void compile_rythm(void);
	void compile_rythm_req(RegexItem *item, char *str, int &i);
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
		INPUT_COUNT
	};
	enum	OutputIds {
		ENUMS(OUTPUT_GATE, 4),
		ENUMS(OUTPUT_PITCH, 4),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};
	RegexWidget					*widget;
	RegexSeq					sequences[4];
	dsp::TSchmittTrigger<float>	clock;

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
