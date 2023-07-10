
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void RegexSeq::compile_req(RegexItem *item, char *str, int &i) {
	RegexItem			*item_new;
	RegexItem			*item_copy;
	bool				brackets;
	bool				negative;
	int					value;
	int					j;

	brackets = false;
	/// INIT ITEM
	item->type = REGEX_SEQUENCE;
	item->sequence.modulator_mode = 0;
	item->sequence.state_a = 0;
	item->sequence.state_b = 0;
	item->sequence.state_c = 0;
	item->sequence.it = item->sequence.sequence.end();
	/// HANDLE MODE (OPTIONNAL)
	if (str[i] == 0)
		return;
	if (IS_MODE(str[i])) {
		item->sequence.mode = str[i];
		i += 1;
	} else {
		item->sequence.mode = '>';
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
		/// HANDLE VALUE AS NUMBER
		if (IS_DIGIT(str[i]) || str[i] == '-') {
			item->sequence.length += 1;
			item->sequence.sequence.emplace_back();
			item_new = &(item->sequence.sequence.back());
			item_new->type = REGEX_VALUE;
			item_new->value.index = i;
			/// HANDLE SIGN
			if (str[i] == '-') {
				negative = true;
				i += 1;
			} else {
				negative = false;
			}
			/// HANDLE VALUE
			value = 0;
			while (IS_DIGIT(str[i])) {
				value = value * 10 + (str[i] - '0');
				i += 1;
			}
			item_new->value.value = (negative) ? -value : value;
			/// HANDLE MULTIPLICATION
			if (IS_MODULATOR(str[i])) {
				i += 1;
				value = 0;
				while (IS_DIGIT(str[i])) {
					value = value * 10 + (str[i] - '0');
					i += 1;
				}
				if (value > 64)
					value = 64;
				for (j = 1; j < value; ++j) {
					item->sequence.sequence.emplace_back();
					item_copy = &(item->sequence.sequence.back());
					item_copy->type = REGEX_VALUE;
					item_copy->value.index = item_new->value.index;
					item_copy->value.value = item_new->value.value;
				}
			}
		/// HANDLE VALUE AS PITCH
		} else if (IS_PITCH(str[i])) {
			item->sequence.length += 1;
			item->sequence.sequence.emplace_back();
			item_new = &(item->sequence.sequence.back());
			item_new->type = REGEX_VALUE;
			item_new->value.index = i;
			/// CHAR 1 (PITCH)
			if (str[i] >= 'a')
				item_new->value.value = table_pitch_midi[str[i] - 'a'];
			else
				item_new->value.value = table_pitch_midi[str[i] - 'A'];
			i += 1;
			/// CHAR 2 (SHARP & FLAT)
			if (str[i] == '#') {
				item_new->value.value += 1;
				i += 1;
			} else if (str[i] == 'b') {
				item_new->value.value -= 1;
				i += 1;
			}
			/// CHAR 3 (OCT)
			if (IS_DIGIT(str[i])) {
				item_new->value.value += ((str[i] - '0') - 4) * 12;
				i += 1;
			}
			/// HANDLE MULTIPLICATION
			if (IS_MODULATOR(str[i])) {
				i += 1;
				value = 0;
				while (IS_DIGIT(str[i])) {
					value = value * 10 + (str[i] - '0');
					i += 1;
				}
				if (value > 64)
					value = 64;
				for (j = 1; j < value; ++j) {
					item->sequence.sequence.emplace_back();
					item_copy = &(item->sequence.sequence.back());
					item_copy->type = REGEX_VALUE;
					item_copy->value.index = item_new->value.index;
					item_copy->value.value = item_new->value.value;
				}
			}
		/// HANDLE VALUE AS SEQUENCE (RECURSIVE)
		} else if (IS_MODE(str[i]) || str[i] == '(') {
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

void RegexSeq::compile(Regex *module) {
	RegexItem	*sequence;
	char		*str;
	int			i;

	if (this->string_syntax == false)
		return;

	/// [1] COMPILE EXPRESSION
	str = (char*)this->string_edit.c_str();
	i = 0;
	sequence = new RegexItem();
	this->compile_req(sequence, str, i);

	/// [2] WAIT FOR THREAD FLAG
	while (module->thread_flag.test_and_set()) {}

	/// [3] SET EXPRESSION
	if (this->sequence == NULL) {
		this->string_run = this->string_edit;
		this->sequence = sequence;
	} else {
		this->string_run_next = this->string_edit;
		if (this->sequence_next != NULL)
			delete this->sequence_next;
		this->sequence_next = sequence;
	}

	/// [4] CLEAR THREAD FLAG
	module->thread_flag.clear();
}
