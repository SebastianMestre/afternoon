#include <stdlib.h>
void runner(char*);
int main() {
	int const MB = 1 << 20;
	char* buffer = calloc(1, 10 * MB);
	runner(buffer + 5 * MB);
	free(buffer);
}
