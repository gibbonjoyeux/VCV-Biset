
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void RegexSeq::compile_req(RegexItem *item, char *str, int &i) {
	RegexItem			*item_new;
	bool				brackets;
	int					value;

	brackets = false;
	/// INIT ITEM
	item->type = REGEX_SEQUENCE;
	item->sequence.modulator_mode = 0;
	item->sequence.state_a = 0;
	item->sequence.state_b = 0;
	item->sequence.it = item->sequence.sequence.end();
	/// HANDLE MODE (OPTIONNAL)
	if (str[i] == 0)
		return;
	if (IS_MODE(str[i])) {
		item->sequence.mode = str[i];
		i += 1;
	} else {
		item->sequence.mode = '#';
	}
	/// HANDLE BRACKETS
	if (str[i] == 0)
		return;
	if (str[i] == '(') {
		i += 1;
		brackets = true;
	}
	/// HANDLE SEQUENCE
	if (str[i] == 0)
		return;
	item->sequence.length = 0;
	while (true) {
		/// HANDLE VALUE AS CLOCK VALUE
		if (this->mode == REGEX_MODE_CLOCK && IS_DIGIT(str[i])) {
			item->sequence.length += 1;
			item->sequence.sequence.emplace_back();
			item_new = &(item->sequence.sequence.back());
			item_new->type = REGEX_VALUE;
			item_new->value.index = i;
			value = 0;
			while (IS_DIGIT(str[i])) {
				value = value * 10 + (str[i] - '0');
				i += 1;
			}
			item_new->value.value = value;
		/// HANDLE VALUE AS PITCH VALUE
		} else if (this->mode == REGEX_MODE_PITCH && IS_PITCH(str[i])) {
			item->sequence.length += 1;
			item->sequence.sequence.emplace_back();
			item_new = &(item->sequence.sequence.back());
			item_new->type = REGEX_VALUE;
			item_new->value.index = i;
			if (str[i] >= 'a')
				item_new->value.value = table_pitch_midi[str[i] - 'a'];
			else
				item_new->value.value = table_pitch_midi[str[i] - 'A'];
			if (str[i] == '#') {
				if (item_new->value.value < 11)
					item_new->value.value += 1;
				else
					item_new->value.value = 0;
				i += 1;
			} else if (str[i] == 'b') {
				if (item_new->value.value > 0)
					item_new->value.value -= 1;
				else
					item_new->value.value = 11;
				i += 1;
			}
		/// HANDLE VALUE AS SEQUENCE (RECURSIVE)
		} else if (IS_MODE(str[i])) {
			item->sequence.length += 1;
			item->sequence.sequence.emplace_back();
			item_new = &(item->sequence.sequence.back());
			this->compile_req(item_new, str, i);
		/// HANDLE ERROR
		} else {
			return;
		}
		/// HANDLE SEPARATOR
		if (str[i] == ',') {
			i += 1;
		/// HANDLE SEQUENCE END
		} else if (str[i] == ')') {
			if (brackets == true) {
				i += 1;
				if (IS_MODULATOR(str[i]))
					break;
				return;
			}
			return;
		/// HANDLE MODULATOR
		} else if (IS_MODULATOR(str[i])) {
			break;
		/// HANDLE END
		} else if (str[i] == 0) {
			if (brackets == true)
				return;
			return;
		/// HANDLE ERROR
		} else {
			return;
		}
	}
	/// HANDLE MODULATOR
	item->sequence.modulator_mode = str[i];
	i += 1;
	if (IS_DIGIT(str[i])) {
		value = 0;
		while (IS_DIGIT(str[i])) {
			value = value * 10 + (str[i] - '0');
			i += 1;
		}
		item->sequence.modulator_value = value;
		return;
	}
	return;
}

void RegexSeq::compile(void) {
	RegexItem	*sequence;
	char		*str;
	int			i;

	if (this->display->check_syntax() == false)
		return;
	/// [1] SAVE EXPRESSION
	if (this->sequence == NULL)
		this->sequence_string = this->display->text;
	else
		this->sequence_next_string = this->display->text;
	/// [2] COMPILE EXPRESSION
	str = (char*)this->display->text.c_str();
	i = 0;
	sequence = new RegexItem();
	this->compile_req(sequence, str, i);
	/// [3] SET EXPRESSION
	if (this->sequence == NULL) {
		this->sequence = sequence;
	} else {
		if (this->sequence_next != NULL)
			delete this->sequence_next;
		this->sequence_next = sequence;
	}
}
