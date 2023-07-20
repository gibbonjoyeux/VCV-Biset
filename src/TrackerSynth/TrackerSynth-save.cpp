
#include "TrackerSynth.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static i64 json_object_get_integer(json_t *j, const char *key) {
	return json_integer_value(json_object_get(j, key));
}

static double json_object_get_real(json_t *j, const char *key) {
	return json_real_value(json_object_get(j, key));
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

json_t *TrackerSynth::dataToJson(void) {
	ParamHandleRange	*handle;
	json_t				*j_root;
	json_t				*j_array;
	json_t				*j_handle;
	int					i, j;

	j_root = json_object();
	j_array = json_array();
	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 4; ++j) {
			handle = &(this->map_handles[i][j]);
			if (handle->module) {
				j_handle = json_object();
				json_object_set_new(j_handle, "cv", json_integer(i));
				json_object_set_new(j_handle, "map", json_integer(j));
				json_object_set_new(j_handle, "module", json_integer(handle->moduleId));
				json_object_set_new(j_handle, "param", json_integer(handle->paramId));
				json_object_set_new(j_handle, "min", json_real(handle->min));
				json_object_set_new(j_handle, "max", json_real(handle->max));
				json_array_append_new(j_array, j_handle);
			}
		}
	}
	json_object_set_new(j_root, "mapping", j_array);
	return j_root;
}

void TrackerSynth::dataFromJson(json_t *j_root) {
	ParamHandleRange	*handle;
	json_t				*j_array;
	json_t				*j_handle;
	i64					handle_module;
	int					handle_param;
	float				handle_min;
	float				handle_max;
	int					len;
	int					i;

	j_array = json_object_get(j_root, "mapping");
	if (j_array && json_typeof(j_array) == JSON_ARRAY) {
		len = json_array_size(j_array);
		for (i = 0; i < len; ++i) {
			j_handle = json_array_get(j_array, i);
			if (j_handle && json_typeof(j_handle) == JSON_OBJECT) {
				/// GET HANDLE PARAMETERS
				this->map_learn_cv = json_object_get_integer(j_handle, "cv");
				this->map_learn_map = json_object_get_integer(j_handle, "map");
				handle_module = json_object_get_integer(j_handle, "module");
				handle_param = json_object_get_integer(j_handle, "param");
				handle_min = json_object_get_real(j_handle, "min");
				handle_max = json_object_get_real(j_handle, "max");
				/// SET HANDLE
				this->learn_map(handle_module, handle_param);
				/// SET HANDLE BOUNDARIES
				handle = &(this->map_handles[this->map_learn_cv][this->map_learn_map]);
				handle->min = handle_min;
				handle->max = handle_max;
			}
		}
	}
}
