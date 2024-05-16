
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static bool draw_list_synth(int index, char **name, int *color, bool *state) {
	Synth			*synth;

	if (index < (int)g_timeline->synth_count) {
		synth = &(g_timeline->synths[index]);
		*name = synth->name;
		*color = synth->color;
		*state = (g_editor->synth == synth);
		return true;
	}
	return false;
}

static bool draw_list_pattern(int index, char **name, int *color, bool *state) {
	PatternSource	*pattern;

	if (index < (int)g_timeline->pattern_count) {
		pattern = &(g_timeline->patterns[index]);
		*name = pattern->name;
		*color = pattern->color;
		*state = (g_editor->pattern == pattern);
		return true;
	}
	return false;
}

static void menu_pattern(PatternSource *pattern) {
	Menu		*menu;
	MenuLabel	*label;
	bool		play_disable;

	play_disable = (g_timeline->play != TIMELINE_MODE_STOP);
	menu = createMenu();

	/// ADD MOVE BUTTONS
	menu->addChild(rack::createMenuItem("Move up", "",
		[=](void) {
			PatternSource		*pattern_a;
			PatternSource		*pattern_b;

			pattern_a = g_editor->pattern;
			pattern_b = &(g_timeline->patterns[g_editor->pattern_id - 1]);
			g_editor->pattern_id -= 1;
			g_timeline->pattern_swap(pattern_a, pattern_b);
			g_editor->pattern = pattern_b;
		},
		!(g_editor->pattern_id > 0)
	));
	menu->addChild(rack::createMenuItem("Move down", "",
		[=](void) {
			PatternSource		*pattern_a;
			PatternSource		*pattern_b;

			pattern_a = g_editor->pattern;
			pattern_b = &(g_timeline->patterns[g_editor->pattern_id + 1]);
			g_editor->pattern_id += 1;
			g_timeline->pattern_swap(pattern_a, pattern_b);
			g_editor->pattern = pattern_b;
		},
		!(g_editor->pattern_id < g_timeline->pattern_count - 1)
	));
	menu->addChild(new MenuSeparator());

	/// ADD LABEL
	label = new MenuLabel();
	label->text = "Edit pattern";
	menu->addChild(label);
	/// ADD COLOR SUB-MENU
	menu->addChild(rack::createSubmenuItem("Color", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Purple", "",
				[=]() { return pattern->color == 0; },
				[=]() { pattern->color = 0; }
			));
			menu->addChild(new MenuCheckItem("Red", "",
				[=]() { return pattern->color == 1; },
				[=]() { pattern->color = 1; }
			));
			menu->addChild(new MenuCheckItem("Orange", "",
				[=]() { return pattern->color == 2; },
				[=]() { pattern->color = 2; }
			));
			menu->addChild(new MenuCheckItem("Yellow", "",
				[=]() { return pattern->color == 3; },
				[=]() { pattern->color = 3; }
			));
			menu->addChild(new MenuCheckItem("Light green", "",
				[=]() { return pattern->color == 4; },
				[=]() { pattern->color = 4; }
			));
			menu->addChild(new MenuCheckItem("Dark green", "",
				[=]() { return pattern->color == 5; },
				[=]() { pattern->color = 5; }
			));
			menu->addChild(new MenuCheckItem("Light blue", "",
				[=]() { return pattern->color == 6; },
				[=]() { pattern->color = 6; }
			));
			menu->addChild(new MenuCheckItem("Dark blue", "",
				[=]() { return pattern->color == 7; },
				[=]() { pattern->color = 7; }
			));
		}
	));
	/// ADD DUPLICATE BUTTON
	menu->addChild(rack::createMenuItem("Duplicate", "",
		[=](void) {
			g_timeline->pattern_dup(pattern);
		}
	));
	/// ADD RENAME BUTTON
	menu->addChild(rack::createMenuItem("Rename", "",
		[=](void) {
			Menu					*menu;
			MenuLabel				*label;
			MenuTextField			*text_field;

			menu = createMenu();
			/// ADD LABEL
			label = new MenuLabel();
			label->text = "Rename pattern";
			menu->addChild(label);
			/// ADD TEXT FIELD
			text_field = new MenuTextField(pattern->name,
				[=](char *name) {
					if (name[0] != 0)
						strncpy(pattern->name, name, 255);
				}, true
			);
			menu->addChild(text_field);
			APP->event->setSelectedWidget(text_field);
			text_field->selectAll();
			/// ADD CONFIRM BUTTON
			menu->addChild(rack::createMenuItem("Rename", "",
				[=](void) {
					std::string	name;

					name = text_field->getText();
					if (name.length() > 0)
						strncpy(pattern->name, name.c_str(), 255);
				}
			));

		}
	));
	/// ADD DELETE BUTTON
	menu->addChild(rack::createMenuItem("Delete", "",
		[=](void) {
			Menu					*menu;
			MenuLabel				*label;

			menu = createMenu();
			/// ADD LABEL
			label = new MenuLabel();
			label->text = "Confirm deletion";
			menu->addChild(label);
			/// ADD DELETE BUTTON
			menu->addChild(rack::createMenuItem("Delete", "",
				[=](void) {
					g_timeline->pattern_del(pattern);
				}
			));
		}, play_disable
	));
	/// ADD PATTERN EDITION MENU
	if (play_disable == false) {
		menu->addChild(new MenuSeparator());
		pattern->context_menu(menu);
	}
}

