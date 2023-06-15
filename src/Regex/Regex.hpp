
#ifndef REGEX_HPP
#define REGEX_HPP

#include "../plugin.hpp"

#define REGEX_VALUE			0
#define REGEX_SEQUENCE		1

#define REGEX_MODE_CLOCK	0
#define REGEX_MODE_PITCH	1
//#define REGEX_MODE_RACHET	2
//#define REGEX_MODE_OCTAVE	3

#define IS_MODE(c)		(c == '#' || c == '@' || c == '?' || c == '!' || c == '$')
#define IS_DIGIT(c)		(c >= '0' && c <= '9')
#define IS_PITCH(c)		((c >= 'a' && c <= 'g') || (c >= 'A' && c <= 'G'))
#define IS_MODULATOR(c)	(c == 'x' || c == '%')

extern int	table_pitch_midi[7];

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
		i8							value;				// Value
		u8							index;				// Value index on expression
	}								value;
	struct {
		u8							state_a;			// Seq state a
		u8							state_b;			// Seq state b
		u8							state_c;			// Seq state c
		char						mode;				// Seq mode
		char						modulator_mode;		// Seq modulator type (x,%)
		u8							modulator_value;	// Seq modulator value
		u8							length;				// Seq length
		list<RegexItem>::iterator	it;					// Seq iterator
		bool						keep;				// Seq is in keep mode
		list<RegexItem>				sequence;			// Seq items
	}								sequence;

	bool pull_clock(int &value, int &index);
	bool pull_clock_seq(int &value, int &index);
	bool pull_clock_shuffle(int &value, int &index);
	bool pull_clock_rand(int &value, int &index);
	bool pull_clock_xrand(int &value, int &index);
	bool pull_clock_walk(int &value, int &index);
	bool pull_pitch(int &value, int &index);
	bool pull_pitch_seq(int &value, int &index);
	bool pull_pitch_shuffle(int &value, int &index);
	bool pull_pitch_rand(int &value, int &index);
	bool pull_pitch_xrand(int &value, int &index);
	bool pull_pitch_walk(int &value, int &index);
	void reset(void);
	void select(int index);
	void shuffle(void);
};

struct RegexSeq {
	u8							mode;			// Clock | Rachet | Pitch | Oct
	RegexItem					*sequence;
	RegexItem					*sequence_next;
	RegexDisplay				*display;
	Input						*in_1;
	Input						*in_2;
	Output						*out;
	std::string					sequence_string;
	std::string					sequence_next_string;
	int							clock_out_divider;
	int							clock_out_count;
	dsp::PulseGenerator			clock_out;
	dsp::TSchmittTrigger<float>	clock_in_1;
	dsp::TSchmittTrigger<float>	clock_in_2;

	RegexSeq();
	~RegexSeq();
	void reset(bool destroy);
	void process(float dt, bool clock_master);
	void compile(void);
	void compile_req(RegexItem *item, char *str, int &i);
};

struct Regex : Module {
	enum	ParamIds {
		ENUMS(PARAM_MODE, 8),
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_RESET,
		INPUT_MASTER,
		ENUMS(INPUT_EXP_1, 8),
		ENUMS(INPUT_EXP_2, 8),
		INPUT_COUNT
	};
	enum	OutputIds {
		ENUMS(OUTPUT_EXP, 8),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};
	RegexWidget					*widget;
	RegexSeq					sequences[8];
	dsp::TSchmittTrigger<float>	clock_reset;
	dsp::TSchmittTrigger<float>	clock_master;

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
	int						active_value;

	RegexDisplay();

	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
	void draw_preview(const DrawArgs &args);
	void onSelectText(const SelectTextEvent &e) override;
	void onSelectKey(const SelectKeyEvent &e) override;

	bool check_syntax(void);
	bool check_syntax_seq(char *str, int &i);
};

struct RegexWidget : ModuleWidget {
	Regex					*module;
	RegexDisplay			*display[8];

	RegexWidget(Regex * _module);
	//void onSelect(const SelectEvent &e) override;
	//void appendContextMenu(Menu *menu) override;
};

#endif
