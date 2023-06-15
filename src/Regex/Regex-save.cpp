
#include "Regex.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

//json_t *Regex::dataToJson(void) {
//	json_t		*j_array;
//	json_t		*j_str;
//	int			i;
//
//	j_array = json_array();
//	for (i = 0; i < 8; ++i) {
//		j_str = json_string(this->sequences[i].display->text.c_str());
//		json_array_append_new(j_array, j_str);
//	}
//	return j_array;
//}
//
//void Regex::dataFromJson(json_t *j_root) {
//	json_t		*j_str;
//	int			i;
//
//	if (json_typeof(j_root) == JSON_ARRAY) {
//		for (i = 0; i < 8; ++i) {
//			j_str = json_array_get(j_root, i);
//			if (j_str && json_typeof(j_str) == JSON_STRING) {
//				this->sequences[i].display->text = json_string_value(j_str);
//			}
//		}
//	}
//}

json_t *Regex::dataToJson(void) {
	json_t		*j_root;
	json_t		*j_array;
	json_t		*j_str;
	int			i;

	j_root = json_object();
	j_array = json_array();
	for (i = 0; i < 8; ++i) {
		j_str = json_string(this->sequences[i].display->text.c_str());
		json_array_append_new(j_array, j_str);
	}
	json_object_set_new(j_root, "expressions", j_array);
	return j_root;
}

void Regex::dataFromJson(json_t *j_root) {
	json_t		*j_array;
	json_t		*j_str;
	int			i;

	j_array = json_object_get(j_root, "expressions");
	if (j_array && json_typeof(j_array) == JSON_ARRAY) {
		for (i = 0; i < 8; ++i) {
			j_str = json_array_get(j_array, i);
			if (j_str && json_typeof(j_str) == JSON_STRING) {
				// TODO: ! ! ! this->sequences[i].display not set yet ! ! !
				// Set on RegexWidget::RegexWidget()
				this->expressions[i] = json_string_value(j_str);
			}
		}
	}
}
