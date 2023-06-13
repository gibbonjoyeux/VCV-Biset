
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void RegexSeq::process(float dt, bool clock_master) {
	bool			state;
	int				value;
	bool			clock;

	/// SEQUENCE AS CLOCK
	if (this->mode == REGEX_MODE_CLOCK) {
		/// GET CLOCK
		if (this->in->isConnected())
			clock = this->clock_in.process(this->in->getVoltage());
		else
			clock = clock_master;
		/// ON CLOCK TRIGGER
		if (clock) {
			this->clock_out_count += 1;
			if (this->clock_out_count >= this->clock_out_divider) {
				this->clock_out_count = 0;
				if (this->sequence != NULL) {
					state = this->sequence->pull_clock(value);
					/// UPDATE SEQUENCE
					if (state == true && this->sequence_next != NULL) {
						delete this->sequence;
						this->sequence = this->sequence_next;
						this->sequence_next = NULL;
						this->sequence_string = std::move(this->sequence_next_string);
					}
					//this->display_pitch->setText(string::f("%d", value));
					this->clock_out_divider = value;
					if (this->clock_out_divider < 1)
						this->clock_out_divider = 1;
					this->clock_out.trigger();
				}
			}
		}
		this->out->setVoltage(this->clock_out.process(dt) ? 10.0 : 0.0);
	/// SEQUENCE AS PITCH
	} else if (this->mode == REGEX_MODE_PITCH) {
	}
}
