
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

RegexSeq::RegexSeq(void) {
	this->sequence_rythm = NULL;
	this->sequence_rythm_next = NULL;
	this->sequence_pitch = NULL;
	this->sequence_pitch_next = NULL;
	this->trigger_divider = 1;
	this->trigger_count = 0;
	this->trigger.reset();
}

RegexSeq::~RegexSeq(void) {
	/// FREE SEQUENCE RYTHM
	if (this->sequence_rythm != NULL)
		delete this->sequence_rythm;
	if (this->sequence_rythm_next != NULL)
		delete this->sequence_rythm_next;
	/// FREE SEQUENCE PITCH
	if (this->sequence_pitch != NULL)
		delete this->sequence_pitch;
	if (this->sequence_pitch_next != NULL)
		delete this->sequence_pitch_next;
}

void RegexSeq::process(float dt, bool trigger) {
	bool			state;
	int				value;

	if (trigger) {
		this->trigger_count += 1;
		if (this->trigger_count >= this->trigger_divider) {
			this->trigger_count = 0;
			if (this->sequence_rythm != NULL) {
				state = this->sequence_rythm->pull(value);
				if (state == true && this->sequence_rythm_next != NULL) {
					delete this->sequence_rythm;
					this->sequence_rythm = this->sequence_rythm_next;
					this->sequence_rythm_next = NULL;
				}
				this->display_pitch->setText(string::f("%d", value));
				this->trigger_divider = value;
				if (this->trigger_divider < 1)
					this->trigger_divider = 1;
				this->trigger.trigger();
			}
		}
	}
	this->out_rythm->setVoltage(this->trigger.process(dt) ? 10.0 : 0.0);
}

void RegexSeq::compile_rythm_req(RegexItem *item, char *str, int &i) {
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
		/// HANDLE VALUE AS NUMBER
		if (IS_DIGIT(str[i])) {
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
		/// HANDLE VALUE AS SEQUENCE (RECURSIVE)
		} else if (IS_MODE(str[i])) {
			item->sequence.length += 1;
			item->sequence.sequence.emplace_back();
			item_new = &(item->sequence.sequence.back());
			this->compile_rythm_req(item_new, str, i);
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

void RegexSeq::compile_rythm(void) {
	RegexItem	*sequence;
	char		*str;
	int			i;

	/// [1] HANDLE SEQUENCE RYTHM
	//// COMPILE SEQUENCE
	str = (char*)this->display_rythm->text.c_str();
	i = 0;
	sequence = new RegexItem();
	this->compile_rythm_req(sequence, str, i);
	//// SET SEQUENCE
	if (this->sequence_rythm == NULL) {
		this->sequence_rythm = sequence;
	} else {
		if (this->sequence_rythm_next != NULL)
			delete this->sequence_rythm_next;
		this->sequence_rythm_next = sequence;
	}
}

void RegexSeq::compile(void) {
	this->compile_rythm();
	//this->compile_pitch();
}
