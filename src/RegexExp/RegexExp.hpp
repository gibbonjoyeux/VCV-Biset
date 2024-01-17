
#ifndef REGEX_GATE_HPP
#define REGEX_GATE_HPP

#include "../plugin.hpp"

#define REGEXEXP_MODE_REGULAR		0
#define REGEXEXP_MODE_CONDENSED		1

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct RegexExp;
struct RegexExpWidget;

struct RegexExp : Module {
	enum	ParamIds {
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_COUNT
	};
	enum	OutputIds {
		ENUMS(OUTPUT_GATE, 12),
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	RegexExp(void);
	void process(const ProcessArgs& args) override;
};

struct RegexExpWidget : ModuleWidget {
	RegexExp			*module;
	bool				mode;

	RegexExpWidget(RegexExp *_module);
	void step(void) override;
};

#endif
