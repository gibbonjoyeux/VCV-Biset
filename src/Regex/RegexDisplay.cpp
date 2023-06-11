
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

void RegexDisplay::draw(const DrawArgs &args) {
	Rect					rect;

	LedDisplayTextField::draw(args);

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	if (this->syntax == true)
		nvgFillColor(args.vg, this->bgColor);
	else
		nvgFillColor(args.vg, colors[1]);
	//nvgRect(args.vg, RECT_ARGS(rect));
	nvgRoundedRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0, 5);
	nvgFill(args.vg);
}

void RegexDisplay::onSelectText(const SelectTextEvent &e) {
	if (e.codepoint != ' ')
		LedDisplayTextField::onSelectText(e);
	/// CHECK SYNTAX
	this->syntax = this->check_syntax();
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
				for (i = 0; i < 4; ++i)
					this->module->sequences[i].compile();
			/// UPDATE SEQUENCE
			} else {
				// TODO: compile only rythm or pitch
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
	this->syntax = this->check_syntax();
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
		/// HANDLE VALUE AS NUMBER
		if (IS_DIGIT(str[i])) {
			while (IS_DIGIT(str[i]))
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
	char		*str;
	int			i;

	str = (char*)this->text.c_str();
	i = 0;
	return this->check_syntax_seq(str, i);
}
