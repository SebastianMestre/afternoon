#include <stdio.h>
#include <assert.h>

void write_bit(int b) {
	assert(b == 0 || b == 1);
	static unsigned char buf = 0;
	static unsigned char pos = 0;
	buf = (buf << 1) | b;
	pos++;
	if (pos == 8) {
		fwrite(&buf, 1, 1, stdout);
		pos = 0;
		buf = 0;
	}
}

void write_byte(unsigned char b) {
	for (int i = 7; i >= 0; --i) {
		write_bit((b >> i) & 1);
	}
}

int main(int argc, char** argv) {

	int frec[256][256];

	FILE* f = fopen(argv[1], "r");

	unsigned char b, p;
	int data_size = 0;
	while (1) {
		fread(&b, 1, 1, f);
		if (feof(f)) break;
		if (data_size != 0) {
			frec[p][b]++;
		}
		p = b;
		++data_size;
	}

	unsigned char most_common[256];
	for (int i = 0; i < 256; ++i) {
		int k = 0;
		for (int j = 1; j < 256; ++j) {
			if (frec[i][k] < frec[i][j]) {
				k = j;
			}
		}
		most_common[i] = k;
	}

	fwrite(most_common, 1, 256, stdout);
	fwrite(&data_size, 4, 1, stdout);

	rewind(f);
	fread(&b, 1, 1, f);
	write_byte(b);
	p = b;

	while (!feof(f)) {
		fread(&b, 1, 1, f);

		if (b == most_common[p]) {
			write_bit(1);
		} else {
			write_bit(0);
			write_byte(b);
		}

		p = b;
	}

	for (int i = 0; i < 7; ++i) {
		write_bit(0);
	}

}
