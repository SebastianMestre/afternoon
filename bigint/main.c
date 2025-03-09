#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define BASE (1ull << 32ull)
typedef uint32_t digit_t;

typedef struct bigint {
	digit_t* digits;
	int size;
} bigint_t;

int max(int x, int y) {
	return x < y ? y : x;
}

void bigint_print(bigint_t c) {
	for (int i = 0; i < c.size; ++i) {
		if (i != 0) {
			printf(" ");
		}
		printf("%u", c.digits[i]);
	}
	printf("\n");
}

bigint_t bigint_create(int size) {
	bigint_t result = {
		malloc(size * sizeof(digit_t)),
		size,
	};
	return result;
}

void bigint_trim(bigint_t* a) {
	while (a->size > 1 && a->digits[a->size-1] == 0) {
		a->size -= 1;
	}
}

bigint_t bigint_add(bigint_t a, bigint_t b) {
	int size = max(a.size, b.size) + 1;
	bigint_t result = bigint_create(size);

	result.digits[0] = 0;
	for (int i = 0; i < size-1; ++i) {
		digit_t a_digit = i < a.size ? a.digits[i] : 0u;
		digit_t b_digit = i < b.size ? b.digits[i] : 0u;
		uint64_t value = (uint64_t) result.digits[i] + a_digit + b_digit;
		if (value >= BASE) {
			result.digits[i] = value - BASE;
			result.digits[i+1] = 1;
		} else {
			result.digits[i] = value;
			result.digits[i+1] = 0;
		}
	}

	if (result.digits[size-1] == 0) {
		result.size -= 1;
	}

	return result;
}

bigint_t bigint_sub(bigint_t a, bigint_t b) {
	// supongo que a >= b

	int size = max(a.size, b.size);
	bigint_t result = bigint_create(size + 1);

	result.digits[0] = 0;
	for (int i = 0; i < size; ++i) {
		digit_t a_digit = i < a.size ? a.digits[i] : 0u;
		digit_t b_digit = i < b.size ? b.digits[i] : 0u;
		int64_t value = (int64_t) (int32_t) result.digits[i] + a_digit - b_digit;
		if (value < 0) {
			result.digits[i] = value + BASE;
			result.digits[i+1] = (uint32_t) -1;
		} else {
			result.digits[i] = value;
			result.digits[i+1] = 0;
		}
	}

	assert(result.digits[size] == 0);
	result.size -= 1;

	bigint_trim(&result);

	return result;
}

bigint_t bigint_shift(bigint_t a, int offset) {
	bigint_t result = bigint_create(a.size + offset);
	for (int i = 0; i < offset; ++i) {
		result.digits[i] = 0;
	}
	for (int i = 0; i < a.size; ++i) {
		result.digits[i+offset] = a.digits[i];
	}
	return result;
}

bigint_t bigint_pad(bigint_t a, int size) {
	assert(a.size <= size);
	bigint_t result = bigint_create(size);
	for (int i = 0; i < a.size; ++i) {
		result.digits[i] = a.digits[i];
	}
	for (int i = a.size; i < size; ++i) {
		result.digits[i] = 0;
	}
	return result;
}

void bigint_split(bigint_t a, bigint_t* out_lo, bigint_t* out_hi) {
	int lo_size = a.size / 2;
	bigint_t lo = bigint_create(lo_size);

	int hi_size = a.size - lo_size;
	bigint_t hi = bigint_create(hi_size);

	for (int i = 0; i < lo_size; ++i) {
		lo.digits[i] = a.digits[i];
	}

	for (int i = 0; i < hi_size; ++i) {
		hi.digits[i] = a.digits[i + lo_size];
	}

	*out_lo = lo;
	*out_hi = hi;
}

bigint_t bigint_mul(bigint_t a, bigint_t b);

bigint_t karatsuba(bigint_t a, bigint_t b) {
	// printf("=========\n");
	// printf("a:"); bigint_print(a);
	// printf("b:"); bigint_print(b);

	assert(a.size == b.size);

	if (a.size == 1) {
		uint64_t value = (uint64_t) a.digits[0] * b.digits[0];
		if (value < BASE) {
			bigint_t result = bigint_create(1);
			result.digits[0] = value;
			return result;
		} else {
			bigint_t result = bigint_create(2);
			result.digits[0] = value % BASE;
			result.digits[1] = value / BASE;
			return result;
		}
	}


	bigint_t a_lo, a_hi;
	bigint_split(a, &a_lo, &a_hi);

	bigint_t b_lo, b_hi;
	bigint_split(b, &b_lo, &b_hi);

	bigint_t X = karatsuba(a_hi, b_hi);
	bigint_t Y = karatsuba(a_lo, b_lo);

	bigint_t a_sum = bigint_add(a_hi, a_lo);
	free(a_lo.digits);
	free(a_hi.digits);

	bigint_t b_sum = bigint_add(b_hi, b_lo);
	free(b_lo.digits);
	free(b_hi.digits);

	bigint_t Z = bigint_mul(a_sum, b_sum);
	free(a_sum.digits);
	free(b_sum.digits);

	bigint_t X_plus_Y = bigint_add(X, Y);
	bigint_t W = bigint_sub(Z, X_plus_Y);
	free(X_plus_Y.digits);
	free(Z.digits);

	int offset = b_lo.size;

	bigint_t X_shifted = bigint_shift(X, 2*offset);
	free(X.digits);

	bigint_t W_shifted = bigint_shift(W, offset);
	free(W.digits);

	bigint_t X_plus_W = bigint_add(W_shifted, X_shifted);
	free(X_shifted.digits);
	free(W_shifted.digits);

	bigint_t result = bigint_add(Y, X_plus_W);
	free(Y.digits);
	free(X_plus_W.digits);

	bigint_trim(&result);

	return result;
}


bigint_t bigint_mul(bigint_t a, bigint_t b) {
	int pad_a = 0;
	int pad_b = 0;
	if (a.size < b.size) {
		a = bigint_pad(a, b.size);
		pad_a = 1;
	} else if (b.size < a.size) {
		b = bigint_pad(b, a.size);
		pad_b = 1;
	}
	bigint_t result = karatsuba(a, b);
	if (pad_a) {
		free(a.digits);
	}
	if (pad_b) {
		free(b.digits);
	}
	return result;
}


int main(int argc, char** argv) {

	int n = atoi(argv[1]);

	bigint_t a = bigint_create(n);
	for (int i = 0; i < n; ++i) {
		a.digits[i] = rand() % BASE;
	}

	bigint_t b = bigint_create(n);
	for (int i = 0; i < n; ++i) {
		b.digits[i] = rand() % BASE;
	}

	clock_t begin = clock();
	bigint_t c = bigint_mul(a, b);
	clock_t end = clock();

	bigint_print(c);

	printf("time elapsed: %lfs\n", (double)(end - begin)/CLOCKS_PER_SEC);
}
