
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void RegexSeq::process(float dt, bool clock_master) {
	bool			state;
	bool			clock;
	int				input;
	int				value;
	int				index;

	/// SEQUENCE AS CLOCK
	if (this->mode == REGEX_MODE_CLOCK) {
		/// COMPUTE CLOCK
		if (this->in_1->isConnected())
			clock = this->clock_in_1.process(this->in_1->getVoltage());
		else
			clock = clock_master;
		if (this->in_2->isConnected())
			clock += this->clock_in_2.process(this->in_2->getVoltage());
		/// ON CLOCK
		if (clock) {
			this->clock_out_count += 1;
			if (this->clock_out_count >= this->clock_out_divider) {
				this->clock_out_count = 0;
				if (this->sequence != NULL) {
					value = 0;
					index = -1;
					state = this->sequence->pull_clock(value, index);
					this->display->active_value = index;
					/// UPDATE SEQUENCE
					if (state == true && this->sequence_next != NULL) {
						delete this->sequence;
						this->sequence = this->sequence_next;
						this->sequence_next = NULL;
						this->sequence_string = std::move(this->sequence_next_string);
					}
					this->clock_out_divider = value;
					if (this->clock_out_divider < 1)
						this->clock_out_divider = 1;
					if (value > 0)
						this->clock_out.trigger();
				}
			}
		}
		/// SET VOLTAGE
		this->out->setVoltage(this->clock_out.process(dt) ? 10.0 : 0.0);
	/// SEQUENCE AS PITCH
	} else if (this->mode == REGEX_MODE_PITCH) {
		/// COMPUTE CLOCK
		if (this->in_1->isConnected())
			clock = this->clock_in_1.process(this->in_1->getVoltage());
		else
			clock = clock_master;
		/// ON CLOCK
		if (clock) {
			/// COMPUTE SEQUENCE
			if (this->sequence != NULL) {
				/// COMPUTE INPUT (PITCH OFFSET)
				if (this->in_2->isConnected())
					input = this->in_2->getVoltage() * 12.0;
				else
					input = 0;
				/// COMPUTE SEQUENCE
				value = 0;
				index = -1;
				state = this->sequence->pull_pitch(value, index);
				this->display->active_value = index;
				/// UPDATE SEQUENCE
				if (state == true && this->sequence_next != NULL) {
					delete this->sequence;
					this->sequence = this->sequence_next;
					this->sequence_next = NULL;
					this->sequence_string = std::move(this->sequence_next_string);
				}
				/// SET VOLTAGE
				this->out->setVoltage((float)(value + input) / 12.0);
			}
		}
	}
}
