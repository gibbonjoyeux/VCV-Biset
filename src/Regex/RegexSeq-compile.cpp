
#include "Regex.hpp"

static void compile_value_modulator(RegexItem *item, char *str, int &i, int value, int index);
static void compile_number(RegexItem *item, char *str, int &i);
static void compile_pitch(RegexItem *item, char *str, int &i);
static void compile_sequence(RegexItem *item, char *str, int &i);
static void compile_req(RegexItem *item, char *str, int &i);

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static void compile_value_modulator(
	RegexItem		*item,
	char			*str,
	int				&i,
	int				item_value,
	int				item_index) {
	RegexItem		*item_new;
	RegexItem		*item_child;
	int				modulator;
	int				count;
	int				type;

	/// VALUE AS SEQUENCE
	if (str[i] == '%' || str[i] == '*') {
		/// CREATE NEW ITEM
		item->sequence.length += 1;
		item->sequence.sequence.emplace_back();
		item_new = &(item->sequence.sequence.back());
		/// GET NEW ITEM MODULATION
		type = str[i];
		i += 1;
		modulator = 0;
		while (IS_DIGIT(str[i])) {
			modulator = modulator * 10 + (str[i] - '0');
			i += 1;
		}
		/// INIT NEW ITEM
		item_new->type = REGEX_SEQUENCE;
		item_new->sequence.mode = '>';
		item_new->sequence.modulator_mode = type;
		item_new->sequence.modulator_value = modulator;
		item_new->sequence.state_a = 0;
		item_new->sequence.state_b = 0;
		item_new->sequence.state_c = 0;
		item_new->sequence.it = item_new->sequence.sequence.end();
		/// ADD NEW ITEM CHILD
		item_new->sequence.length = 1;
		item_new->sequence.sequence.emplace_back();
		item_child = &(item_new->sequence.sequence.back());
		item_child->type = REGEX_VALUE;
		item_child->value.value = item_value;
		item_child->value.index = item_index;
	/// VALUE AS INLINED SEQUENCE
	} else if (str[i] == 'x') {
		/// GET NEW ITEM MODULATION
		type = str[i];
		i += 1;
		modulator = 0;
		while (IS_DIGIT(str[i])) {
			modulator = modulator * 10 + (str[i] - '0');
			i += 1;
		}
		/// INSERT ITEM N TIMES
		for (count = 0; count < modulator; ++count) {
			/// CREATE NEW ITEM
			item->sequence.length += 1;
			item->sequence.sequence.emplace_back();
			item_new = &(item->sequence.sequence.back());
			/// INIT FIRST ITEM
			item_new->type = REGEX_VALUE;
			item_new->value.value = item_value;
			item_new->value.index = item_index;
		}
	/// VALUE AS SINGLE VALUE
	} else {
		/// CREATE NEW ITEM
		item->sequence.length += 1;
		item->sequence.sequence.emplace_back();
		item_new = &(item->sequence.sequence.back());
		/// INIT NEW ITEM
		item_new->type = REGEX_VALUE;
		item_new->value.value = item_value;
		item_new->value.index = item_index;
	}
}

static void compile_number(RegexItem *item, char *str, int &i) {
	int				item_value;
	int				item_index;
	bool			negative;

	item_index = i;
	/// HANDLE SIGN
	if (str[i] == '-') {
		negative = true;
		i += 1;
	} else {
		negative = false;
	}
	/// HANDLE VALUE
	item_value = 0;
	while (IS_DIGIT(str[i])) {
		item_value = item_value * 10 + (str[i] - '0');
		i += 1;
	}
	item_value = (negative) ? -item_value : item_value;
	/// HANDLE FINALIZATION / MULTIPLICATION
	compile_value_modulator(item, str, i, item_value, item_index);
}

static void compile_pitch(RegexItem *item, char *str, int &i) {
	int				item_value;
	int				item_index;

	item_index = i;
	/// CHAR 1 (PITCH)
	if (str[i] >= 'a')
		item_value = table_pitch_midi[str[i] - 'a'];
	else
		item_value = table_pitch_midi[str[i] - 'A'];
	i += 1;
	/// CHAR 2 (SHARP & FLAT)
	if (str[i] == '#') {
		item_value += 1;
		i += 1;
	} else if (str[i] == 'b') {
		item_value -= 1;
		i += 1;
	}
	/// CHAR 3 (OCTAVE)
	if (IS_DIGIT(str[i])) {
		item_value += ((str[i] - '0') - 4) * 12;
		i += 1;
	}
	/// HANDLE FINALIZATION / MULTIPLICATION
	compile_value_modulator(item, str, i, item_value, item_index);
}

static void compile_sequence(RegexItem *item, char *str, int &i) {
	RegexItem		*item_new;

	item->sequence.length += 1;
	item->sequence.sequence.emplace_back();
	item_new = &(item->sequence.sequence.back());
	compile_req(item_new, str, i);
}

static void compile_req(RegexItem *item, char *str, int &i) {
	bool				brackets;
	int					value;

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
			compile_number(item, str, i);
		/// HANDLE VALUE AS PITCH
		} else if (IS_PITCH(str[i])) {
			compile_pitch(item, str, i);
		/// HANDLE VALUE AS SEQUENCE (RECURSIVE)
		} else if (IS_MODE(str[i]) || str[i] == '(') {
			compile_sequence(item, str, i);
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

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

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
	compile_req(sequence, str, i);

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
