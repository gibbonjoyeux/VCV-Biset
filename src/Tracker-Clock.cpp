
#include "plugin.hpp"

Clock::Clock() {
	this->beat = 0;
	this->phase = 0.0;
}

void Clock::process(float dt) {
	this->phase += dt;
	while (this->phase >= 1.0) {
		this->beat += 1;
		this->phase -= 1.0;
	}
}

void Clock::reset() {
	this->beat = 0;
	this->phase = 0.0;
}
