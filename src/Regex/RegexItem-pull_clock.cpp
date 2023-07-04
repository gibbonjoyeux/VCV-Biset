
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

bool RegexItem::pull_clock_foreward(int &value, int &index, float bias) {
	list<RegexItem>::iterator	it_end;
	bool						state;
	int							step;

	it_end = this->sequence.sequence.end();
	/// INIT SEQUENCE
	if (this->sequence.it == it_end)
		this->sequence.it = this->sequence.sequence.begin();
	/// PULL SEQUENCE
	state = this->sequence.it->pull_clock(value, index, bias);
	/// TYPE MODULO
	if (this->sequence.modulator_mode == '%') {
		step = (value > 0) ? value : 1;
		if (this->sequence.state_a + step >= this->sequence.modulator_value) {
			if (value > 0)
				value = this->sequence.modulator_value - this->sequence.state_a;
			else
				value = 0;
			this->sequence.it = this->sequence.sequence.begin();
			this->sequence.state_a = 0;
			return true;
		} else {
			this->sequence.state_a += step;
		}
	}
	/// SEQUENCE NEXT
	if (state == true) {
		this->sequence.it = std::next(this->sequence.it);
		/// SEQUENCE END
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
			} else if (this->sequence.modulator_mode == 0) {
				return true;
			}
		}
	}
	return false;
}

bool RegexItem::pull_clock_backward(int &value, int &index, float bias) {
	list<RegexItem>::iterator	it_end;
	bool						state;
	int							step;

	it_end = this->sequence.sequence.end();
	/// INIT SEQUENCE
	if (this->sequence.it == it_end)
		this->sequence.it = std::prev(this->sequence.sequence.end());
	/// PULL SEQUENCE
	state = this->sequence.it->pull_clock(value, index, bias);
	/// TYPE MODULO
	if (this->sequence.modulator_mode == '%') {
		step = (value > 0) ? value : 1;
		if (this->sequence.state_a + step >= this->sequence.modulator_value) {
			if (value > 0)
				value = this->sequence.modulator_value - this->sequence.state_a;
			else
				value = 0;
			this->sequence.it = std::prev(this->sequence.sequence.end());
			this->sequence.state_a = 0;
			return true;
		} else {
			this->sequence.state_a += step;
		}
	}
	/// SEQUENCE NEXT
	if (state == true) {
		this->sequence.it = std::prev(this->sequence.it);
		/// SEQUENCE END
		if (this->sequence.it == it_end) {
			this->sequence.it = std::prev(this->sequence.sequence.end());
			/// TYPE MULT
			if (this->sequence.modulator_mode == 'x') {
				this->sequence.state_a += 1;
				if (this->sequence.state_a >= this->sequence.modulator_value) {
					this->sequence.state_a = 0;
					return true;
				}
			/// TYPE OFF
			} else if (this->sequence.modulator_mode == 0) {
				return true;
			}
		}
	}
	return false;
}

bool RegexItem::pull_clock_pingpong(int &value, int &index, float bias) {
	list<RegexItem>::iterator	it_end;
	bool						state;
	int							step;

	it_end = this->sequence.sequence.end();
	/// INIT SEQUENCE
	if (this->sequence.it == it_end) {
		this->sequence.it = this->sequence.sequence.begin();
		this->sequence.state_b = 0;
	}
	/// PULL SEQUENCE
	state = this->sequence.it->pull_clock(value, index, bias);
	/// TYPE MODULO
	if (this->sequence.modulator_mode == '%') {
		step = (value > 0) ? value : 1;
		if (this->sequence.state_a + step >= this->sequence.modulator_value) {
			if (value > 0)
				value = this->sequence.modulator_value - this->sequence.state_a;
			else
				value = 0;
			this->sequence.it = this->sequence.sequence.begin();
			this->sequence.state_a = 0;
			this->sequence.state_b = 0;
			return true;
		} else {
			this->sequence.state_a += step;
		}
	}
	/// SEQUENCE NEXT
	if (state == true) {
		if (this->sequence.state_b == 0)
			this->sequence.it = std::next(this->sequence.it);
		else
			this->sequence.it = std::prev(this->sequence.it);
		/// SEQUENCE END
		if (this->sequence.it == it_end) {
			/// GO BACKWARD
			if (this->sequence.state_b == 0) {
				this->sequence.it = std::prev(this->sequence.sequence.end());
				this->sequence.state_b = 1;
			/// GO FOREWARD (END)
			} else {
				this->sequence.it = this->sequence.sequence.begin();
				this->sequence.state_b = 0;
				/// TYPE MULT
				if (this->sequence.modulator_mode == 'x') {
					this->sequence.state_a += 1;
					if (this->sequence.state_a >= this->sequence.modulator_value) {
						this->sequence.state_a = 0;
						return true;
					}
				/// TYPE OFF
				} else if (this->sequence.modulator_mode == 0) {
					return true;
				}
			}
		}
	}
	return false;
}