static void menu_synth(Synth *synth) {
	Menu			*menu;
	MenuLabel		*label;
	bool			play_disable;

	play_disable = (g_timeline->play != TIMELINE_MODE_STOP);
	menu = createMenu();

	/// ADD MOVE BUTTONS
	menu->addChild(rack::createMenuItem("Move up", "",
		[=](void) {
			Synth		*synth_a;
			Synth		*synth_b;

			synth_a = g_editor->synth;
			synth_b = &(g_timeline->synths[g_editor->synth_id - 1]);
			g_editor->synth_id -= 1;
			g_timeline->synth_swap(synth_a, synth_b);
			g_editor->synth = synth_b;
		},
		!(g_editor->synth_id > 0)
	));
	menu->addChild(rack::createMenuItem("Move down", "",
		[=](void) {
			Synth		*synth_a;
			Synth		*synth_b;

			synth_a = g_editor->synth;
			synth_b = &(g_timeline->synths[g_editor->synth_id + 1]);
			g_editor->synth_id += 1;
			g_timeline->synth_swap(synth_a, synth_b);
			g_editor->synth = synth_b;
		},
		!(g_editor->synth_id < g_timeline->synth_count - 1)
	));
	menu->addChild(new MenuSeparator());

	/// ADD LABEL
	label = new MenuLabel();
	label->text = "Edit synth";
	menu->addChild(label);
	/// ADD COLOR SUB-MENU
	menu->addChild(rack::createSubmenuItem("Color", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Purple", "",
				[=]() { return synth->color == 0; },
				[=]() { synth->color = 0; }
			));
			menu->addChild(new MenuCheckItem("Red", "",
				[=]() { return synth->color == 1; },
				[=]() { synth->color = 1; }
			));
			menu->addChild(new MenuCheckItem("Orange", "",
				[=]() { return synth->color == 2; },
				[=]() { synth->color = 2; }
			));
			menu->addChild(new MenuCheckItem("Yellow", "",
				[=]() { return synth->color == 3; },
				[=]() { synth->color = 3; }
			));
			menu->addChild(new MenuCheckItem("Light green", "",
				[=]() { return synth->color == 4; },
				[=]() { synth->color = 4; }
			));
			menu->addChild(new MenuCheckItem("Dark green", "",
				[=]() { return synth->color == 5; },
				[=]() { synth->color = 5; }
			));
			menu->addChild(new MenuCheckItem("Light blue", "",
				[=]() { return synth->color == 6; },
				[=]() { synth->color = 6; }
			));
			menu->addChild(new MenuCheckItem("Dark blue", "",
				[=]() { return synth->color == 7; },
				[=]() { synth->color = 7; }
			));
		}
	));
	/// ADD RENAME BUTTON
	menu->addChild(rack::createMenuItem("Rename", "",
		[=](void) {
			Menu					*menu;
			MenuLabel				*label;
			MenuTextField			*text_field;

			menu = createMenu();
			/// ADD LABEL
			label = new MenuLabel();
			label->text = "Rename synth";
			menu->addChild(label);
			/// ADD TEXT FIELD
			text_field = new MenuTextField(synth->name + 5,
				[=](char *name) {
					if (name[0] != 0)
						synth->rename(name);
				}, true
			);
			menu->addChild(text_field);
			APP->event->setSelectedWidget(text_field);
			text_field->selectAll();
			/// ADD CONFIRM BUTTON
			menu->addChild(rack::createMenuItem("Rename", "",
				[=](void) {
					std::string	name;

					name = text_field->getText();
					if (name.length() > 0)
						synth->rename((char*)name.c_str());
				}
			));

		}
	));
	/// ADD DELETE BUTTON
	menu->addChild(rack::createMenuItem("Delete", "",
		[=](void) {
			Menu					*menu;
			MenuLabel				*label;

			menu = createMenu();
			/// ADD LABEL
			label = new MenuLabel();
			label->text = "Confirm deletion";
			menu->addChild(label);
			/// ADD DELETE BUTTON
			menu->addChild(rack::createMenuItem("Delete", "",
				[=](void) {
					g_timeline->synth_del(synth);
				}
			));
		}, play_disable
	));
	/// ADD SYNTH EDITION MENU
	if (play_disable == false) {
		menu->addChild(new MenuSeparator());
		synth->context_menu(menu);
	}
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerDisplaySide::TrackerDisplaySide() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerDisplaySide::draw(const DrawArgs &args) {
	Rect					rect;

	LedDisplay::draw(args);

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[0]);
	nvgRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0);
	nvgFill(args.vg);
}

