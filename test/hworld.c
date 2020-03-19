#include "hworld.h"

int main(void) {
	string_t *string = malloc(15);
	strncpy(string->s, "Hello, World!\n", 15);
	string->len = strlen(string->s);

	printf("%s%u characters\n", string->s, string->len);

	return 0;
}

