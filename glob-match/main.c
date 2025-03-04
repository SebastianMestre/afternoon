#include <stdio.h>

int match(char const* str, char const* pat) {
	// simple optimization to avoid backtracking on cases like
	// match("aaaaaaaaaaaaaaa", "***********b")
	if (*pat == '*' && *(pat+1) == '*') return match(str, pat+1);

	if (!*str) {
		if (!*pat) return 1;
		if (*pat == '*') return match(str, pat+1);
		return 0;
	} else {
		if (!*pat) return 0;
		if (*pat == '?') return match(str+1, pat+1);
		if (*pat == '*') return match(str, pat+1) || match(str+1, pat);
		if (*pat == *str) return match(str+1, pat+1);
		return 0;
	}
}

int main() {
	char const* strings[] = {
		"menace",
		"menace/README.md",
		"menace/src",
		"menace/src/main.js",
		"menace/src/index.html",
		"replacer",
		"replacer/example",
		"replacer/example/output.joy",
		"replacer/example/program.joy",
		"replacer/example/defs",
		"replacer/readme.md",
		"replacer/replacer.py",
		"rijiggly-body",
		"rijiggly-body/index2.html",
		"rijiggly-body/index.html",
		"rijiggly-body/particle2.js",
		"rijiggly-body/particle.js",
		"BFAF",
		"BFAF/.clang-format",
		"BFAF/README.md",
		"BFAF/tests",
		"BFAF/tests/collatz.bf.formatted",
		"BFAF/tests/test.bf",
		"BFAF/tests/collatz.bf",
		"BFAF/tests/test.bf.formatted",
		"BFAF/src",
		"BFAF/src/main.cpp",
		"BFAF/LICENSE",
		"polynomials-a-la-carte",
		"polynomials-a-la-carte/index.html",
		"README.md",
		"joy",
		"joy/readme.md",
		"joy/joy.c",
		"brainguck",
		"brainguck/fib.b",
		"brainguck/README.md",
		"brainguck/brainguck.cpp",
		"brainguck/mandelbrot.b",
		"brainguck/main.c",
		"brainguck/run.sh",
		"cellular",
		"cellular/README.md",
		"cellular/main.cpp",
		"ir-compiler",
		"ir-compiler/design.md",
		NULL,
	};

	#define PATTERN_MAX_SIZE 256
	char pattern[PATTERN_MAX_SIZE];
	scanf("%[^\n]%*c", pattern);

	for (int i = 0; strings[i]; ++i) {
		if (match(strings[i], pattern)) {
			printf("%s\n", strings[i]);
		}
	}
}
