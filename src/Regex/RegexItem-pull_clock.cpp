
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

bool RegexItem::pull_clock_seq(int &value) {
	list<RegexItem>::iterator	it_end;
	bool						state;

	it_end = this->sequence.sequence.end();
	/// INIT SEQUENCE
	if (this->sequence.it == it_end)
		this->sequence.it = this->sequence.sequence.begin();
	/// PULL SEQUENCE
	state = this->sequence.it->pull_clock(value);
	/// SEQUENCE NEXT
	if (state == true) {
		this->sequence.it = std::next(this->sequence.it);
		/// TYPE MODULO
		if (this->sequence.modulator_mode == '%') {
			if (this->sequence.state_a + value >= this->sequence.modulator_value) {
				value = this->sequence.modulator_value - this->sequence.state_a;
				this->sequence.it = this->sequence.sequence.begin();
				this->sequence.state_a = 0;
				return true;
			} else {
				this->sequence.state_a += value;
			}
		}
		if (this->sequence.it == it_end) {
			this->sequence.it = this->sequence.sequence.begin();
			/// TYPE MULT
			if (this->sequence.modulator_mode == 'x') {
				this->sequence.state_a += 1;
				if (this->sequence.state_a >= this->sequence.modulator_value) {
					this->sequence.state_a = 0;
					return true;
				}
			/// TYPE OFF
			} else {
				return true;
			}
		}
	}
	return false;
}

bool RegexItem::pull_clock_shuffle(int &value) {
	list<RegexItem>::iterator	it_end;
	bool						state;

	it_end = this->sequence.sequence.end();
	/// INIT SEQUENCE
	if (this->sequence.it == it_end) {
		this->shuffle();
		this->sequence.it = this->sequence.sequence.begin();
	}
	/// PULL SEQUENCE
	state = this->sequence.it->pull_clock(value);
	/// SEQUENCE NEXT
	if (state == true) {
		this->sequence.it = std::next(this->sequence.it);
		/// TYPE MODULO
		if (this->sequence.modulator_mode == '%') {
			if (this->sequence.state_a + value >= this->sequence.modulator_value) {
				value = this->sequence.modulator_value - this->sequence.state_a;
				this->shuffle();
				this->sequence.it = this->sequence.sequence.begin();
				this->sequence.state_a = 0;
				return true;
			} else {
				this->sequence.state_a += value;
			}
		}
		if (this->sequence.it == it_end) {
			this->sequence.it = this->sequence.sequence.begin();
			/// TYPE MULT
			if (this->sequence.modulator_mode == 'x') {
				this->sequence.state_a += 1;
				if (this->sequence.state_a >= this->sequence.modulator_value) {
					this->shuffle();
					this->sequence.state_a = 0;
					return true;
				}
			/// TYPE OFF
			} else {
				this->shuffle();
				return true;
			}
		}
	}
	return false;
}

bool RegexItem::pull_clock_rand(int &value) {
	list<RegexItem>::iterator	it_end;
	bool						state;
	int							index;

	/// INIT SEQUENCE
	if (this->sequence.it == this->sequence.sequence.end()) {
		index = random::uniform() * (float)this->sequence.length;
		this->select(index);
	}
	/// PULL SEQUENCE
	state = this->sequence.it->pull_clock(value);
	/// SEQUENCE NEXT
	if (state == true) {
		/// SELECT RANDOM ELEMENT
		index = random::uniform() * (float)this->sequence.length;
		this->select(index);
		/// TYPE MODULO
		if (this->sequence.modulator_mode == '%') {
			if (this->sequence.state_a + value >= this->sequence.modulator_value) {
				value = this->sequence.modulator_value - this->sequence.state_a;
				this->sequence.state_a = 0;
				return true;
			} else {
				this->sequence.state_a += value;
			}
		/// TYPE MULT
		} else if (this->sequence.modulator_mode == 'x') {
			this->sequence.state_a += 1;
			if (this->sequence.state_a >= this->sequence.length) {
				this->sequence.state_a = 0;
				this->sequence.state_b += 1;
				if (this->sequence.state_b >= this->sequence.modulator_value) {
					this->sequence.state_b = 0;
					return true;
				}
			}
		/// TYPE OFF
		} else {
			this->sequence.state_a += 1;
			if (this->sequence.state_a >= this->sequence.length) {
				this->sequence.state_a = 0;
				return true;
			}
		}
	}
	return false;
}

