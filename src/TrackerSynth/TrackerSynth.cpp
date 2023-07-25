
#include "TrackerSynth.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerSynth::TrackerSynth() {
	int			i, j;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);
	/// CONFIG PARAMETERS
	configParam(PARAM_SYNTH, 0, 99, 0, "Synth")->snapEnabled = true;
	for (i = 0; i < 8; ++i)
		configParam(PARAM_OUT_MIN + i, -10.0, 10.0, 0.0, "Min");
	for (i = 0; i < 8; ++i)
		configParam(PARAM_OUT_MAX + i, -10.0, 10.0, 10.0, "Max");
	/// CONFIG MENU PARAMETERS
	for (i = 0; i < 2; ++i)
		configParam<ParamQuantityLink>(PARAM_MENU + i, 0, 1, 0);
	/// CONFIG OUTPUTS
	configOutput(OUTPUT_PITCH, "Pitch");
	configOutput(OUTPUT_GATE, "Gate");
	configOutput(OUTPUT_VELOCITY, "Velocity");
	configOutput(OUTPUT_PANNING, "Panning");
	for (i = 0; i < 8; ++i)
		configOutput(OUTPUT_CV + i, string::f("CV %d", i + 1));
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

TrackerSynth::~TrackerSynth() {
	int			i, j;

	for (i = 0; i < 8; i++)
		for (j = 0; j < 4; ++j)
			APP->engine->removeParamHandle(&(this->map_handles[i][j]));
}

void TrackerSynth::process(const ProcessArgs& args) {
	ParamHandleRange	*handle;
	ParamQuantity		*quantity;
	Synth				*synth;
	float				cv;
	float				cv_min, cv_max;
	int					i, j;

	/// GET SYNTH
	i = (int)this->params[PARAM_SYNTH].getValue();
	if (i >= g_timeline.synth_count)
		return;
	synth = &(g_timeline.synths[i]);
	/// SET CHANNEL COUNT
	this->outputs[OUTPUT_PITCH].setChannels(synth->channel_count);
	this->outputs[OUTPUT_GATE].setChannels(synth->channel_count);
	this->outputs[OUTPUT_VELOCITY].setChannels(synth->channel_count);
	this->outputs[OUTPUT_PANNING].setChannels(synth->channel_count);
	/// SET OUTPUT SYNTH
	for (i = 0; i < synth->channel_count; ++i) {
		this->outputs[OUTPUT_PITCH].setVoltage(synth->out_synth[i * 4 + 0], i);
		this->outputs[OUTPUT_GATE].setVoltage(synth->out_synth[i * 4 + 1], i);
		this->outputs[OUTPUT_VELOCITY].setVoltage(synth->out_synth[i * 4 + 2], i);
		this->outputs[OUTPUT_PANNING].setVoltage(synth->out_synth[i * 4 + 3], i);
	}
	/// SET OUTPUT CV
	for (i = 0; i < 8; ++i) {
		if (this->outputs[OUTPUT_CV + i].isConnected()) {
			/// MAP FROM [0:1] TO [MIN:MAX]
			cv_min = this->params[PARAM_OUT_MIN + i].getValue();
			cv_max = this->params[PARAM_OUT_MAX + i].getValue();
			cv = (synth->out_cv[i] * (cv_max - cv_min)) + cv_min;
			/// SET CV
			this->outputs[OUTPUT_CV + i].setVoltage(cv);
			/// SET MAPPED PARAMS
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
}

void TrackerSynth::learn_enable(int cv, int map) {
	this->map_learn = true;
	this->map_learn_cv = cv;
	this->map_learn_map = map;
}

void TrackerSynth::learn_disable(void) {
	this->map_learn = false;
}

void TrackerSynth::learn_map(i64 module_id, int param_id) {
	ParamHandleRange	*handle;

	handle = &(this->map_handles[this->map_learn_cv][this->map_learn_map]);
	APP->engine->updateParamHandle(handle, module_id, param_id, true);
	handle->init();
	this->map_learn = false;
}

Model* modelTrackerSynth = createModel<TrackerSynth, TrackerSynthWidget>("Biset-Tracker-Synth");