bool RegexItem::pull_clock_shuffle(int &value, int &index, float bias) {
	list<RegexItem>::iterator	it_end;
	bool						state;
	int							step;

	it_end = this->sequence.sequence.end();
	/// INIT SEQUENCE
	if (this->sequence.it == it_end) {
		this->shuffle();
	}
	/// PULL SEQUENCE
	state = this->sequence.it->pull_clock(value, index, bias);
	/// TYPE MODULO
	if (this->sequence.modulator_mode == '%') {
		step = (value > 0) ? value : 1;
		if (this->sequence.state_a + step >= this->sequence.modulator_value) {
			if (value > 0)
				value = this->sequence.modulator_value - this->sequence.state_a;
			else
				value = 0;
			this->shuffle();
			this->sequence.state_a = 0;
			return true;
		} else {
			this->sequence.state_a += step;
		}
	}
	/// SEQUENCE NEXT
	if (state == true) {
		this->sequence.it = std::next(this->sequence.it);
		/// SEQUENCE END
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
			} else if (this->sequence.modulator_mode == 0) {
				this->shuffle();
				return true;
			}
		}
	}
	return false;
}

bool RegexItem::pull_clock_rand(int &value, int &index, float bias) {
	list<RegexItem>::iterator	it_end;
	bool						state;
	int							step;

	/// INIT SEQUENCE
	if (this->sequence.it == this->sequence.sequence.end()) {
		this->pick(bias);
	}
	/// PULL SEQUENCE
	state = this->sequence.it->pull_clock(value, index, bias);
	/// TYPE MODULO
	if (this->sequence.modulator_mode == '%') {
		step = (value > 0) ? value : 1;
		if (this->sequence.state_a + step >= this->sequence.modulator_value) {
			if (value > 0)
				value = this->sequence.modulator_value - this->sequence.state_a;
			else
				value = 0;
			this->pick(bias);
			this->sequence.state_a = 0;
			return true;
		} else {
			this->sequence.state_a += step;
		}
	}
	/// SEQUENCE NEXT
	if (state == true) {
		/// SELECT RANDOM ELEMENT
		this->pick(bias);
		/// TYPE MULT
		if (this->sequence.modulator_mode == 'x') {
			this->sequence.state_a += 1;
			if (this->sequence.state_a
			>= this->sequence.length * this->sequence.modulator_value) {
				this->sequence.state_a = 0;
				return true;
			}
		/// TYPE OFF
		} else if (this->sequence.modulator_mode == 0) {
			this->sequence.state_a += 1;
			if (this->sequence.state_a >= this->sequence.length) {
				this->sequence.state_a = 0;
				return true;
			}
		}
	}
	return false;
}