void TrackerDisplaySide::draw_list(const DrawArgs &args, Rect rect,
	int camera_y, std::function<bool(int,char**,int*,bool*)> func) {
	char		*name;
	int			color;
	bool		state;
	float		x, y, w, h;
	Vec			p;
	int			i;

	p = rect.getTopLeft();
	x = p.x;
	w = CHAR_W * 26;
	h = CHAR_H * 3 - 4;
	/// GET ITEM NAME & COLOR & STATUS ?
	for (i = 0; i < 13; ++i) {
		y = p.y + CHAR_H * 3 * i + 6;
		/// DRAW ITEM
		if (func(i + camera_y, &name, &color, &state) == true) {
			/// FILL
			//// MAIN ROUND RECT
			nvgBeginPath(args.vg);
			if (state == true
			|| (g_editor->side_mouse_pos.y > y
			&& g_editor->side_mouse_pos.y < y + h))
				nvgFillColor(args.vg, colors[13]);
			else
				nvgFillColor(args.vg, colors[14]);
			nvgRoundedRect(args.vg, x, y, w - 10, h, 5);
			nvgFill(args.vg);
			//// COLORED ROUND RECT
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, colors_user[color]);
			nvgRoundedRect(args.vg, x + w - 15, y, 15, h, 5);
			nvgFill(args.vg);
			//// COLORED SQUARE RECT
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, colors_user[color]);
			nvgRect(args.vg, x + w - 15, y, 5, h);
			nvgFill(args.vg);
			/// TEXT
			nvgFillColor(args.vg, colors[12]);
			nvgTextBox(args.vg,
			/**/ x + 3.0, y + 3.0,
			/**/ CHAR_W * 23,
			/**/ name, NULL);
		/// DRAW ADD ITEM
		} else {
			/// FILL
			nvgBeginPath(args.vg);
			if (g_editor->side_mouse_pos.y > y && g_editor->side_mouse_pos.y < y + h)
				nvgFillColor(args.vg, colors[3]);
			else
				nvgFillColor(args.vg, colors[2]);
			nvgRoundedRect(args.vg, x, y, w, h, 5);
			nvgFill(args.vg);
			/// TEXT
			nvgFillColor(args.vg, colors[0]);
			nvgTextBox(args.vg,
			/**/ x + w / 2.0 - CHAR_W / 2.0, y + 6.0,
			/**/ CHAR_W * 26,
			/**/ "+", NULL);
			return;
		}
	}
}

