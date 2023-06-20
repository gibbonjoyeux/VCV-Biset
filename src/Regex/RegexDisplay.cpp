
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
	std::shared_ptr<Font>	font;

	LedDisplayTextField::draw(args);

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// DRAW BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[15]);
	this->color = colors[4];
	nvgRoundedRect(args.vg, RECT_ARGS(rect), 5);
	nvgFill(args.vg);
	/// DRAW TEXT
	if (this != ((RegexWidget*)this->moduleWidget)->display[3]
	&& this != ((RegexWidget*)this->moduleWidget)->display[4])
		return;
	//// GET FONT
	font = APP->window->loadFont(this->fontPath);
	if (font == NULL)
		return;
	//// SET FONT
	nvgFontSize(args.vg, 12);
	nvgFontFaceId(args.vg, font->handle);
	nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	//// DRAW FONT
	nvgFillColor(args.vg, colors[4]);
	nvgText(args.vg,
	/**/ rect.pos.x + 3.0,
	/**/ rect.pos.y + 3.0,
	/**/ "            R - G - X           ", NULL);
	nvgText(args.vg,
	/**/ rect.pos.x + 3.0,
	/**/ rect.pos.y + 3.0 + 12.0,
	/**/ "            - E - E -           ", NULL);
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
	if (this->sequence->mode == REGEX_MODE_CLOCK)
		nvgFillColor(args.vg, colors[15]);
	else
		nvgFillColor(args.vg, colors[14]);
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
	int						index;
	int						count;
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
	/// HANDLE OFFSET
	index = 0;
	if (this->condensed) {
		count = 32;
		if (this->cursor >= 32)
			index = this->cursor - 32;
	} else {
		count = 64;
		if (this->cursor >= 64)
			index = ((this->cursor - 32) / 32) * 32;
	}
	i = 0;
	while (i < count && str[index] != 0) {
		c[0] = str[index];
		/// ACTIVE VALUE
		if (i == this->active_value) {
			nvgFillColor(args.vg, colors[3]);
			/// VALUE AS NUMBER
			if (IS_DIGIT(str[index])) {
				while (i < count && IS_DIGIT(str[index])) {
					c[0] = str[index];
					nvgText(args.vg,
					/**/ rect.pos.x + 3.0 + (float)(i % 32) * char_width,
					/**/ rect.pos.y + 3.0 + (float)(i / 32) * 12.0,
					/**/ c, NULL);
					index += 1;
					i += 1;
				}
				continue;
			/// VALUE AS PITCH
			} else if (IS_PITCH(str[index])) {
				nvgText(args.vg,
				/**/ rect.pos.x + 3.0 + (float)i * char_width, rect.pos.y + 3.0,
				/**/ c, NULL);
				index += 1;
				i += 1;
				if (i < count && (str[index] == '#' || str[index] == 'b')) {
					c[0] = str[index];
					nvgText(args.vg,
					/**/ rect.pos.x + 3.0 + (float)(i % 32) * char_width,
					/**/ rect.pos.y + 3.0 + (float)(i / 32) * 12.0,
					/**/ c, NULL);
					index += 1;
					i += 1;
				}
				if (i < count && IS_DIGIT(str[index])) {
					c[0] = str[index];
					nvgText(args.vg,
					/**/ rect.pos.x + 3.0 + (float)(i % 32) * char_width,
					/**/ rect.pos.y + 3.0 + (float)(i / 32) * 12.0,
					/**/ c, NULL);
					index += 1;
					i += 1;
				}
				continue;
			}
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
		/**/ rect.pos.x + 3.0 + (float)(i % 32) * char_width,
		/**/ rect.pos.y + 3.0 + (float)(i / 32) * 12.0,
		/**/ c, NULL);
		index += 1;
		i += 1;
	}
	/// DRAW CURSOR
	if (this == APP->event->selectedWidget) {
		nvgFillColor(args.vg, colors[0]);
		nvgBeginPath(args.vg);
		if (this->condensed) {
			nvgRect(args.vg,
			/**/ rect.pos.x + (float)((this->cursor < 32) ? this->cursor : 32) * char_width + 2.0,
			/**/ rect.pos.y + 3.0,
			/**/ 2, 12);
		} else {
			nvgRect(args.vg,
			/**/ rect.pos.x + (float)(this->cursor % 32) * char_width + 2.0,
			/**/ rect.pos.y + 3.0 + (float)(this->cursor >= 32) * 12.0,
			/**/ 2, 12);
		}
		nvgFill(args.vg);
	}
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
	/// HANDLE ENTER (COMPILE)
	if (e.key == GLFW_KEY_ENTER) {
		if (e.action == GLFW_PRESS) {
			/// UPDATE ALL SEQUENCES
			if (e.mods & GLFW_MOD_CONTROL) {
				for (i = 0; i < this->module->exp_count; ++i)
					this->module->sequences[i].compile(this->module);
			/// UPDATE SEQUENCE
			} else {
				this->sequence->compile(this->module);
			}
		}
		e.consume(this);
		return;
	}
	/// HANDLE ESCAPE (STOP)
	if (e.key == GLFW_KEY_ESCAPE) {
		if (e.action == GLFW_PRESS) {
			/// WAIT FOR THREAD FLAG
			while (this->module->thread_flag.test_and_set()) {}
			/// RESET EXPRESSION
			this->sequence->reset(true);
			/// CLEAR THREAD FLAG
			this->module->thread_flag.clear();
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
			} else if (this->condensed == false) {
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
			} else if (this->condensed == false) {
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
		/// HANDLE VALUE AS NUMBER
		if (IS_DIGIT(str[i])) {
			while (IS_DIGIT(str[i]))
				i += 1;
		/// HANDLE VALUE AS PITCH
		} else if (IS_PITCH(str[i])) {
			i += 1;
			if (str[i] == '#' || str[i] == 'b')
				i += 1;
			if (IS_DIGIT(str[i]))
				i += 1;
		/// HANDLE VALUE AS SEQUENCE (RECURSIVE)
		} else if (IS_MODE(str[i]) || str[i] == '(') {
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
