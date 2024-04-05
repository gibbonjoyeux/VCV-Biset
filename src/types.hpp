
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

// Allocate 2D arrays with only one malloc
template<typename T> struct Array2D {
	T**			ptr;
	size_t		width;
	size_t		height;

	Array2D() {
		this->ptr = NULL;
		this->width = 0;
		this->height = 0;
	}

	~Array2D() {
		if (this->ptr)
			free(this->ptr);
	}

	bool allocate(size_t height, size_t width) {
		T**		new_ptr;
		void	**array_ptr;
		int8_t	*array_byte;
		size_t	total_size;
		size_t	i, j;

		if (width == 0 || height == 0) {
			if (this->ptr)
				free(this->ptr);
			this->ptr = NULL;
			this->width = width;
			this->height = height;
			return true;
		}
		/// DEFINE ARRAY SIZE
		total_size = (height * sizeof(void*)) + (width * height * sizeof(T));
		if ((total_size - height * sizeof(void*)) != (width * height * sizeof(T)))
			return false;
		/// ALLOC ARRAY
		array_ptr = (void**)malloc(total_size);
		if (array_ptr == NULL)
			return false;
		/// CLEAN ARRAY
		for (i = 0; i < total_size; ++i)
			((u8*)array_ptr)[i] = 0;
		/// CREATE ARRAY DIMENSIONS
		array_byte = (int8_t*)array_ptr + height * sizeof(void*);
		for (i = 0; i < height; ++i)
			array_ptr[i] = array_byte + (i * (width * sizeof(T)));
		new_ptr = (T**)array_ptr;

		/// COPY PREVIOUS ARRAY
		if (this->ptr) {
			/// COPY
			for (i = 0; i < height && i < this->height; ++i)
				for (j = 0; j < width && j < this->width; ++j)
					memcpy(&(new_ptr[i][j]), &(this->ptr[i][j]), sizeof(T));
			/// FREE PREVIOUS
			free(this->ptr);
		}
		/// UPDATE ARRAY
		this->ptr = new_ptr;
		this->width = width;
		this->height = height;
		return true;
	}

	T* operator[](int i) {
		return this->ptr[i];
	}
};

// Allocate 1D array of specific type (usually struct or class) and extend
// each element with an additional memory slot.
//  Can be used to extend struct with an array of size 0 placed at the end of
// the struct. This array, once extended, could be used.
template<typename T> struct ArrayExt {
	T**			ptr;
	size_t		size;
	size_t		extension_slot;

	ArrayExt() {
		this->ptr = NULL;
		this->size = 0;
		this->extension_slot = 0;
	}

	~ArrayExt() {
		if (this->ptr)
			free(this->ptr);
	}

	bool allocate(size_t size, size_t extension_slot) {
		T**		new_ptr;
		void	**array_ptr;
		int8_t	*array_byte;
		size_t	total_size;
		size_t	min_size;
		size_t	i;

		/// DEFINE ARRAY SIZE
		total_size =
		/**/ (size * sizeof(void*))						// Pointer zone
		/**/ + (size * (sizeof(T) + extension_slot));	// Memory zone
		if (total_size == 0) {
			this->ptr = NULL;
			this->size = size;
			this->extension_slot = extension_slot;
			return true;
		}
		/// ALLOC ARRAY
		array_ptr = (void**)malloc(total_size);
		if (array_ptr == NULL)
			return false;
		/// CLEAN ARRAY
		for (i = 0; i < total_size; ++i)
			((u8*)array_ptr)[i] = 0;
		/// CREATE ARRAY
		array_byte = (int8_t*)array_ptr + (size * sizeof(void*));
		for (i = 0; i < size; ++i)
			array_ptr[i] = array_byte + (i * (sizeof(T) + extension_slot));
		new_ptr = (T**)array_ptr;

		/// COPY PREVIOUS ARRAY
		if (this->ptr) {
			/// COMPUTE PREV / CURRENT LOWEST EXTENSION SLOT SIZE
			if (extension_slot > this->extension_slot)
				min_size = this->extension_slot;
			else
				min_size = extension_slot;
			/// COPY PREVIOUS ITEMS CONTENT
			for (i = 0; i < size && i < this->size; ++i)
				memcpy(new_ptr[i], this->ptr[i], sizeof(T) + min_size);
			/// FREE PREVIOUS
			free(this->ptr);
		}
		/// UPDATE ARRAY
		this->ptr = new_ptr;
		this->size = size;
		this->extension_slot = extension_slot;
		return true;
	}

	void delocate(void) {
		if (this->ptr)
			free(this->ptr);
		this->ptr = NULL;
		this->size = 0;
		this->extension_slot = 0;
	}

	T* operator[](int i) {
		if (this->ptr)
			return this->ptr[i];
		return NULL;
	}
};