void TrackerDisplaySide::drawLayer(const DrawArgs &args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;

	if (g_module != this->module || this->module == NULL || layer != 1)
		return;

	/// GET FONT
	font = APP->window->loadFont(font_path);
	if (font == NULL)
		return;
	/// SET FONT
	nvgFontSize(args.vg, 9);
	nvgFontFaceId(args.vg, font->handle);
	nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	/// GET CANVAS FORMAT
	rect = box.zeroPos();

	nvgScissor(args.vg, RECT_ARGS(rect));
	/// DRAW
	if (g_editor->mode == EDITOR_MODE_PATTERN) {
		this->draw_list(args, rect,
		/**/ g_editor->side_synth_cam_y, draw_list_synth);
	} else if (g_editor->mode == EDITOR_MODE_TIMELINE) {
		this->draw_list(args, rect,
		/**/ g_editor->side_pattern_cam_y, draw_list_pattern);
	}
	nvgResetScissor(args.vg);
	LedDisplay::drawLayer(args, layer);
}

void TrackerDisplaySide::onHover(const HoverEvent &e) {

	if (g_module != this->module)
		return;

	g_editor->side_mouse_pos = e.pos;
	LedDisplay::onHover(e);
	e.consume(this);
}

void TrackerDisplaySide::onButton(const ButtonEvent &e) {
	int		index;

	if (g_module != this->module)
		return;

	g_editor->side_mouse_pos = e.pos;
	g_editor->side_mouse_button = e.button;
	g_editor->side_mouse_action = e.action;
	if (e.action != GLFW_PRESS)
		return;
	e.consume(this);
	if (g_editor->mode == EDITOR_MODE_PATTERN) {
		index = (e.pos.y - 6) / (CHAR_H * 3) + g_editor->side_synth_cam_y;
		/// CLICK ON SYNTH
		if (index < (int)g_timeline->synth_count) {
			/// SELECT SYNTH
			g_editor->set_synth(index);
			/// CLICK RIGHT
			if (e.button == GLFW_MOUSE_BUTTON_RIGHT)
				menu_synth(&(g_timeline->synths[index]));
		/// CLICK ON ADD SYNTH
		} else if (index == (int)g_timeline->synth_count) {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				if (g_editor->side_synth_cam_y == g_timeline->synth_count - 12)
					g_editor->side_synth_cam_y += 1;
				g_timeline->synth_new();
			}
		}
	} else if (g_editor->mode == EDITOR_MODE_TIMELINE) {
		index = (e.pos.y - 6) / (CHAR_H * 3) + g_editor->side_pattern_cam_y;
		/// CLICK ON PATTERN
		if (index < (int)g_timeline->pattern_count) {
			/// SELECT PATTERN
			g_editor->set_pattern(index);
			/// CLICK RIGHT
			if (e.button == GLFW_MOUSE_BUTTON_RIGHT)
				menu_pattern(&(g_timeline->patterns[index]));
		/// CLICK ON ADD PATTERN
		} else if (index == (int)g_timeline->pattern_count) {
			/// CLICK LEFT
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				if (g_editor->side_pattern_cam_y == g_timeline->pattern_count - 12)
					g_editor->side_pattern_cam_y += 1;
				g_timeline->pattern_new();
			}
		}
	}
}

