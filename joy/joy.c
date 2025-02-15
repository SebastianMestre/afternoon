#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int word_t;
enum builtin_words {
	WORD_STOP,
	WORD_LB,
	WORD_RB,
	WORD_EVAL,
	WORD_POP,
	WORD_DUP,
	WORD_SWAP,

	WORD_BUILTIN_COUNT,
};

char const* builtin_words_name[] = {
	"WORD_STOP",
	"WORD_LB",
	"WORD_RB",
	"WORD_EVAL",
	"WORD_POP",
	"WORD_DUP",
	"WORD_SWAP",
};

char const* builtin_words_strings[] = {
	"",
	"[",
	"]",
	"eval",
	"pop",
	"dup",
	"swap",
};

word_t* make_quote(word_t* words, int len) {
	word_t* result = malloc(sizeof(word_t) * (len + 1));
	for (int i = 0; i < len; ++i) {
		result[i] = words[i];
	}
	result[len] = WORD_STOP;
	return result;
}

#define QUOTE_BUFFER_MAX 1000
#define STACK_MAX 1000

void print(word_t* code) {
	int position = 0;
	int quotes = 0;
	for (; ; ++code, ++position) {
		printf("[%04x] %s\n", position, builtin_words_name[*code]);
		if (*code == WORD_LB) ++quotes;
		if (*code == WORD_RB) --quotes;
		if (*code == WORD_STOP && !quotes) return;
	}
}

word_t* stk[STACK_MAX];
int stk_top = 0;

int quotes = 0;
int quote_buffer_len = 0;
word_t quote_buffer[QUOTE_BUFFER_MAX];

void eval_quote(word_t* code, int position);
void eval(word_t* code, int position);
void eval_all(word_t* code);

void eval_all(word_t* code) {
	printf("eval_all:\n");
	print(code);
	printf("\n");
	return eval(code, 0);
}

void eval_quote(word_t* code, int position) {
	for (; ; ++code, ++position) {
		printf("[%04x] %s\n", position, builtin_words_name[*code]);

		switch (*code) {
		case WORD_LB:
			++quotes;
			quote_buffer[quote_buffer_len++] = *code;
			break;
		case WORD_RB:
			--quotes;
			if (quotes == 0) {
				printf("exited quote-mode\n");
				stk[stk_top] = make_quote(quote_buffer, quote_buffer_len);
				quote_buffer_len = 0;
				++stk_top;
				return eval(code+1, position+1);
			} else {
				assert(quotes > 0);
				quote_buffer[quote_buffer_len++] = *code;
			}
			break;
		default:
			quote_buffer[quote_buffer_len++] = *code;
			break;
		}
	}
}

void eval(word_t* code, int position) {
	for (; ; ++code, ++position) {
		printf("[%04x] %s\n", position, builtin_words_name[*code]);

		switch (*code) {
		case WORD_STOP:
			return;
		case WORD_LB:
			++quotes;
			assert(quotes == 1);
			printf("entered quote-mode\n");
			// do nothing, we have just entered quote-mode
			return eval_quote(code+1, position+1);
		case WORD_RB:
			assert(0);
			break;
		case WORD_EVAL:
			{
				word_t* temp = stk[--stk_top];
				eval_all(temp);
			}
			break;
		case WORD_POP:
			++stk_top;
			break;
		case WORD_DUP:
			stk[stk_top] = stk[stk_top-1];
			++stk_top;
			break;
		}
	}
}

void basic_test() {
	printf("==== BASIC TEST ====\n");
	quotes = 0;
	stk_top = 0;
	quote_buffer_len = 0;

	// [ [ ] dup ] dup eval
	word_t program[] = {
		WORD_LB,      // [
		WORD_LB,      // [
		WORD_RB,      // ]
		WORD_DUP,     // dup
		WORD_RB,      // ]
		WORD_DUP,     // dup
		WORD_EVAL,    // eval
		WORD_STOP,
	};
	eval_all(program);
}

void parse(char* str, word_t* out_code) {
	if (*str) while (1) {
		while (isspace(*str)) ++str;
		if (!*str) break;
		char const* begin = str;
		while (*str && !isspace(*str)) ++str;
		int reached_end = *str == '\0';
		*str = '\0';
		int found = 0;
		for (int i = 0; i < WORD_BUILTIN_COUNT; ++i) {
			if (strcmp(builtin_words_strings[i], begin) == 0) {
				found = 1;
				*out_code++ = i;
				break;
			}
		}
		if (!found) {
			fprintf(stderr, "invalid word %s\n", begin);
		}
		str += !reached_end;
	}
	*out_code++ = WORD_STOP;
}

void parser_test() {
	printf("==== PARSER TEST ====\n");
	char source[] = "[ [ ] dup ] dup eval";
	word_t code[100];
	parse(source, code);
	eval_all(code);
}

int main() {
	basic_test();
	parser_test();
}
