
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

RegexDisplay::RegexDisplay() {
	this->multiline = true;
	this->fontPath = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
	this->bgColor = colors[15];
	this->color = colors[4];
	this->textOffset = Vec(-1.0, -1.0);
	this->syntax = true;
	LedDisplayTextField();
}

void RegexDisplay::draw_preview(const DrawArgs &args) {
	Rect					rect;

	LedDisplayTextField::draw(args);

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// RECT BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[15]);
	this->color = colors[4];
	nvgRoundedRect(args.vg, RECT_ARGS(rect), 5);
	nvgFill(args.vg);
}

void RegexDisplay::draw(const DrawArgs &args) {
	Rect					rect;

	LedDisplayTextField::draw(args);

	if (this->module == NULL) {
		this->draw_preview(args);
		return;
	}
	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// RECT BACKGROUND
	nvgBeginPath(args.vg);
	if (this->sequence->mode == REGEX_MODE_CLOCK) {
		nvgFillColor(args.vg, colors[15]);
		this->color = colors[4];
	//} else if (this->sequence->mode == REGEX_MODE_RACHET) {
	//	nvgFillColor(args.vg, colors[14]);
	//	this->color = colors[4];
	} else if (this->sequence->mode == REGEX_MODE_PITCH) {
		//nvgFillColor(args.vg, colors[13]);
		//this->color = colors[15];
		nvgFillColor(args.vg, colors[14]);
		this->color = colors[4];
	//} else if (this->sequence->mode == REGEX_MODE_OCTAVE) {
	//	nvgFillColor(args.vg, colors[12]);
	//	this->color = colors[14];
	}
	nvgRoundedRect(args.vg, RECT_ARGS(rect), 5);
	nvgFill(args.vg);
	/// RECT STATE
	//// STATE SYNTAX ERROR
	if (this->syntax == false && this->text.empty() == false)
		nvgFillColor(args.vg, colors[2]);
	//// STATE RUNNING
	else if (this->sequence->sequence_string == this->text)
		nvgFillColor(args.vg, colors[5]);
	//// STATE EDITING
	else
		nvgFillColor(args.vg, colors[10]);
	nvgBeginPath(args.vg);
	nvgRoundedRect(args.vg, rect.pos.x + rect.size.x - 20, rect.pos.y, 20, rect.size.y, 5);
	nvgFill(args.vg);
	nvgBeginPath(args.vg);
	nvgRect(args.vg, rect.pos.x + rect.size.x - 20, rect.pos.y, 10, rect.size.y);
	nvgFill(args.vg);
}

void RegexDisplay::onSelectText(const SelectTextEvent &e) {
	if (e.codepoint != ' ')
		LedDisplayTextField::onSelectText(e);
	/// CHECK SYNTAX
	this->check_syntax();
}

void RegexDisplay::onSelectKey(const SelectKeyEvent &_e) {
	SelectKeyEvent	e;
	int				len;
	int				i;

	e = _e;
	/// HANDLE ENTER
	if (e.key == GLFW_KEY_ENTER) {
		if (e.action == GLFW_PRESS) {
			/// UPDATE ALL SEQUENCES
			if (e.mods & GLFW_MOD_CONTROL) {
				for (i = 0; i < 8; ++i)
					this->module->sequences[i].compile();
			/// UPDATE SEQUENCE
			} else {
				this->sequence->compile();
			}
		}
		e.consume(this);
		return;
	}
	/// HANDLE DELETE / BACKSPACE
	if (e.key == GLFW_KEY_DELETE
	|| e.key == GLFW_KEY_BACKSPACE)
		e.mods = 0;
	/// HANDLE KEY DOWN / UP
	if (e.action == GLFW_PRESS || e.action == GLFW_REPEAT) {
		/// HANDLE KEY DOWN
		if (e.key == GLFW_KEY_DOWN) {
			/// GO TO NEXT TEXT FIELD
			if (e.mods & GLFW_MOD_CONTROL) {
				if (this->display_next)
					APP->event->setSelectedWidget(this->display_next);
			/// GO TO NEXT LINE
			} else {
				len = this->text.length();
				if (this->cursor + 32 <= len)
					this->cursor += 32;
				this->selection = this->cursor;
			}
		/// HANDLE KEY UP
		} else if (e.key == GLFW_KEY_UP) {
			/// GO TO PREVIOUS TEXT FIELD
			if (e.mods & GLFW_MOD_CONTROL) {
				if (this->display_prev)
					APP->event->setSelectedWidget(this->display_prev);
			/// GO TO PREVIOUS LINE
			} else {
				if (this->cursor - 32 >= 0)
					this->cursor -= 32;
				this->selection = this->cursor;
			}
		}
	}
	LedDisplayTextField::onSelectKey(e);
	/// CHECK SYNTAX
	this->check_syntax();
}

bool RegexDisplay::check_syntax_seq(char *str, int &i) {
	bool				brackets;

	brackets = false;
	/// HANDLE MODE (OPTIONNAL)
	if (str[i] == 0)
		return false;
	if (IS_MODE(str[i]))
		i += 1;
	/// HANDLE BRACKETS
	if (str[i] == 0)
		return false;
	if (str[i] == '(') {
		i += 1;
		brackets = true;
	}
	/// HANDLE SEQUENCE
	if (str[i] == 0)
		return false;
	while (true) {
		/// HANDLE VALUE AS CLOCK VALUE
		if (this->sequence->mode == REGEX_MODE_CLOCK
		&& IS_DIGIT(str[i])) {
			while (IS_DIGIT(str[i]))
				i += 1;
		/// HANDLE VALUE AS PITCH VALUE
		} else if (this->sequence->mode == REGEX_MODE_PITCH
		&& IS_PITCH(str[i])) {
			i += 1;
			if (str[i] == '#' || str[i] == 'b')
				i += 1;
		/// HANDLE VALUE AS SEQUENCE (RECURSIVE)
		} else if (IS_MODE(str[i])) {
			if (check_syntax_seq(str, i) == false)
				return false;
		/// HANDLE ERROR
		} else {
			return false;
		}
		/// HANDLE SEPARATOR
		if (str[i] == ',') {
			i += 1;
		/// HANDLE SEQUENCE END
		} else if (str[i] == ')') {
			if (brackets == true) {
				i += 1;
				if (IS_MODULATOR(str[i]))
					break;
				return true;
			}
			return true;
		/// HANDLE MODULATOR
		} else if (IS_MODULATOR(str[i])) {
			break;
		/// HANDLE END
		} else if (str[i] == 0) {
			if (brackets == true)
				return false;
			return true;
		/// HANDLE ERROR
		} else {
			return false;
		}
	}
	/// HANDLE MODULATOR
	i += 1;
	if (IS_DIGIT(str[i])) {
		while (IS_DIGIT(str[i]))
			i += 1;
		return true;
	}
	return false;
}

bool RegexDisplay::check_syntax(void) {
	char							*str;
	int								i;

	str = (char*)this->text.c_str();
	i = 0;
	this->syntax = this->check_syntax_seq(str, i);
	return this->syntax;
}
