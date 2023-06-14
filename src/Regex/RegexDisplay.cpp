
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
	this->active_value = -1;
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

void RegexDisplay::drawLayer(const DrawArgs &args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	char					c[2];
	char					*str;
	int						i;
	float					char_width;

	if (module == NULL || layer != 1)
		return;
	/// GET FONT
	font = APP->window->loadFont(this->fontPath);
	if (font == NULL)
		return;
	/// SET FONT
	nvgFontSize(args.vg, 12);
	nvgFontFaceId(args.vg, font->handle);
	nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// DRAW TEXT
	if (this->sequence->sequence_string != this->text)
		this->active_value = -1;
	char_width = nvgTextBounds(args.vg, 0, 0, "x", NULL, NULL);
	str = (char*)this->text.c_str();
	c[1] = 0;
	i = 0;
	while (i < 64 && str[i] != 0) {
		c[0] = str[i];
		/// ON ACTIVE VALUE
		if (i == this->active_value) {
			nvgFillColor(args.vg, colors[3]);
			/// MODE CLOCK
			if (this->sequence->mode == REGEX_MODE_CLOCK) {
				while (IS_DIGIT(str[i])) {
					nvgText(args.vg,
					/**/ rect.pos.x + 3.0 + (float)i * char_width, rect.pos.y + 3.0,
					/**/ c, NULL);
					i += 1;
				}
			/// MODE PITCH
			} else if (this->sequence->mode == REGEX_MODE_PITCH) {
				if (IS_PITCH(str[i])) {
					nvgText(args.vg,
					/**/ rect.pos.x + 3.0 + (float)i * char_width, rect.pos.y + 3.0,
					/**/ c, NULL);
					i += 1;
					if (str[i] == '#' || str[i] == 'b') {
						nvgText(args.vg,
						/**/ rect.pos.x + 3.0 + (float)i * char_width, rect.pos.y + 3.0,
						/**/ c, NULL);
						i += 1;
					}
				}
			}
			continue;
		}
		/// DEFINE CHARACTER COLOR
		if (IS_MODE(c[0]))
			nvgFillColor(args.vg, colors[10]);	// MODE
		else if (IS_MODULATOR(c[0]))
			nvgFillColor(args.vg, colors[10]);	// MODULATOR
		else if (c[0] == '(' || c[0] == ')')
			nvgFillColor(args.vg, colors[13]);	// BRACKET
		else if (c[0] == ',')
			nvgFillColor(args.vg, colors[13]);	// COMMA
		else
			nvgFillColor(args.vg, colors[4]);	// VALUE
		/// DRAW CHARACTER
		nvgText(args.vg,
		/**/ rect.pos.x + 3.0 + (float)i * char_width, rect.pos.y + 3.0,
		/**/ c, NULL);
		i += 1;
	}
	/// DRAW CURSOR
	if (this == APP->event->selectedWidget) {
		nvgFillColor(args.vg, colors[0]);
		nvgBeginPath(args.vg);
		nvgRect(args.vg,
		/**/ rect.pos.x + this->cursor * char_width + 2.0,
		/**/ rect.pos.y + 3.0, 2, 12);
		nvgFill(args.vg);
	}
	//nvgScissor(args.vg, RECT_ARGS(rect));
	//nvgResetScissor(args.vg);
	//LedDisplayTextField::drawLayer(args, layer);
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