void TrackerDisplaySide::onDoubleClick(const DoubleClickEvent &e) {
	int		index;

	if (g_module != this->module)
		return;

	if (g_editor->mode == EDITOR_MODE_TIMELINE) {
		index = (g_editor->side_mouse_pos.y - 6) / (CHAR_H * 3)
		/**/ + g_editor->side_pattern_cam_y;
		/// CLICK ON PATTERN
		if (index < (int)g_timeline->pattern_count) {
			/// SELECT PATTERN
			g_editor->pattern_id = index;
			g_editor->pattern = &(g_timeline->patterns[index]);
			/// OPEN PATTERN
			g_module->params[Tracker::PARAM_MODE_PATTERN].setValue(1);
			g_module->params[Tracker::PARAM_MODE_TIMELINE].setValue(0);
		}
	}
}

void TrackerDisplaySide::onLeave(const LeaveEvent &e) {

	if (g_module != this->module)
		return;

	g_editor->side_mouse_pos.x = 0;
	g_editor->side_mouse_pos.y = 0;
}

void TrackerDisplaySide::onHoverScroll(const HoverScrollEvent &e) {
	int		*count;
	float	*scroll;

	if (g_module != this->module)
		return;

	/// CONSUME EVENT
	e.consume(this);
	/// POINT CORRESPONDING SCROLL
	if (g_editor->mode == EDITOR_MODE_PATTERN) {
		count = &(g_timeline->synth_count);
		scroll = &(g_editor->side_synth_cam_y);
	} else {
		count = &(g_timeline->pattern_count);
		scroll = &(g_editor->side_pattern_cam_y);
	}
	/// UPDATE CORRESPONDING SCROLL
	*scroll -= e.scrollDelta.y / (CHAR_H * 3.0);
	if (*scroll > *count - 12)
		*scroll = *count - 12;
	if (*scroll < 0)
		*scroll = 0;
}

//void TrackerDisplaySide::onSelectKey(const SelectKeyEvent &e) {
//	Synth			*synth_a;
//	Synth			*synth_b;
//	PatternSource	*pattern_a;
//	PatternSource	*pattern_b;
//
//	if (g_module != this->module)
//		return;
//
//	if ((e.action == GLFW_PRESS || e.action == GLFW_REPEAT)
//	&& (e.key == GLFW_KEY_UP || e.key == GLFW_KEY_DOWN)) {
//		e.consume(this);
//		if (g_timeline->play != TIMELINE_MODE_STOP)
//			return;
//		/// MOVE SYNTH
//		if (g_editor->mode == EDITOR_MODE_PATTERN) {
//			if (g_editor->synth) {
//				synth_a = g_editor->synth;
//				/// MOVE UP
//				if (e.key == GLFW_KEY_UP) {
//					if (g_editor->synth_id <= 0)
//						return;
//					synth_b = &(g_timeline->synths[g_editor->synth_id - 1]);
//					g_editor->synth_id -= 1;
//				/// MOVE DOWN
//				} else {
//					if (g_editor->synth_id >= g_timeline->synth_count - 1)
//						return;
//					synth_b = &(g_timeline->synths[g_editor->synth_id + 1]);
//					g_editor->synth_id += 1;
//				}
//				g_timeline->synth_swap(synth_a, synth_b);
//				/// RE-SELECT MOVED SYNTH
//				g_editor->synth = synth_b;
//			}
//		/// MOVE PATTERN
//		} else if (g_editor->mode == EDITOR_MODE_TIMELINE) {
//			if (g_editor->pattern) {
//				pattern_a = g_editor->pattern;
//				/// MOVE UP
//				if (e.key == GLFW_KEY_UP) {
//					if (g_editor->pattern_id <= 0)
//						return;
//					pattern_b = &(g_timeline->patterns[g_editor->pattern_id - 1]);
//					g_editor->pattern_id -= 1;
//				/// MOVE DOWN
//				} else {
//					if (g_editor->pattern_id >= g_timeline->pattern_count - 1)
//						return;
//					pattern_b = &(g_timeline->patterns[g_editor->pattern_id + 1]);
//					g_editor->pattern_id += 1;
//				}
//				g_timeline->pattern_swap(pattern_a, pattern_b);
//				/// RE-SELECT MOVED SYNTH
//				g_editor->pattern = pattern_b;
//			}
//		}
//	}
//}
