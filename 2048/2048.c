#include <assert.h>
#include <stdint.h>
#include <stdio.h>

enum value {
	VEMPTY, V2, V4, V8,
	V16, V32, V64, V128,
	V256, V512, V1024, V2048,
	V4096, V8192, V16384, V32768,
};

typedef uint64_t board_t;

void print_board(board_t b);

enum value get(board_t* b, int i, int j) {
	assert(0 <= i && i < 4);
	assert(0 <= j && j < 4);
	unsigned ix = i * 4 + j;
	return (*b >> (ix * 4)) & 0xfull;
}

void set(board_t* b, int i, int j, enum value v) {
	unsigned ix = i * 4 + j;
	*b &= ~(0xfull << (ix * 4));
	*b |= ((uint64_t)v << (ix * 4));
}

board_t with(board_t b, int i, int j, enum value v) {
	set(&b, i, j, v);
	return b;
}

int is_over(board_t b) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (get(&b, i, j) == V2048) {
				return 1;
			}
		}
	}
	return 0;
}

board_t rot_cw(board_t b) {
	board_t r = 0;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			set(&r, i, j, get(&b, 3-j, i));
		}
	}
	return r;
}

board_t with_shift_left(board_t b) {
	board_t fixed = 0;
	for (int i = 0; i < 4; ++i) {

		for (int j = 0; j < 4; ++j) {
			int jj = j;
			while (1) {
				if (jj == 0) break;
				enum value u = get(&b, i, jj);
				if (u == VEMPTY) break;
				enum value v = get(&b, i, jj - 1);
				if (v == VEMPTY) {
					set(&b, i, jj-1, u);
					set(&b, i, jj, VEMPTY);
					jj = jj-1;
					continue;
				} else if (v == u && get(&fixed, i, jj-1) == VEMPTY) {
					set(&b, i, jj-1, u+1);
					set(&b, i, jj, VEMPTY);
					set(&fixed, i, jj-1, V2);
					break;
				} else {
					break;
				}
			}
		}

	}

	return b;
}

board_t with_shift(board_t b, int dir) {
	for (int k = 0; k < dir; ++k) {
		b = rot_cw(b);
	}

	b = with_shift_left(b);

	for (int k = 0; k < 4-dir; ++k) {
		b = rot_cw(b);
	}

	return b;
}

uint16_t shift_table[1u << 16u];
void build_shift_table() {
	for (board_t b = 0; b < (1u << 16u); ++b) {
		shift_table[(uint16_t)b] = with_shift_left(b);
	}
}

board_t with_shift_left_tbl(board_t b) {
	board_t res = 0;
	for (unsigned i = 0; i < 64; i += 16) {
		res |= (board_t)shift_table[(b >> i) & 0xffffu] << i;
	}
	return res;
}

board_t with_shift_tbl(board_t b, int dir) {
	assert(0 <= dir && dir <= 4);

	for (int k = 0; k < dir; ++k) {
		b = rot_cw(b);
	}

	b = with_shift_left_tbl(b);

	for (int k = 0; k < (3&(4-dir)); ++k) {
		b = rot_cw(b);
	}

	return b;
}

int pos = 0;
uint64_t states = 0;
board_t steps[600];

// search for game of 2048 with <= depth
int iddfs(board_t b, int depth) {

	states++;
	steps[pos++] = b;

	// print_board(b); printf("\n");

	if (depth == 0) return (pos--, 0);
	if (is_over(b)) return 1;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (get(&b, i, j) != VEMPTY) continue;
			for (enum value v = V4; v >= V4; --v) {
				board_t b_ = with(b, i, j, v);
				for (int dir = 0; dir < 4; ++dir) {
					board_t b__ = with_shift_tbl(b_, dir);
					if (b__ == b_) continue;
					if (iddfs(b__, depth-1)) return 1;
				}
			}
		}
	}

	return (pos--, 0);
}

void print_board(board_t b) {
	char const* rep[] = {
	"     ", "    2", "    4", "    8",
	"   16", "   32", "   64", "  128",
	"  256", "  512", " 1024", " 2048",
	" 4096", " 8192", "16384", "32768",
	};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (j != 0) printf("|");
			printf("%s", rep[get(&b, i, j)]);
		}
		printf("\n");
	}
}

int main() {
	build_shift_table();

	int depth_limit;
	scanf("%d", &depth_limit);
	if (iddfs(0, depth_limit)) {
		for (int i = pos-1; i < pos; ++i) {
			print_board(steps[i]);
			printf("\n");
		}
		printf("FOUND in %d steps\n", pos);
		printf("SEARCHED %lu states\n", states);
		return 0;
	} else {
		return 1;
	}
}
