
#include "Acro.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

AcroDisplay::AcroDisplay(void) {
	this->font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void AcroDisplay::draw(const DrawArgs &args) {
	char					line[ACRO_W + 1];
	Rect					rect;
	std::shared_ptr<Font>	font;
	AcroChar				c;
	int						x, y;

	//LedDisplay::draw(args);

	if (this->module == NULL) {
		return;
	}
	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// RECT BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[15]);
	nvgRect(args.vg, RECT_ARGS(rect));
	nvgFill(args.vg);

	/// DRAW CURSOR

	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[14]);
	nvgRect(args.vg,
	/**/ ACRO_CHAR_W * (float)this->module->cur_x,
	/**/ ACRO_CHAR_H * (float)this->module->cur_y,
	/**/ ACRO_CHAR_W, ACRO_CHAR_H);
	nvgFill(args.vg);

	//// GET FONT
	font = APP->window->loadFont(this->font_path);
	if (font == NULL)
		return;
	//// SET FONT
	nvgFontSize(args.vg, 12);
	nvgFontFaceId(args.vg, font->handle);
	nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgFillColor(args.vg, colors[4]);

	for (y = 0; y < ACRO_H; ++y) {
		for (x = 0; x < ACRO_W; ++x) {
			c = this->module->grid[y][x];
			if (c.enabled) {
				line[x] = acro_chars[c.value];
				if (c.upper && c.value >= ACRO_C_A && c.value <= ACRO_C_Z)
					line[x] -= 'a' - 'A';
			} else {
				if (x % 8 == 0 && y % 8 == 0)
					line[x] = '+';
				else if (x % 2 == 0 && y % 2 == 0)
					line[x] = '.';
				else
					line[x] = ' ';
			}
		}
		line[ACRO_W] = 0;
		nvgText(args.vg, 0.0, 0.0 + 12.0 * y, line, NULL);
	}

	// DEBUG
	//char str[64];
	//sprintf(str, "(%llu) x (%llu)", sizeof(AcroChar), sizeof(char));
	//nvgText(args.vg, 10.0, 10.0, str, NULL);
}

void AcroDisplay::drawLayer(const DrawArgs &args, int layer) {
}

void AcroDisplay::onButton(const ButtonEvent &e) {
	if (e.action == GLFW_PRESS) {
		this->module->cur_x = e.pos.x / ACRO_CHAR_W;
		this->module->cur_y = e.pos.y / ACRO_CHAR_H;

		if (this->module->cur_x < 0)
			this->module->cur_x = 0;
		if (this->module->cur_x >= ACRO_W - 1)
			this->module->cur_x = ACRO_W - 1;
		if (this->module->cur_y < 0)
			this->module->cur_y = 0;
		if (this->module->cur_y >= ACRO_H - 1)
			this->module->cur_y = ACRO_H - 1;

		e.consume(this);
	}
}

void AcroDisplay::onSelectKey(const SelectKeyEvent &e) {
	AcroChar		*c;

	if (e.action == GLFW_PRESS || e.action == GLFW_REPEAT) {

		c = this->module->get(this->module->cur_x, this->module->cur_y);

		/// KEY DELETE / BACKSPACE
		if (e.key == GLFW_KEY_DELETE
		|| e.key == GLFW_KEY_BACKSPACE) {
			this->module->reset(this->module->cur_x, this->module->cur_y);
			e.consume(this);

		/// KEY ARROWS
		} else if (e.key == GLFW_KEY_LEFT) {
			this->module->cur_x -= 1;
			if (this->module->cur_x < 0)
				this->module->cur_x = 0;
			e.consume(this);
		} else if (e.key == GLFW_KEY_RIGHT) {
			this->module->cur_x += 1;
			if (this->module->cur_x >= ACRO_W - 1)
				this->module->cur_x = ACRO_W - 1;
			e.consume(this);
		} else if (e.key == GLFW_KEY_UP) {
			this->module->cur_y -= 1;
			if (this->module->cur_y < 0)
				this->module->cur_y = 0;
			e.consume(this);
		} else if (e.key == GLFW_KEY_DOWN) {
			this->module->cur_y += 1;
			if (this->module->cur_y >= ACRO_H - 1)
				this->module->cur_y = ACRO_H - 1;
			e.consume(this);

		/// KEY DIGIT
		} else if (e.keyName[0] >= '0' && e.keyName[0] <= '9') {
			c->enabled = true;
			c->upper = false;
			c->value = e.keyName[0] - '0' + ACRO_C_0;
			e.consume(this);

		/// KEY CHARACTERS
		} else if (e.keyName[0] >= 'a' && e.keyName[0] <= 'z') {
			c->enabled = true;
			c->upper = e.mods & GLFW_MOD_SHIFT;
			c->value = e.keyName[0] - 'a' + ACRO_C_A;
			e.consume(this);
		}
	}
}
