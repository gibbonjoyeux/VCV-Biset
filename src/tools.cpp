
#include "plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void		itoaw(char *str, int number, int width) {
	char	digit;
	int		i;

	str[width] = 0;
	for (i = width - 1; i >= 0; --i) {
		digit = number % 10;
		str[i] = '0' + digit;
		number /= 10;
	}
}

bool		endian_native(void) {
	i32		i;

	i = 1;
	/// LITTLE ENDIAN
	if (*((u8*)&i) == 1)
		return 0;
	/// BIG ENDIAN
	return 1;
}
