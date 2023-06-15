
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
	this->reset(true);
}

void RegexSeq::reset(bool destroy) {
	/// RESET + FREE SEQUENCE
	if (destroy == true) {
		if (this->sequence != NULL)
			delete this->sequence;
		if (this->sequence_next != NULL)
			delete this->sequence_next;
		this->sequence = NULL;
		this->sequence_next = NULL;
	/// RESET SEQUENCE
	} else {
		/// TAKE LAST AVAILABLE SEQUENCE
		if (this->sequence_next != NULL) {
			delete this->sequence;
			this->sequence = this->sequence_next;
			this->sequence_next = NULL;
			this->sequence_string = std::move(this->sequence_next_string);
		}
		/// RESET SEQUENCE
		if (this->sequence != NULL) {
			this->sequence->reset();
		}
	}
	/// RESET CLOCK
	this->clock_out_divider = 1;
	this->clock_out_count = 0;
	this->clock_out.reset();
	this->clock_in_1.reset();
	this->clock_in_2.reset();
}
