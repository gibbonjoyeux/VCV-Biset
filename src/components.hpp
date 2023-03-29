
#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <rack.hpp>
#include "plugin.hpp"

extern Plugin	*pluginInstance;

////////////////////////////////////////////////////////////////////////////////
/// STRUCTURES
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
/// KNOBS
//////////////////////////////////////////////////

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

//////////////////////////////////////////////////
/// INLET / OUTLET
//////////////////////////////////////////////////

struct Outlet : app::SvgPort {
	Outlet() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/Outlet.svg")));
		shadow->opacity = 0.0;
	}
};

#endif
