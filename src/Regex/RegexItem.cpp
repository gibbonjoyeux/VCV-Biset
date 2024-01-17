
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

void RegexItem::reset(void) {
	list<RegexItem>::iterator	it;

	/// ITEM AS VALUE
	if (this->type == REGEX_VALUE)
		return;
	/// RESET STATES
	this->sequence.state_a = 0;
	this->sequence.state_b = 0;
	this->sequence.state_c = 0;
	/// RESET CURRENT ITEM
	this->sequence.it = this->sequence.sequence.end();
	/// RESET LIST ITEMS (RECURSIVE)
	it = this->sequence.sequence.begin();
	while (it != this->sequence.it) {
		it->reset();
		it = std::next(it);
	}
}

void RegexItem::select(int index) {
	list<RegexItem>::iterator	it;

	/// CHECK RANGE
	if (index >= this->sequence.length - 1)
		index = this->sequence.length - 1;
	/// SELECT ITEM
	it = this->sequence.sequence.begin();
	while (index > 0) {
		it = std::next(it);
		index -= 1;
	}
	this->sequence.it = it;
}

int RegexItem::pick(float bias) {
	float	range;
	int		index;

	if (bias < 0) {
		range = (float)this->sequence.length * (1.0 + bias);
		index = random::uniform() * range;
	} else {
		range = (float)this->sequence.length * (1.0 - bias);
		index = (float)this->sequence.length - (random::uniform() * range);
	}
	this->select(index);
	return index;
}

int RegexItem::xpick(int last_picked, float bias) {
	float	range;
	float	offset;
	float	index;

	/// COMPUTE RANGE & OFFSET
	if (bias < 0) {
		range = (float)this->sequence.length * (1.0 + bias);
		if (range < 2.0 && this->sequence.length > 1)
			range = 2.0;
		offset = 0;
	} else {
		range = (float)this->sequence.length * (1.0 - bias);
		if (range < 2.0 && this->sequence.length > 1)
			range = 2.0;
		offset = (float)this->sequence.length - range;
	}
	/// COMPUTE RANDOM
	if (range <= 2.0) {
		/// SEQUENCE == 1
		if (this->sequence.length == 1)
			index = 0.0;
		/// SEQUENCE > 1
		else if (last_picked == (int)offset)
			index = 1.0;
		else
			index = 0.0;
	} else {
		index = random::uniform() * range;
		while (last_picked == (int)(offset + index))
			index = random::uniform() * range;
	}
	this->select(offset + index);
	return offset + index;
}

void RegexItem::walk(float bias) {
	float	threshold;

	if (this->sequence.length <= 1)
		return;
	if (this->sequence.it == this->sequence.sequence.begin()) {
		this->sequence.it = std::next(this->sequence.it);
	} else if (this->sequence.it == std::prev(this->sequence.sequence.end())) {
		this->sequence.it = std::prev(this->sequence.it);
	} else {
		threshold = 0.5 - (bias * 0.5);
		if (random::uniform() > threshold)
			this->sequence.it = std::next(this->sequence.it);
		else
			this->sequence.it = std::prev(this->sequence.it);
	}
}

void RegexItem::shuffle(void) {
	/// SHUFFLE LIST
	this->sequence.sequence.sort([](RegexItem &i1, RegexItem &i2) -> bool {
		return random::uniform() > 0.5;
	});
	/// SELECT FIRST ITEM
	this->sequence.it = this->sequence.sequence.begin();
}
