
#include "TrackerDrum.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerDrum::TrackerDrum() {
	int			i, j;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);
	/// CONFIG PARAMETERS
	configParam(PARAM_SYNTH, 0, 63, 0, "Synth")->snapEnabled = true;
	for (i = 0; i < 8; ++i)
		configParam(PARAM_OUT_MIN + i, -10.0, 10.0, 0.0, "Min");
	for (i = 0; i < 8; ++i)
		configParam(PARAM_OUT_MAX + i, -10.0, 10.0, 10.0, "Max");
	/// CONFIG MENU PARAMETERS
	for (i = 0; i < 2; ++i)
		configParam<ParamQuantityLink>(PARAM_MENU + i, 0, 1, 0);
	/// CONFIG OUTPUTS
	for (i = 0; i < 12; ++i) {
		configOutput(OUTPUT_TRIGGER + i, rack::string::f("Trigger %d", i + 1));
		configOutput(OUTPUT_VELOCITY + i, rack::string::f("Velocity %d", i + 1));
		configOutput(OUTPUT_PANNING + i, rack::string::f("Panning %d", i + 1));
	}
	for (i = 0; i < 8; ++i)
		configOutput(OUTPUT_CV + i, rack::string::f("CV %d", i + 1));
	/// CONFIG PARAM HANDLES
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 4; ++j) {
			this->map_handles[i][j].color = nvgRGB(0xff, 0xff, 0x40);
			APP->engine->addParamHandle(&(this->map_handles[i][j]));
		}
	}
	this->map_learn = false;
	this->map_learn_cv = 0;
	this->map_learn_map = 0;
}

TrackerDrum::~TrackerDrum() {
	int			i, j;

	for (i = 0; i < 8; i++)
		for (j = 0; j < 4; ++j)
			APP->engine->removeParamHandle(&(this->map_handles[i][j]));
}

void TrackerDrum::process(const ProcessArgs& args) {
	ParamHandleRange	*handle;
	ParamQuantity		*quantity;
	Synth				*synth;
	float				cv;
	float				cv_min, cv_max;
	int					i, j;

	if (g_module == NULL || g_timeline == NULL)
		return;

	synth = &(g_timeline->synths[(int)this->params[PARAM_SYNTH].getValue()]);
	/// SET OUTPUT SYNTH
	for (i = 0; i < 12; ++i) {
		this->outputs[OUTPUT_TRIGGER + i].setVoltage(synth->out_synth[i * 4 + 1]);
		this->outputs[OUTPUT_VELOCITY + i].setVoltage(synth->out_synth[i * 4 + 2]);
		this->outputs[OUTPUT_PANNING + i].setVoltage(synth->out_synth[i * 4 + 3]);
	}
	/// SET OUTPUT CV
	for (i = 0; i < 8; ++i) {
		/// CV AS VOLTAGE
		if (this->outputs[OUTPUT_CV + i].isConnected()) {
			/// MAP FROM [0:1] TO [MIN:MAX]
			cv_min = this->params[PARAM_OUT_MIN + i].getValue();
			cv_max = this->params[PARAM_OUT_MAX + i].getValue();
			cv = (synth->out_cv[i] * (cv_max - cv_min)) + cv_min;
			/// SET CV
			this->outputs[OUTPUT_CV + i].setVoltage(cv);
		}
		/// CV AS MAPPED PARAM
		for (j = 0; j < 4; ++j) {
			handle = &(this->map_handles[i][j]);
			if (handle->module) {
				quantity = handle->module->getParamQuantity(handle->paramId);
				if (quantity) {
					cv_min = handle->min;
					cv_max = handle->max;
					cv = (synth->out_cv[i] * (cv_max - cv_min)) + cv_min;
					quantity->setValue(cv);
				}
			}
		}
	}
}

void TrackerDrum::learn_enable(int cv, int map) {
	this->map_learn = true;
	this->map_learn_cv = cv;
	this->map_learn_map = map;
}

void TrackerDrum::learn_disable(void) {
	this->map_learn = false;
}

void TrackerDrum::learn_map(i64 module_id, int param_id) {
	ParamHandleRange	*handle;

	handle = &(this->map_handles[this->map_learn_cv][this->map_learn_map]);
	APP->engine->updateParamHandle(handle, module_id, param_id, true);
	handle->init();
	this->map_learn = false;
}

Model* modelTrackerDrum = createModel<TrackerDrum, TrackerDrumWidget>("Biset-Tracker-Drum");
