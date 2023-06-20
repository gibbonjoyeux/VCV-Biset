
#ifndef REGEX_HPP
#define REGEX_HPP

#include "../plugin.hpp"

#define REGEX_VALUE			0
#define REGEX_SEQUENCE		1

#define REGEX_MODE_CLOCK	0
#define REGEX_MODE_PITCH	1

#define IS_MODE(c)		(c == '>' || c == '<' || c == '^' || c == '@' || c == '?' || c == '!' || c == '$')
#define IS_DIGIT(c)		(c >= '0' && c <= '9')
#define IS_PITCH(c)		((c >= 'a' && c <= 'g') || (c >= 'A' && c <= 'G'))
#define IS_MODULATOR(c)	(c == 'x' || c == '%')

extern int	table_pitch_midi[7];

struct Regex;
struct RegexItem;
struct RegexSeq;
struct RegexDisplay;
struct RegexWidget;
struct RegexWidgetCondensed;

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
	bool pull_clock_foreward(int &value, int &index);
	bool pull_clock_backward(int &value, int &index);
	bool pull_clock_pingpong(int &value, int &index);
	bool pull_clock_shuffle(int &value, int &index);
	bool pull_clock_rand(int &value, int &index);
	bool pull_clock_xrand(int &value, int &index);
	bool pull_clock_walk(int &value, int &index);

	bool pull_pitch(int &value, int &index);
	bool pull_pitch_foreward(int &value, int &index);
	bool pull_pitch_backward(int &value, int &index);
	bool pull_pitch_pingpong(int &value, int &index);
	bool pull_pitch_shuffle(int &value, int &index);
	bool pull_pitch_rand(int &value, int &index);
	bool pull_pitch_xrand(int &value, int &index);
	bool pull_pitch_walk(int &value, int &index);

	void reset(void);
	void select(int index);
	void shuffle(void);
};

struct RegexSeq {
	u8							mode;			// Clock | Pitch
	RegexItem					*sequence;
	RegexItem					*sequence_next;
	RegexDisplay				*display;
	Input						*in_reset;
	Input						*in_1;
	Input						*in_2;
	Output						*out;
	Output						*out_eoc;
	std::string					sequence_string;
	std::string					sequence_next_string;
	int							clock_out_divider;
	int							clock_out_count;
	dsp::PulseGenerator			clock_out;
	dsp::PulseGenerator			clock_out_eoc;
	dsp::TSchmittTrigger<float>	clock_in_reset;
	dsp::TSchmittTrigger<float>	clock_in_1;
	dsp::TSchmittTrigger<float>	clock_in_2;

	RegexSeq();
	~RegexSeq();
	void reset(bool destroy);
	void process(float dt, bool clock_reset_master, bool clock_master);
	void compile(Regex *module);
	void compile_req(RegexItem *item, char *str, int &i);
};

struct Regex : Module {
	enum	ParamIds {
		ENUMS(PARAM_MODE, 12),
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_RESET,
		INPUT_MASTER,
		ENUMS(INPUT_EXP_RESET, 12),
		ENUMS(INPUT_EXP_1, 12),
		ENUMS(INPUT_EXP_2, 12),
		INPUT_COUNT
	};
	enum	OutputIds {
		ENUMS(OUTPUT_EXP, 12),
		ENUMS(OUTPUT_EXP_EOC, 12),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};
	std::atomic_flag			thread_flag;
	RegexWidget					*widget;
	RegexSeq					sequences[12];
	dsp::TSchmittTrigger<float>	clock_reset;
	dsp::TSchmittTrigger<float>	clock_master;
	std::string					expressions[12];
	int							exp_count;

	Regex(bool condensed = false);
	void process(const ProcessArgs& args) override;
	json_t *dataToJson(void) override;
	void dataFromJson(json_t *j_root) override;
};

struct RegexCondensed : Regex {
	RegexCondensed() : Regex(true) {};
};

struct RegexDisplay : LedDisplayTextField {
	Regex					*module;
	ModuleWidget			*moduleWidget;
	RegexSeq				*sequence;
	RegexDisplay			*display_prev;
	RegexDisplay			*display_next;
	bool					condensed;
	bool					syntax;
	float					char_width;
	int						active_value;

	RegexDisplay();

	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
	void draw_preview(const DrawArgs &args);
	void onSelectText(const SelectTextEvent &e) override;
	void onSelectKey(const SelectKeyEvent &e) override;
	void onButton(const ButtonEvent &e) override;
	void onDragHover(const DragHoverEvent &e) override;

	bool check_syntax(void);
	bool check_syntax_seq(char *str, int &i);
};

struct RegexWidget : ModuleWidget {
	Regex					*module;
	RegexDisplay			*display[12];
	bool					condensed;
	int						exp_count;

	RegexWidget(Regex *_module, bool condensed = false);
	void appendContextMenu(Menu *menu) override;
};

struct RegexCondensedWidget : RegexWidget {
	RegexCondensedWidget(Regex *_module) : RegexWidget(_module, true) {};
};

#endif
