
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Synth::Synth() {
	memset(this->name, 0, 256 + 5);
	strcpy(this->name, "synth");
	this->color = 1;
	this->index = 0;
	this->channel_cur = 0;
	this->channel_count = 1;
}

void Synth::init(void) {
	int			index;
	int			i, j;

	/// TODO: INIT NAME & COLOR ?
	/// INIT INDEX
	index = ((intptr_t)this - (intptr_t)g_timeline->synths) / sizeof(Synth);
	this->index = index;
	/// INIT NAME
	this->rename((char*)"synth");
	/// INIT COLOR
	this->color = 1;
	/// INIT VOICES
	for (i = 0; i < 16; ++i)
		this->voices[i].init(i);
	/// INIT OUTPUTS
	for (i = 0; i < 16; ++i)
		for (j = 0; j < 4; ++j)
			this->out_synth[i * 4 + j] = 0.0;
	for (i = 0; i < 8; ++i)
			this->out_cv[i] = 0.0;
	/// SET CHANNEL COUNT
	this->channel_count = 1;
}

void Synth::process(float dt_sec, float dt_beat) {
	int			i;

	// -> ! ! ! BOTTLENECK ! ! !
	// -> Call only if active ? On de-activation -> set output to 0
	/// COMPUTE VOICES
	//// MODE DRUM
	if (this->mode == SYNTH_MODE_DRUM) {
		for (i = 0; i < 12; ++i)
			this->voices[i].process(dt_sec, dt_beat, this->out_synth);
	//// MODE GATE + TRIGGER
	} else {
		for (i = 0; i < this->channel_count; ++i)
			this->voices[i].process(dt_sec, dt_beat, this->out_synth);
	}
}

void Synth::rename(void) {
	int		index;

	/// UPDATE SYNTH INDEX
	index = ((intptr_t)this - (intptr_t)g_timeline->synths) / sizeof(Synth);
	itoaw(this->name, index, 2);
	this->name[2] = ' ';
}

void Synth::rename(char *name) {
	int		index;

	/// UPDATE SYNTH INDEX
	index = ((intptr_t)this - (intptr_t)g_timeline->synths) / sizeof(Synth);
	itoaw(this->name, index, 2);
	strcpy(this->name + 2, " - ");
	strncpy(this->name + 5, name, 255);
}

SynthVoice* Synth::add(PatternNoteCol *col, PatternNote *note, int lpb,
	int *state) {
	Synth			*synth;
	SynthVoice		*voice;

	synth = &(g_timeline->synths[note->synth]);
	/// MODE DRUM
	if (synth->mode == SYNTH_MODE_DRUM) {
		voice = &(this->voices[note->pitch % 12]);
		*state = voice->start(synth, col, note, lpb);
		if (*state == VOICE_ADD_ADD)
			return voice;
	/// MODE GATE + TRIGGER
	} else {
		voice = &(this->voices[this->channel_cur]);
		*state = voice->start(synth, col, note, lpb);
		if (*state == VOICE_ADD_ADD) {
			this->channel_cur = (this->channel_cur + 1) % this->channel_count;
			return voice;
		}
	}
	return NULL;
}

void Synth::context_menu(Menu *menu) {
	ParamQuantityLink	*quant_channel;
	ParamQuantityLink	*quant_mode;

	/// ADD SYNTH CHANNEL COUNT
	quant_channel = (ParamQuantityLink*)
	/**/ g_module->paramQuantities[Tracker::PARAM_MENU + 0];
	quant_channel->displayOffset = 0;
	quant_channel->minValue = 1;
	quant_channel->maxValue = 16;
	quant_channel->defaultValue = this->channel_count;
	quant_channel->setValue(this->channel_count);
	quant_channel->name = "Synth channels";
	quant_channel->unit = "";
	quant_channel->precision = 0;
	quant_channel->setLink(NULL);
	menu->addChild(new MenuSliderEdit(quant_channel, 0));
	/// ADD SYNTH MODE
	quant_mode = (ParamQuantityLink*)
	/**/ g_module->paramQuantities[Tracker::PARAM_MENU + 1];
	quant_mode->displayOffset = 0;
	quant_mode->minValue = 0;
	quant_mode->maxValue = 2;
	quant_mode->setValue(this->mode);
	quant_mode->setLink(NULL);
	menu->addChild(rack::createSubmenuItem("Mode", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Gate", "",
				[=]() { return quant_mode->getValue() == SYNTH_MODE_GATE; },
				[=]() { quant_mode->setValue(SYNTH_MODE_GATE); }
			));
			menu->addChild(new MenuCheckItem("Trigger", "",
				[=]() { return quant_mode->getValue() == SYNTH_MODE_TRIGGER; },
				[=]() { quant_mode->setValue(SYNTH_MODE_TRIGGER); }
			));
			menu->addChild(new MenuCheckItem("Drum", "",
				[=]() { return quant_mode->getValue() == SYNTH_MODE_DRUM; },
				[=]() { quant_mode->setValue(SYNTH_MODE_DRUM); }
			));
		}
	));
	/// ADD SYNTH UPDATE BUTTON
	menu->addChild(createMenuItem("Update synth", "",
		[=]() {
			/// WAIT FOR THREAD FLAG
			while (g_timeline->thread_flag.test_and_set()) {}

			this->mode = quant_mode->getValue();
			this->channel_count = quant_channel->getValue();

			/// CLEAR THREAD FLAG
			g_timeline->thread_flag.clear();
		}
	));
}
