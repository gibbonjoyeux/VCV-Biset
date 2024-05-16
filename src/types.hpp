
typedef uint8_t		byte;
typedef uint8_t		u8;
typedef int8_t		i8;
typedef uint16_t	u16;
typedef int16_t		i16;
typedef uint32_t	u32;
typedef int32_t		i32;
typedef uint64_t	u64;
typedef int64_t		i64;

// Lookup table with interpolation between table cells
// With circular enabled, interpolation is also computed
//   between last and first cells of the array
struct LookupTable {
	float		*array;
	int			length;
	bool		circular;

	LookupTable(void) {
		this->array = NULL;
		this->length = 0;
		this->circular = false;
	}

	~LookupTable(void) {
		if (this->array)
			free(this->array);
	}

	float* init(int length, bool circular = false) {
		if (this->array != NULL) {
			free(this->array);
			this->array = NULL;
			this->length = 0;
			this->circular = false;
		}
		this->array = (float*)malloc(length * sizeof(float));
		if (this->array) {
			this->length = length;
			this->circular = circular;
		}
		return this->array;
	}

	float get(float t) {
		int		index_a;
		int		index_b;
		float	index_inter;

		if (array == NULL)
			return (0);

		/// LIMIT INDEX
		if (t < 0.0)
			t = 0.0;
		if (this->circular) {
			if (t >= this->length)
				t = fmod(t, this->length);
		} else {
			if (t > this->length - 1)
				t = this->length - 1;
		}

		/// COMPUTE INTERPOLATED INDEXES
		index_a = (int)t;
		index_b = index_a + 1;
		if (index_b >= this->length) {
			if (this->circular)
				index_b = 0;
			else
				index_b = index_a;
		}
		index_inter = t - (float)index_a;

		/// COMPUTE INTERPOLATED VALUE
		return (this->array[index_a] * (1.0 - index_inter)
		/**/ + this->array[index_b] * index_inter);
	}
};

// Generates gate with small gapes between adjacent gates
struct GateGenerator {
	bool	state;						// Gate final state
	bool	gate;						// Gate ideal state
	float	remaining;					// Gate remaining delay

	GateGenerator(void) {
		this->gate = false;
		this->state = false;
		this->remaining = 0.0;
	}

	bool process(float delta) {
		if (this->remaining > 0.0)
			this->remaining -= delta;
		this->state = this->gate;
		if (this->remaining > 0.0)
			this->state = false;
		return this->state;
	}

	void on(void) {
		if (this->gate)
			this->remaining = 1e-3f;
		else
			this->gate = true;
	}

	void off(void) {
		this->gate = false;
		this->state = false;
		this->remaining = 1e-3f;
	}
};