bool RegexItem::pull_clock_xrand(int &value, int &index, float bias) {
	list<RegexItem>::iterator	it_end;
	bool						state;
	int							step;

	/// INIT SEQUENCE
	if (this->sequence.it == this->sequence.sequence.end()) {
		this->sequence.state_c = this->pick(bias);
	}
	/// PULL SEQUENCE
	state = this->sequence.it->pull_clock(value, index, bias);
	/// TYPE MODULO
	if (this->sequence.modulator_mode == '%') {
		step = (value > 0) ? value : 1;
		if (this->sequence.state_a + step >= this->sequence.modulator_value) {
			if (value > 0)
				value = this->sequence.modulator_value - this->sequence.state_a;
			else
				value = 0;
			this->sequence.state_c = this->xpick(this->sequence.state_c, bias);
			this->sequence.state_a = 0;
			return true;
		} else {
			this->sequence.state_a += step;
		}
	}
	/// SEQUENCE NEXT
	if (state == true) {
		/// SELECT RANDOM ELEMENT
		this->sequence.state_c = this->xpick(this->sequence.state_c, bias);
		/// TYPE MULT
		if (this->sequence.modulator_mode == 'x') {
			this->sequence.state_a += 1;
			if (this->sequence.state_a
			>= this->sequence.length * this->sequence.modulator_value) {
				this->sequence.state_a = 0;
				return true;
			}
		/// TYPE OFF
		} else if (this->sequence.modulator_mode == 0) {
			this->sequence.state_a += 1;
			if (this->sequence.state_a >= this->sequence.length) {
				this->sequence.state_a = 0;
				return true;
			}
		}
	}
	return false;
}

bool RegexItem::pull_clock_walk(int &value, int &index, float bias) {
	list<RegexItem>::iterator	it_end;
	bool						state;
	int							step;

	it_end = this->sequence.sequence.end();
	/// INIT SEQUENCE
	if (this->sequence.it == it_end)
		this->sequence.it = this->sequence.sequence.begin();
	/// PULL SEQUENCE
	state = this->sequence.it->pull_clock(value, index, bias);
	/// TYPE MODULO
	if (this->sequence.modulator_mode == '%') {
		step = (value > 0) ? value : 1;
		if (this->sequence.state_a + step >= this->sequence.modulator_value) {
			if (value > 0)
				value = this->sequence.modulator_value - this->sequence.state_a;
			else
				value = 0;
			this->sequence.it = this->sequence.sequence.begin();
			this->sequence.state_a = 0;
			return true;
		} else {
			this->sequence.state_a += step;
		}
	}
	/// SEQUENCE NEXT
	if (state == true) {
		this->walk(bias);
		/// TYPE MULT
		if (this->sequence.modulator_mode == 'x') {
			this->sequence.state_a += 1;
			if (this->sequence.state_a
			>= this->sequence.modulator_value * this->sequence.length) {
				this->sequence.it = this->sequence.sequence.begin();
				this->sequence.state_a = 0;
				return true;
			}
		/// TYPE OFF
		} else if (this->sequence.modulator_mode == 0) {
			this->sequence.state_a += 1;
			if (this->sequence.state_a >= this->sequence.length) {
				this->sequence.it = this->sequence.sequence.begin();
				this->sequence.state_a = 0;
				return true;
			}
		}
	}
	return false;
}

bool RegexItem::pull_clock(int &value, int &index, float bias) {
	/// ITEM AS VALUE
	if (this->type == REGEX_VALUE) {
		value = this->value.value;
		index = this->value.index;
		return true;
	/// ITEM AS SEQUENCE
	} else {
		/// SEQUENCE FOREWARD
		if (this->sequence.mode == '>') {
			return this->pull_clock_foreward(value, index, bias);
		/// SEQUENCE BACKWARD
		} else if (this->sequence.mode == '<') {
			return this->pull_clock_backward(value, index, bias);
		/// SEQUENCE PING-PONG
		} else if (this->sequence.mode == '^') {
			return this->pull_clock_pingpong(value, index, bias);
		/// SEQUENCE SHUFFLE
		} else if (this->sequence.mode == '@') {
			return this->pull_clock_shuffle(value, index, bias);
		/// SEQUENCE RANDOM
		} else if (this->sequence.mode == '?') {
			return this->pull_clock_rand(value, index, bias);
		/// SEQUENCE X-RANDOM
		} else if (this->sequence.mode == '!') {
			return this->pull_clock_xrand(value, index, bias);
		/// SEQUENCE WALK
		} else if (this->sequence.mode == '$') {
			return this->pull_clock_walk(value, index, bias);
		}
	}
	return false;
}
