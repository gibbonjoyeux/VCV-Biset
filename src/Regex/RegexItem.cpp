
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static inline int euclidian_rythm(int onsets, int pulses, int beat_aim) {
	float		slope;
	int			current, previous;
	int			beat_count;
	int			len;
	int			i;

    slope = (float)onsets / (float)pulses;
	len = 0;
	beat_count = 0;
	previous = -1;
	for (i = 0; i < pulses; ++i) {
		current = (int)((float)i * slope);
		/// BEAT NEW
		if (current != previous) {
			/// MATCHING BEAT (RETURN LENGTH)
			if (len > 0) {
				// TODO: could store `i` value on `state_b` to restart loop
				// without full computation
				return len;
			}
			/// MATCHING BEAT (INIT LENGTH)
			if (beat_count == beat_aim)
				len = 1;
			beat_count += 1;
		/// BEAT CONTINUE
		} else {
			/// MATCHING BEAT (INCREMENT LENGTH)
			if (len > 0)
				len += 1;
		}
		previous = current;
	}
}

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
	return false;
}

bool RegexItem::pull_clock_xrand(int &value) {
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

void RegexItem::select(int index) {
	list<RegexItem>::iterator	it;
	int							i;

	/// FROM START TO END
	if (index < this->sequence.length / 2) {
		it = this->sequence.sequence.begin();
		i = 0;
		while (i < index) {
			std::next(it);
			i += 1;
		}
		this->sequence.it = it;
	/// FROM END TO START
	} else {
		it = this->sequence.sequence.end();
		i = this->sequence.length;
		while (i > index) {
			std::prev(it);
			i -= 1;
		}
		this->sequence.it = it;
	}
}

void RegexItem::shuffle(void) {
	//std::random_shuffle(
	///**/ this->sequence.sequence.begin(),
	///**/ this->sequence.sequence.end());
}
