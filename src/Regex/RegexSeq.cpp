
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

RegexSeq::RegexSeq(void) {
	this->mode = REGEX_MODE_CLOCK;
	this->sequence = NULL;
	this->sequence_next = NULL;
	this->clock_out_divider = 1;
	this->clock_out_count = 0;
	this->clock_out.reset();
	this->clock_out_eoc.reset();
	this->clock_in_reset.reset();
	this->clock_in_1.reset();
	this->clock_in_2.reset();
	this->string_active_value = -1;
	this->string_syntax = true;
}

RegexSeq::~RegexSeq(void) {
	this->reset(true);
}

void RegexSeq::reset(bool destroy) {
	/// RESET + FREE SEQUENCE
	if (destroy == true) {
		if (this->sequence != NULL)
			delete this->sequence;
		if (this->sequence_next != NULL)
			delete this->sequence_next;
		this->sequence = NULL;
		this->sequence_next = NULL;
	/// RESET SEQUENCE
	} else {
		/// TAKE LAST AVAILABLE SEQUENCE
		if (this->sequence_next != NULL) {
			delete this->sequence;
			this->sequence = this->sequence_next;
			this->sequence_next = NULL;
			this->string_run = std::move(this->string_run_next);
		}
		/// RESET SEQUENCE
		if (this->sequence != NULL) {
			this->sequence->reset();
		}
	}
	/// RESET CLOCK
	this->clock_out_divider = 1;
	this->clock_out_count = 0;
	this->clock_out.reset();
	this->clock_out_eoc.reset();
	this->clock_in_reset.reset();
	this->clock_in_1.reset();
	this->clock_in_2.reset();
}

bool RegexSeq::check_syntax_seq(char *str, int &i) {
	bool				brackets;

	brackets = false;
	/// HANDLE MODE (OPTIONNAL)
	if (str[i] == 0)
		return false;
	if (IS_MODE(str[i]))
		i += 1;
	/// HANDLE BRACKETS
	if (str[i] == 0)
		return false;
	if (str[i] == '(') {
		i += 1;
		brackets = true;
	}
	/// HANDLE SEQUENCE
	if (str[i] == 0)
		return false;
	while (true) {
		/// HANDLE VALUE AS NUMBER
		if (IS_DIGIT(str[i]) || str[i] == '-') {
			if (str[i] == '-') {
				i += 1;
				if (IS_DIGIT(str[i]) == false)
					return false;
			}
			while (IS_DIGIT(str[i]))
				i += 1;
		/// HANDLE VALUE AS PITCH
		} else if (IS_PITCH(str[i])) {
			i += 1;
			if (str[i] == '#' || str[i] == 'b')
				i += 1;
			if (IS_DIGIT(str[i]))
				i += 1;
		/// HANDLE VALUE AS SEQUENCE (RECURSIVE)
		} else if (IS_MODE(str[i]) || str[i] == '(') {
			if (check_syntax_seq(str, i) == false)
				return false;
		/// HANDLE ERROR
		} else {
			return false;
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
				return true;
			}
			return true;
		/// HANDLE MODULATOR
		} else if (IS_MODULATOR(str[i])) {
			break;
		/// HANDLE END
		} else if (str[i] == 0) {
			if (brackets == true)
				return false;
			return true;
		/// HANDLE ERROR
		} else {
			return false;
		}
	}
	/// HANDLE MODULATOR
	i += 1;
	if (IS_DIGIT(str[i])) {
		while (IS_DIGIT(str[i]))
			i += 1;
		return true;
	}
	return false;
}

bool RegexSeq::check_syntax(void) {
	char							*str;
	int								i;

	str = (char*)this->string_edit.c_str();
	i = 0;
	this->string_syntax = this->check_syntax_seq(str, i);
	return this->string_syntax;
}
