
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

json_t *Regex::dataToJson(void) {
	json_t		*j_root;
	json_t		*j_array;
	json_t		*j_str;
	int			i;

	j_root = json_object();
	j_array = json_array();
	for (i = 0; i < this->exp_count; ++i) {
		j_str = json_string(this->sequences[i].string_edit.c_str());
		json_array_append_new(j_array, j_str);
	}
	json_object_set_new(j_root, "expressions", j_array);
	return j_root;
}

void Regex::dataFromJson(json_t *j_root) {
	json_t		*j_array;
	json_t		*j_str;
	char		*str;
	bool		run;
	int			i;

	/// COMPUTE START SWITCH VALUE
	run = (bool)this->params[Regex::PARAM_RUN_START].getValue();
	/// COMPUTE EXPRESSIONS
	j_array = json_object_get(j_root, "expressions");
	if (j_array && json_typeof(j_array) == JSON_ARRAY) {
		for (i = 0; i < this->exp_count; ++i) {
			/// SET EXPRESSION
			j_str = json_array_get(j_array, i);
			str = NULL;
			if (j_str && json_typeof(j_str) == JSON_STRING)
				str = (char*)json_string_value(j_str);
			this->sequences[i].reset(true);
			this->sequences[i].string_edit = (str) ? str : "";
			this->sequences[i].string_run = "";
			this->sequences[i].string_run_next = "";
			if (this->widget)
				this->widget->display[i]->setText((str) ? str : "");
			/// RUN EXPRESSION
			if (run) {
				this->sequences[i].mode =
				/**/ this->params[PARAM_MODE + i].getValue();
				this->sequences[i].check_syntax();
				this->sequences[i].compile(this);
			}

		}
	}

}
