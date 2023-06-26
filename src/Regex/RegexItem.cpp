
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

	it = this->sequence.sequence.begin();
	while (index > 0) {
		it = std::next(it);
		index -= 1;
	}
	this->sequence.it = it;
}

int RegexItem::pick(void) {
	int		rand;

	rand = random::uniform() * (float)this->sequence.length;
	this->select(rand);
	return rand;
}

int RegexItem::xpick(int last_picked) {
	int		rand;

	if (this->sequence.length == 1) {
		rand = 0;
	} else if (this->sequence.length == 2) {
		rand = (last_picked == 0) ? 1 : 0;
	} else {
		rand = random::uniform() * (float)this->sequence.length;
		while (rand == last_picked)
			rand = random::uniform() * (float)this->sequence.length;
	}
	this->select(rand);
	return rand;
}

void RegexItem::walk(void) {
	if (this->sequence.length <= 1)
		return;
	if (this->sequence.it == this->sequence.sequence.begin()) {
		this->sequence.it = std::next(this->sequence.it);
	} else if (this->sequence.it == std::prev(this->sequence.sequence.end())) {
		this->sequence.it = std::prev(this->sequence.it);
	} else {
		if (random::uniform() > 0.5)
			this->sequence.it = std::next(this->sequence.it);
		else
			this->sequence.it = std::prev(this->sequence.it);
	}
}

void RegexItem::shuffle(void) {
	list<RegexItem>::iterator	it_left;
	list<RegexItem>::iterator	it_right;
	int							index;
	int							i;

	/// SHUFFLE LIST
	for (i = 0; i < this->sequence.length; ++i) {
		/// SELECT LEFT
		index = random::uniform() * (float)this->sequence.length;
		this->select(index);
		it_left = this->sequence.it;
		/// SELECT RIGHT
		index = random::uniform() * (float)this->sequence.length;
		this->select(index);
		it_right = this->sequence.it;
		/// SWAP LEFT & RIGHT
		if (it_left != it_right)
			std::swap(*it_left, *it_right);
	}
	/// SELECT FIRST ITEM
	this->sequence.it = this->sequence.sequence.begin();
}
