
typedef uint8_t		u8;
typedef int8_t		i8;
typedef uint16_t	u16;
typedef int16_t		i16;
typedef uint32_t	u32;
typedef int32_t		i32;

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
		T**	new_ptr;
		void	**array_ptr;
		int8_t	*array_byte;
		size_t	total_size;
		size_t	i, j;

		if (width == 0 || height == 0)
			return false;
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
