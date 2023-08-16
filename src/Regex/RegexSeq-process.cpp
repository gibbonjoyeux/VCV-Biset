
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void RegexSeq::process(
	float			dt,
	bool			clock_reset_master,
	bool			clock_master,
	float			bias) {
	bool			state;
	bool			clock;
	bool			clock_reset;
	float			input;
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
		clock_reset = clock_reset_master;
		if (this->in_reset->isConnected())
			clock_reset += this->clock_in_reset.process(this->in_reset->getVoltage());
		/// ON CLOCK RESET
		if (clock_reset) {
			this->reset(false);
			/// IF CLOCK WAS ACTIVE LAST TURN : RE-RUN CLOCK TO COMPENSATE RESET
			if (this->clock_in_prev)
				clock = true;
		}
		this->clock_in_prev = false;
		/// ON CLOCK
		if (clock) {
			this->clock_in_prev = true;
			this->clock_out_count += 1;
			if (this->clock_out_count >= this->clock_out_divider) {
				this->clock_out_count = 0;
				/// HANDLE EOC
				if (this->clock_out_eoc_next) {
					this->clock_out_eoc_next = false;
					this->clock_out_eoc.trigger();
					if (this->sequence != NULL && this->sequence_next != NULL) {
						delete this->sequence;
						this->sequence = this->sequence_next;
						this->sequence_next = NULL;
						this->string_run = std::move(this->string_run_next);
					}
				}
				/// HANDLE SEQUENCE
				if (this->sequence != NULL) {
					value = 0;
					index = -1;
					state = this->sequence->pull_clock(value, index, bias);
					this->string_active_value = index;
					/// UPDATE SEQUENCE
					if (state == true)
						this->clock_out_eoc_next = true;
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
		this->out_eoc->setVoltage(this->clock_out_eoc.process(dt) ? 10.0 : 0.0);
	/// SEQUENCE AS PITCH
	} else if (this->mode == REGEX_MODE_PITCH) {
		/// COMPUTE CLOCK
		if (this->in_1->isConnected())
			clock = this->clock_in_1.process(this->in_1->getVoltage());
		else
			clock = clock_master;
		clock_reset = clock_reset_master;
		if (this->in_reset->isConnected())
			clock_reset += this->clock_in_reset.process(this->in_reset->getVoltage());
		/// ON CLOCK RESET
		if (clock_reset) {
			this->reset(false);
			/// IF CLOCK WAS ACTIVE LAST TURN : RE-RUN CLOCK TO COMPENSATE RESET
			if (this->clock_in_prev)
				clock = true;
		}
		this->clock_in_prev = false;
		/// ON CLOCK
		if (clock) {
			this->clock_in_prev = true;
			/// HANDLE EOC
			if (this->clock_out_eoc_next) {
				this->clock_out_eoc_next = false;
				this->clock_out_eoc.trigger();
				if (this->sequence != NULL && this->sequence_next != NULL) {
					delete this->sequence;
					this->sequence = this->sequence_next;
					this->sequence_next = NULL;
					this->string_run = std::move(this->string_run_next);
				}
			}
			/// HANDLE SEQUENCE
			if (this->sequence != NULL) {
				/// COMPUTE INPUT (PITCH OFFSET)
				if (this->in_2->isConnected())
					input = this->in_2->getVoltage() * 12.0;
				else
					input = 0.0;
				/// COMPUTE SEQUENCE
				value = 0;
				index = -1;
				state = this->sequence->pull_pitch(value, index, bias);
				this->string_active_value = index;
				/// UPDATE SEQUENCE
				if (state == true)
					this->clock_out_eoc_next = true;
				/// SET VOLTAGE
				this->out->setVoltage(((float)value + input) / 12.0);
			}
		}
		this->out_eoc->setVoltage(this->clock_out_eoc.process(dt) ? 10.0 : 0.0);
	}
}