bool RegexItem::pull_clock_xrand(int &value) {
	list<RegexItem>::iterator	it_end;
	bool						state;
	int							index;

	/// INIT SEQUENCE
	if (this->sequence.it == this->sequence.sequence.end()) {
		index = random::uniform() * (float)this->sequence.length;
		this->sequence.state_c = index;
		this->select(index);
	}
	/// PULL SEQUENCE
	state = this->sequence.it->pull_clock(value);
	/// SEQUENCE NEXT
	if (state == true) {
		/// SELECT RANDOM ELEMENT
		if (this->sequence.length == 1) {
			index = 0;
		} else if (this->sequence.length == 2) {
			index = (this->sequence.state_c == 0) ? 1 : 0;
		} else {
			index = random::uniform() * (float)this->sequence.length;
			while (index == this->sequence.state_c)
				index = random::uniform() * (float)this->sequence.length;
		}
		this->sequence.state_c = index;
		this->select(index);
		/// TYPE MODULO
		if (this->sequence.modulator_mode == '%') {
			if (this->sequence.state_a + value >= this->sequence.modulator_value) {
				value = this->sequence.modulator_value - this->sequence.state_a;
				this->sequence.state_a = 0;
				return true;
			} else {
				this->sequence.state_a += value;
			}
		/// TYPE MULT
		} else if (this->sequence.modulator_mode == 'x') {
			this->sequence.state_a += 1;
			if (this->sequence.state_a >= this->sequence.length) {
				this->sequence.state_a = 0;
				this->sequence.state_b += 1;
				if (this->sequence.state_b >= this->sequence.modulator_value) {
					this->sequence.state_b = 0;
					return true;
				}
			}
		/// TYPE OFF
		} else {
			this->sequence.state_a += 1;
			if (this->sequence.state_a >= this->sequence.length) {
				this->sequence.state_a = 0;
				return true;
			}
		}
	}
	return false;
}

bool RegexItem::pull_clock_walk(int &value) {
	list<RegexItem>::iterator	it_end;
	bool						state;

	it_end = this->sequence.sequence.end();
	/// INIT SEQUENCE
	if (this->sequence.it == it_end)
		this->sequence.it = this->sequence.sequence.begin();
	/// PULL SEQUENCE
	state = this->sequence.it->pull_clock(value);
	/// SEQUENCE NEXT
	if (state == true) {
		if (this->sequence.length > 1) {
			if (this->sequence.it == this->sequence.sequence.begin()) {
				this->sequence.it = std::next(this->sequence.it);
			} else if (this->sequence.it == it_end) {
				this->sequence.it = std::prev(this->sequence.it);
			} else {
				if (random::uniform() > 0.5)
					this->sequence.it = std::next(this->sequence.it);
				else
					this->sequence.it = std::prev(this->sequence.it);
			}
		}
		/// TYPE MULT
		if (this->sequence.modulator_mode == 'x') {
			this->sequence.state_a += 1;
			if (this->sequence.state_a
			>= this->sequence.modulator_value * this->sequence.length) {
				this->sequence.state_a = 0;
				return true;
			}
		/// TYPE MODULO
		} else if (this->sequence.modulator_mode == '%') {
			if (this->sequence.state_a + value >= this->sequence.modulator_value) {
				value = this->sequence.modulator_value - this->sequence.state_a;
				this->sequence.state_a = 0;
				return true;
			} else {
				this->sequence.state_a += value;
			}
		/// TYPE OFF
		} else {
			this->sequence.state_a += 1;
			if (this->sequence.state_a >= this->sequence.length) {
				this->sequence.state_a = 0;
				return true;
			}
		}
	}
	return false;
}

bool RegexItem::pull_clock(int &value) {
	/// ITEM AS VALUE
	if (this->type == REGEX_VALUE) {
		value = this->value.value;
		return true;
	/// ITEM AS SEQUENCE
	} else {
		/// SEQUENCE BASIC
		if (this->sequence.mode == '#') {
			return this->pull_clock_seq(value);
		/// SEQUENCE SHUFFLE
		} else if (this->sequence.mode == '@') {
			return this->pull_clock_shuffle(value);
		/// SEQUENCE RANDOM
		} else if (this->sequence.mode == '?') {
			return this->pull_clock_rand(value);
		/// SEQUENCE X-RANDOM
		} else if (this->sequence.mode == '!') {
			return this->pull_clock_xrand(value);
		/// SEQUENCE WALK
		} else if (this->sequence.mode == '$') {
			return this->pull_clock_walk(value);
		}
	}
	return false;
}
