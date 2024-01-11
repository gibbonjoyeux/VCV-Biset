
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

RegexClockGate::RegexClockGate(void) {
	this->gate = false;
	this->state = false;
	this->remaining = 0.0;
}

bool RegexClockGate::process(float delta) {
	if (remaining > 0.0)
		remaining -= delta;
	this->state = this->gate;
	if (remaining > 0.0)
		this->state = false;
	return this->state;
}

void RegexClockGate::on(void) {
	if (this->gate)
		this->remaining = 1e-3f;
	else
		this->gate = true;
}

void RegexClockGate::off(void) {
	this->gate = false;
	this->state = false;
	this->remaining = 0.0;
}
