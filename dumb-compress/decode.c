#include <stdio.h>

int read_bit(FILE* f) {
	static unsigned char buf = 0;
	static unsigned char pos = 0;
	if (pos == 0) {
		fread(&buf, 1, 1, f);
		pos = 8;
	}
	int result = (buf >> (pos - 1)) & 1;
	pos--;
	return result;
}

unsigned char read_byte(FILE* f) {
	unsigned char result = 0;
	for (int i = 0; i < 8; ++i) {
		result = (result << 1) | read_bit(f);
	}
	return result;
}

int main(int argc, char** argv) {

	unsigned char most_common[256];

	FILE* f = fopen(argv[1], "r");
	fread(most_common, 1, 256, f);

	int data_size;
	fread(&data_size, 4, 1, f);

	unsigned char p;
	fread(&p, 1, 1, f);

	putchar(p);

	for (int i = 1; i < data_size; ++i) {
		int b = read_bit(f);
		if (b == 1) {
			putchar(p = most_common[p]);
		} else {
			putchar(p = read_byte(f));
		}
	}
}
