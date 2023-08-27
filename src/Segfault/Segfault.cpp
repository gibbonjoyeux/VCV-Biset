
#include "Segfault.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Segfault::Segfault(void) {
	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);
	/// CONFIG PARAMS
	configSwitch(PARAM_BYPASS + 0, 0, 1, 1, "C", {"Off", "On"});
	configSwitch(PARAM_BYPASS + 1, 0, 1, 0, "C# / Db", {"Off", "On"});
	configSwitch(PARAM_BYPASS + 2, 0, 1, 1, "D", {"Off", "On"});
	configSwitch(PARAM_BYPASS + 3, 0, 1, 0, "D# / Eb", {"Off", "On"});
	configSwitch(PARAM_BYPASS + 4, 0, 1, 1, "E", {"Off", "On"});
	configSwitch(PARAM_BYPASS + 5, 0, 1, 1, "F", {"Off", "On"});
	configSwitch(PARAM_BYPASS + 6, 0, 1, 0, "F# / Gb", {"Off", "On"});
	configSwitch(PARAM_BYPASS + 7, 0, 1, 1, "G", {"Off", "On"});
	configSwitch(PARAM_BYPASS + 8, 0, 1, 0, "G# / Ab", {"Off", "On"});
	configSwitch(PARAM_BYPASS + 9, 0, 1, 1, "A", {"Off", "On"});
	configSwitch(PARAM_BYPASS + 10, 0, 1, 0, "A# / Bb", {"Off", "On"});
	configSwitch(PARAM_BYPASS + 11, 0, 1, 1, "B", {"Off", "On"});
	/// CONFIG INPUTS / OUTPUTS
	configInput(INPUT_PITCH, "Pitch");
	configInput(INPUT_GATE, "Gate");
	configOutput(OUTPUT_GATE, "Gate");
	//// CONFIG OUTPUTS KEYS
	configOutput(OUTPUT_GATE_KEY + 0, "Gate C");
	configOutput(OUTPUT_GATE_KEY + 1, "Gate C# / Db");
	configOutput(OUTPUT_GATE_KEY + 2, "Gate D");
	configOutput(OUTPUT_GATE_KEY + 3, "Gate D# / Eb");
	configOutput(OUTPUT_GATE_KEY + 4, "Gate E");
	configOutput(OUTPUT_GATE_KEY + 5, "Gate F");
	configOutput(OUTPUT_GATE_KEY + 6, "Gate F# / Gb");
	configOutput(OUTPUT_GATE_KEY + 7, "Gate G");
	configOutput(OUTPUT_GATE_KEY + 8, "Gate G# / Ab");
	configOutput(OUTPUT_GATE_KEY + 9, "Gate A");
	configOutput(OUTPUT_GATE_KEY + 10, "Gate A# / Bb");
	configOutput(OUTPUT_GATE_KEY + 11, "Gate B");
}

void Segfault::process(const ProcessArgs& args) {
	float	pitch;
	int		pitch_oct;
	float	gate;
	bool	bypass;
	int		len;
	int		i;

	/// PITCH CONNECTED
	if (this->inputs[INPUT_PITCH].isConnected()) {
		/// GATE CONNECTED
		if (this->inputs[INPUT_GATE].isConnected()) {
			len = this->inputs[INPUT_PITCH].getChannels();
			this->outputs[OUTPUT_GATE].setChannels(len);
			for (i = 0; i < 12; ++i)
				this->outputs[OUTPUT_GATE_KEY + i].setVoltage(0.0);
			for (i = 0; i < len; ++i) {
				/// GET PITCH
				pitch = this->inputs[INPUT_PITCH].getVoltage(i);
				if (pitch >= 0)
					pitch_oct = fmod(pitch, 1.0) * 12.0;
				else
					pitch_oct = (12 + (int)(fmod(pitch, 1.0) * 12.0)) % 12;
				/// GET GATE
				gate = this->inputs[INPUT_GATE].getVoltage(i);
				/// GATE ON
				if (gate > 5.0) {
					/// GET BYPASS
					bypass = this->params[PARAM_BYPASS + pitch_oct].getValue();
					/// SET KEY GATE
					this->outputs[OUTPUT_GATE_KEY + pitch_oct].setVoltage(10.0);
					/// SET GLOBAL GATE
					/// FLOW
					if (bypass == true) {
						this->outputs[OUTPUT_GATE].setVoltage(gate, i);
					/// STOP
					} else {
						this->outputs[OUTPUT_GATE].setVoltage(0.0, i);
					}
				/// GATE OFF
				} else {
					this->outputs[OUTPUT_GATE].setVoltage(0.0, i);
				}
			}
		/// GATE NOT CONNECTED
		} else {
			len = this->inputs[INPUT_PITCH].getChannels();
			for (i = 0; i < 12; ++i)
				this->outputs[OUTPUT_GATE_KEY + i].setVoltage(0.0);
			for (i = 0; i < len; ++i) {
				/// GET PITCH
				pitch = this->inputs[INPUT_PITCH].getVoltage(i);
				if (pitch >= 0)
					pitch_oct = fmod(pitch, 1.0) * 12.0;
				else
					pitch_oct = (12 + (int)(fmod(pitch, 1.0) * 12.0)) % 12;
				/// GET BYPASS
				bypass = this->params[PARAM_BYPASS + pitch_oct].getValue();
				/// SET KEY GATE
				this->outputs[OUTPUT_GATE_KEY + pitch_oct].setVoltage(10.0);
			}
		}
	}
}

//void Segfault::onReset(const ResetEvent &e) {
//	Module::onReset(e);
//}

Model* modelSegfault = createModel<Segfault, SegfaultWidget>("Biset-Segfault");
