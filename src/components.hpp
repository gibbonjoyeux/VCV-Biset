
#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <rack.hpp>
#include "plugin.hpp"

extern Plugin	*pluginInstance;

////////////////////////////////////////////////////////////////////////////////
/// STRUCTURES
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
/// SVG COMPONENTS
//////////////////////////////////////////////////

//////////////////////////////
/// BUTTONS
//////////////////////////////

struct ButtonSwitch : app::SvgSwitch {
	ButtonSwitch() {
		this->momentary = false;
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Switch-Off.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Switch-On.svg")));
	}
};

struct ButtonPlaySong : app::SvgSwitch {
	ButtonPlaySong() {
		this->momentary = true;
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Button-Play-Song.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Button-Play-Song-Press.svg")));
	}
};

struct ButtonPlayPattern : app::SvgSwitch {
	ButtonPlayPattern() {
		this->momentary = true;
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Button-Play-Pattern.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Button-Play-Pattern-Press.svg")));
	}
};

struct ButtonPlaySimple : app::SvgSwitch {
	ButtonPlaySimple() {
		this->momentary = true;
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Button-Play-Simple.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Button-Play-Simple-Press.svg")));
	}
};

struct ButtonStop : app::SvgSwitch {
	ButtonStop() {
		this->momentary = true;
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Button-Stop.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Button-Stop-Press.svg")));
	}
};

struct ButtonMinus : app::SvgSwitch {
	ButtonMinus() {
		this->momentary = true;
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Button-Minus.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Button-Minus-Press.svg")));
	}
};

struct ButtonPlus : app::SvgSwitch {
	ButtonPlus() {
		this->momentary = true;
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Button-Plus.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/Button-Plus-Press.svg")));
	}
};

//////////////////////////////
/// KNOBS
//////////////////////////////

struct FlatKnob : app::SvgKnob {
	widget::SvgWidget* bg;

	FlatKnob() {
		minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;

		bg = new widget::SvgWidget;
		fb->addChildBelow(bg, tw);
		speed = 2.f;
		shadow->opacity = 0.f;
	}
};

struct KnobSmall : FlatKnob {
	KnobSmall() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/Knob-Small.svg")));
		bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/Knob-Small-bg.svg")));
	}
};

struct KnobMedium : FlatKnob {
	KnobMedium() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/Knob-Medium.svg")));
		bg->setSvg(Svg::load(asset::plugin(pluginInstance, "res/Knob-Medium-bg.svg")));
	}
};

struct ParamQuantityOptions : ParamQuantity {
	vector<std::string>	options;

	std::string getDisplayValueString() override {
		return options[(int)getValue()];
	}
};

//////////////////////////////
/// INLET / OUTLET
//////////////////////////////

struct Outlet : app::SvgPort {
	Outlet() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/Outlet.svg")));
		shadow->opacity = 0.0;
	}
};

//////////////////////////////////////////////////
/// MENU COMPONENTS
//////////////////////////////////////////////////

//struct MenuSliderQuantity : Quantity {
//	float		*ptr;
//	float		def;
//	float		min;
//	float		max;
//	std::string	label;
//
//	MenuSliderQuantity(float *ptr, std::string label, float def, float min, float max) {
//		*ptr = def;
//		this->ptr = ptr;
//		this->def = def;
//		this->min = min;
//		this->max = max;
//		this->label = label;
//	}
//
//	void setValue(float value) override {
//		if (value < this->min)
//			value = this->min;
//		if (value > this->max)
//			value = this->max;
//		*(this->ptr) = value; 
//	}
//	float getValue(void) override {
//		return *(this->ptr);
//	}
//	float getDisplayValue(void) override {
//		//return (float)(int)*(this->ptr);
//		return *(this->ptr);
//	}
//	float getMinValue() override { return this->min; }
//	float getMaxValue() override { return this->max; }
//	float getDefaultValue() override {return this->def; }
//	std::string getLabel() override { return label; }
//	std::string getUnit() override { return " "; }
//};

/// Menu text field
struct MenuTextField : ui::TextField {
	std::function<void(char*)>	func;
	bool						close;

	MenuTextField(char *str, bool close = false) {
		this->box.size = Vec(20, 20);
		this->multiline = false;
		this->setText(str);
		this->func = NULL;
		this->close = close;
	}

	MenuTextField(char *str, std::function<void(char*)> func, bool close = false) {
		this->box.size = Vec(20, 20);
		this->multiline = false;
		this->setText(str);
		this->func = func;
		this->close = close;
	}

	void onSelectKey(const SelectKeyEvent &e) override {
		if (e.action == GLFW_PRESS
		&& e.key == GLFW_KEY_ENTER) {
			this->func((char*)this->getText().c_str());
			if (this->close == true)
				this->parent->requestDelete();
		} else {
			ui::TextField::onSelectKey(e);
		}
	}
};

/// Menu text field connected to a quantity. Update the quantity when Enter is
/// pressed
struct MenuTextFieldLinked : ui::TextField {
	Quantity	*quantity;

	MenuTextFieldLinked(Quantity *quantity, int precision = 2) {
		char	str[32];

		/// INIT STRING
		sprintf(str, "%.*f", precision, quantity->getValue());
		/// INIT TEXT FIELD
		this->box.size.x = 50;
		this->multiline = false;
		this->setText(str);
		this->quantity = quantity;
	}

	void onSelectKey(const SelectKeyEvent &e) override {
		float	value;

		if (e.action == GLFW_PRESS
		&& e.key == GLFW_KEY_ENTER) {
			value = atof((char*)this->getText().c_str());
			this->quantity->setValue(value);
			this->parent->requestDelete();
		} else {
			ui::TextField::onSelectKey(e);
		}
	}
};

