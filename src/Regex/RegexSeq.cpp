
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

RegexSeq::RegexSeq(void) {
	this->sequence = NULL;
	this->sequence_next = NULL;
	this->clock_out_divider = 1;
	this->clock_out_count = 0;
	this->clock_out.reset();
}

RegexSeq::~RegexSeq(void) {
	this->reset();
}

void RegexSeq::reset(void) {
	if (this->sequence != NULL)
		delete this->sequence;
	if (this->sequence_next != NULL)
		delete this->sequence_next;
	this->sequence = NULL;
	this->sequence_next = NULL;
	this->clock_out_divider = 1;
	this->clock_out_count = 0;
	this->clock_out.reset();
	this->clock_in.reset();
}