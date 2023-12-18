
#include "Acro.hpp"

#define ACRO_GRID_LEFT(grid, y, x)	((x > 0) ? grid[y][x - 1] : 0);
#define ACRO_GRID_RIGHT(grid, y, x)	((x < ACRO_W - 1) ? grid[y][x + 1] : 0);

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

char acro_chars[] = "0123456789"
"abcdefghijklmnopqrstuvwxyz"
"*:!#";

Acro::Acro() {
	int		x, y;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	for (y = 0; y < ACRO_H; ++y)
		for (x = 0; x < ACRO_W; ++x)
			this->reset(x, y);
	this->cur_x = 0;
	this->cur_y = 0;
	this->beat = 0;
}

void Acro::process(const ProcessArgs& args) {
	AcroChar	*c;
	AcroChar	*c_left, *c_right;
	int			value_left, value_right;
	int			value;
	int			x, y;

	if (this->trigger_clock.process(inputs[INPUT_CLOCK].getVoltage())) {

		/// [A] UPDATE BEAT
		this->beat += 1;

		/// [B] RESET FLAGS
		for (y = 0; y < ACRO_H; ++y)
			for (x = 0; x < ACRO_W; ++x)
				this->grid[y][x].flags = ACRO_FLAG_ZERO;

		/// [C] COMPUTE CODE
		for (y = 0; y < ACRO_H; ++y) {
			for (x = 0; x < ACRO_W; ++x) {

				c = this->get(x, y);

				if (c == NULL)
					continue;
			 	if (c->flags & ACRO_FLAG_OWNED)
					continue;
			 	if (c->flags & ACRO_FLAG_PROCESSED)
					continue;
				if (c->value >= ACRO_C_A && c->value <= ACRO_C_Z
				&& c->upper == false && !(c->flags & ACRO_FLAG_BANG))
					continue;

				switch (c->value) {

					/// OPE A - ADD
					case ACRO_C_A :
						c_left = this->get(x - 1, y);
						c_right = this->get(x + 1, y);
						if (c_left && c_left->enabled)
							value_left = c_left->value;
						else
							value_left = 0;
						if (c_right && c_right->enabled)
							value_right = c_right->value;
						else
							value_right = 0;
						this->set(x, y + 1,
						/**/ value_left + value_right,
						/**/ c_right->upper,
						/**/ ACRO_FLAG_OWNED | ACRO_FLAG_OWNED_OUTPUT);
						this->own(x - 1, y);
						this->own(x + 1, y);
					break;

					/// OPE B - SUB
					case ACRO_C_B :
						c_left = this->get(x - 1, y);
						c_right = this->get(x + 1, y);
						if (c_left && c_left->enabled)
							value_left = c_left->value;
						else
							value_left = 0;
						if (c_right && c_right->enabled)
							value_right = c_right->value;
						else
							value_right = 0;
						value = value_left - value_right;
						if (value < 0)
							value = -value;
						this->set(x, y + 1,
						/**/ value,
						/**/ c_right->upper,
						/**/ ACRO_FLAG_OWNED | ACRO_FLAG_OWNED_OUTPUT);
						this->own(x - 1, y);
						this->own(x + 1, y);
					break;

					/// OPE C - CLOCK
					case ACRO_C_C :
						c_left = this->get(x - 1, y);
						c_right = this->get(x + 1, y);
						if (c_left && c_left->enabled)
							value_left = c_left->value;
						else
							value_left = 1;
						if (value_left < 1)
							value_left = 1;
						if (c_right && c_right->enabled)
							value_right = c_right->value;
						else
							value_right = 8;
						if (value_right < 1)
							value_right = 1;
						this->set(x, y + 1,
						/**/ (this->beat / value_left) % value_right,
						/**/ (c_right) ? c_right->upper : false,
						/**/ ACRO_FLAG_OWNED | ACRO_FLAG_OWNED_OUTPUT);
						this->own(x - 1, y);
						this->own(x + 1, y);
					break;

					/// OPE D - DELAY
					case ACRO_C_D :
						c_left = this->get(x - 1, y);
						c_right = this->get(x + 1, y);
						if (c_left && c_left->enabled)
							value_left = c_left->value;
						else
							value_left = 1;
						if (value_left < 1)
							value_left = 1;
						if (c_right && c_right->enabled)
							value_right = c_right->value;
						else
							value_right = 8;
						if (value_right < 1)
							value_right = 1;
						if ((this->beat / value_left) % value_right == 0)
							this->set(x, y + 1, ACRO_C_BANG);
						this->own(x - 1, y);
						this->own(x + 1, y);
					break;

					/// OPE E - EST
					case ACRO_C_E :
						c_right = this->get(x + 1, y);
						if (c_right && c_right->enabled == false) {
							this->set(x + 1, y,
							/**/ ACRO_C_E,
							/**/ c->upper,
							/**/ ACRO_FLAG_PROCESSED);
							this->reset(x, y);
						} else {
							this->set(x, y, ACRO_C_BANG);
						}
					break;

					/// OPE F - IF
					case ACRO_C_F :
						c_left = this->get(x - 1, y);
						c_right = this->get(x + 1, y);
						value = false;
						if (c_left && c_left->enabled) {
							if (c_right && c_right->enabled) {
								if (c_left->upper == c_right->upper
								&& c_left->value == c_right->value)
									value = true;
							}
						} else if (c_right == NULL
						|| c_right->enabled == false) {
							value = true;
						}
						if (value)
							this->set(x, y + 1, ACRO_C_BANG);
						this->own(x - 1, y);
						this->own(x + 1, y);
					break;

					/// OPE * - BANG
					case ACRO_C_BANG :
						this->reset(x, y);
						this->grid[y][x].flags = ACRO_FLAG_BANG;
						this->bang(x + 1, y);
						this->bang(x - 1, y);
						this->bang(x, y + 1);
						this->bang(x, y - 1);
					break;
				}

				//c->flag = ACRO_FLAG_FREE;
			}
		}
	}
}

AcroChar *Acro::get(int x, int y) {
	if (x < 0 || x >= ACRO_W || y < 0 || y >= ACRO_H)
		return NULL;

	return &(this->grid[y][x]);
}

void Acro::set(int x, int y, int value, int upper, int flags) {
	if (x < 0 || x >= ACRO_W || y < 0 || y >= ACRO_H)
		return;

	this->grid[y][x].value = value;
	this->grid[y][x].enabled = true;
	if (upper >= 0)
		this->grid[y][x].upper = upper;
	if (flags >= 0)
		this->grid[y][x].flags = flags;
}

void Acro::reset(int x, int y) {
	if (x < 0 || x >= ACRO_W || y < 0 || y >= ACRO_H)
		return;

	this->grid[y][x].value = 0;
	this->grid[y][x].enabled = false;
	this->grid[y][x].upper = false;
	this->grid[y][x].flags = ACRO_FLAG_ZERO;
}

void Acro::own(int x, int y, int flag) {
	if (x < 0 || x >= ACRO_W || y < 0 || y >= ACRO_H)
		return;

	this->grid[y][x].flags |= ACRO_FLAG_OWNED | flag;
}

void Acro::bang(int x, int y) {
	if (x < 0 || x >= ACRO_W || y < 0 || y >= ACRO_H)
		return;

	if (this->grid[y][x].enabled)
		this->grid[y][x].flags |= ACRO_FLAG_BANG;
}

Model* modelAcro = createModel<Acro, AcroWidget>("Biset-Acro");