/// Menu item that does not quit menu on click
struct MenuItemStay : ui::MenuItem {
	std::function<void()>	action_func;

	MenuItemStay(std::string text_left, std::string text_right,
	std::function< void()> action) {
		this->text = text_left;
		this->rightText = text_right;
		this->action_func = action;
	}

	void onButton(const ButtonEvent &e) override {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT
		&& e.action == GLFW_PRESS) {
			this->action_func();
			e.stopPropagating();
		}
	}
};

/// Menu check item that does not quit menu on click
struct MenuCheckItem : ui::MenuItem {
	std::string				text_right_prefix;
	std::function<void()>	func_action;
	std::function<bool()>	func_checked;

	MenuCheckItem(std::string text_left, std::string text_right,
	std::function<bool()> func_checked, std::function<void()> func_action) {
		this->text = text_left;
		this->text_right_prefix = text_right;
		//this->rightText = CHECKMARK_STRING;
		this->func_action = func_action;
		this->func_checked = func_checked;
	}

	void step() override {
		this->rightText = this->text_right_prefix;
		if (this->func_checked()) {
			if (this->text_right_prefix.empty() == false)
				this->rightText += "  ";
			this->rightText += CHECKMARK_STRING;
		}
		ui::MenuItem::step();
	}

	void onButton(const ButtonEvent &e) override {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT
		&& e.action == GLFW_PRESS) {
			this->func_action();
			e.stopPropagating();
		}
	}
};

/// Menu slider
struct MenuSlider : ui::Slider {

	//MenuSlider(float *value, std::string label, float def, float min, float max) {
	//	this->quantity = new MenuSliderQuantity(value, label, def, min, max);
	//}
	MenuSlider(ParamQuantity *quantity) {
		this->quantity = quantity;
	}

	~MenuSlider() {
		//delete this->quantity;
	}
};

/// Menu slider that can open text filed to edit value manually
struct MenuSliderEdit : rack::Widget {
	MenuItem		*item_button;
	MenuSlider		*item_slider;
	Quantity		*quantity;

	MenuSliderEdit(ParamQuantity *quantity, int precision = 2) {
		this->box.size.x = 200.0f;
		this->box.size.y = 20.0f;
		/// INIT QUANTITY
		this->quantity = quantity;
		/// INIT BUTTON
		//// "▸" //// "▴" //// "▾" //// "◆"
		this->item_button = new MenuItemStay("▾", "",	
			[=]() {
				MenuTextFieldLinked	*text_field;
				MenuLabel			*label;
				Menu				*menu;

				/// CREATE MENU
				menu = createMenu();
				menu->box.size.x = 200;
				/// INIT MENU LABEL
				label = new MenuLabel();
				label->text = "Edit value";
				menu->addChild(label);
				/// INIT MENU TEXT FIELD
				text_field = new MenuTextFieldLinked(this->quantity, precision);
				menu->addChild(text_field);
				/// SELECT MENU TEXT FIELD
				APP->event->setSelectedWidget(text_field);
				text_field->selectAll();
			}
		);
		this->item_button->box.size.x = 20.0f;
		this->item_button->box.size.y = 20.0f;
		this->addChild(this->item_button);
		/// INIT SLIDER
		this->item_slider = new MenuSlider(quantity);
		this->item_slider->box.size.x = 180.f;
		this->item_slider->box.size.y = 20.f;
		this->item_slider->box.pos.x = 20.f;
		this->addChild(this->item_slider);
	}

	~MenuSliderEdit() {
	}
};

struct LedDisplayDigit : LedDisplay {
	Module			*module;
	std::string		font_path;
	Quantity		*value_quant;
	int				*value_link;
	int				value_length;
	NVGcolor		color_back;
	NVGcolor		color_font;
	char			str[8];

	LedDisplayDigit() {
		this->font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
		this->value_quant = NULL;
		this->value_link = NULL;
		this->value_length = 3;
		this->color_back = {1, 1, 1, 1};
		this->color_font = {0, 0, 0, 1};
	}

	void draw(const DrawArgs &args) override {
		Rect					rect;

		LedDisplay::draw(args);
		/// GET CANVAS FORMAT
		rect = box.zeroPos();
		/// BACKGROUND
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, this->color_back);
		nvgRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0);
		nvgFill(args.vg);
	}

	void drawLayer(const DrawArgs& args, int layer) override {
		std::shared_ptr<Font>	font;
		Rect					rect;
		Vec						p;
		int						digit;
		int						value;
		int						i;

		if (module == NULL || layer != 1)
			return;
		/// GET FONT
		font = APP->window->loadFont(font_path);
		if (font == NULL)
			return;
		/// SET FONT
		nvgFontSize(args.vg, 9);
		nvgFontFaceId(args.vg, font->handle);
		/// GET CANVAS FORMAT
		rect = box.zeroPos();
		p = rect.getTopLeft();
		p.y -= 1.0;
		/// GET VALUE
		if (this->value_quant != NULL)
			value = this->value_quant->getValue();
		else if (this->value_link != NULL)
			value = *(this->value_link);
		else
			value = 0;
		/// CONVERT VALUE (TO STRING)
		this->str[this->value_length] = 0;
		for (i = this->value_length - 1; i >= 0; --i) {
			digit = value % 10;
			this->str[i] = '0' + digit;
			value /= 10;
		}
		/// DRAW VALUE (AS STRING)
		nvgFillColor(args.vg, this->color_font);
		nvgText(args.vg, p.x + 2.0, p.y + 9.5, this->str, NULL);
		/// CALL PARENT
		LedDisplay::drawLayer(args, layer);
	}
};

#endif
