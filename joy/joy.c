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
	WORD_CONCAT,
	WORD_QUOTE,

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
	"WORD_CONCAT",
	"WORD_QUOTE",
};

char const* builtin_words_strings[] = {
	"",
	"[",
	"]",
	"eval",
	"pop",
	"dup",
	"swap",
	"concat",
	"quote",
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

void print(word_t* code, FILE* fp) {
	int position = 0;
	for (; *code != WORD_STOP; ++code, ++position) {
		fprintf(fp, "[%04x] %s\n", position, builtin_words_name[*code]);
	}
}

void show(word_t* code, FILE* fp) {
	fprintf(fp, "[");
	for (; *code != WORD_STOP; ++code) {
		fprintf(fp, " %s", builtin_words_strings[*code]);
	}
	fprintf(fp, " ]\n");
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
	fprintf(stderr, "eval_all:\n");
	print(code, stderr);
	fprintf(stderr, "\n");
	return eval(code, 0);
}

void eval_quote(word_t* code, int position) {
	for (; ; ++code, ++position) {
		fprintf(stderr, "[%04x] %s\n", position, builtin_words_name[*code]);

		switch (*code) {
		case WORD_LB:
			++quotes;
			quote_buffer[quote_buffer_len++] = *code;
			break;
		case WORD_RB:
			--quotes;
			if (quotes == 0) {
				fprintf(stderr, "exited quote-mode\n");
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
		fprintf(stderr, "[%04x] %s\n", position, builtin_words_name[*code]);

		switch (*code) {
		case WORD_STOP:
			return;
		case WORD_LB:
			++quotes;
			assert(quotes == 1);
			fprintf(stderr, "entered quote-mode\n");
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
		case WORD_CONCAT:
			{
				word_t* right = stk[--stk_top];
				word_t* left  = stk[--stk_top];
				assert(quote_buffer_len == 0);

				int len_left = 0;
				while (left[len_left] != WORD_STOP)
					quote_buffer[quote_buffer_len++] = left[len_left++];

				int len_right = 0;
				while (right[len_right] != WORD_STOP)
					quote_buffer[quote_buffer_len++] = right[len_right++];

				stk[stk_top] = make_quote(quote_buffer, quote_buffer_len);
				quote_buffer_len = 0;
				++stk_top;
			} break;
		case WORD_QUOTE:
			{
				word_t* temp = stk[--stk_top];
				assert(quote_buffer_len == 0);
				quote_buffer[quote_buffer_len++] = WORD_LB;
				int len = 0;
				while (temp[len] != WORD_STOP)
					quote_buffer[quote_buffer_len++] = temp[len++];
				quote_buffer[quote_buffer_len++] = WORD_RB;
				stk[stk_top] = make_quote(quote_buffer, quote_buffer_len);
				quote_buffer_len = 0;
				++stk_top;
			} break;
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

	printf("STACK STATE:\n");
	for (int i = 0; i < stk_top; ++i) {
		show(stk[i], stdout);
	}
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
	quotes = 0;
	stk_top = 0;
	quote_buffer_len = 0;

	char source[] = "[ [ ] dup ] dup eval";
	word_t code[100];
	parse(source, code);
	eval_all(code);

	printf("STACK STATE:\n");
	for (int i = 0; i < stk_top; ++i) {
		show(stk[i], stdout);
	}
}

void misc_test() {
	printf("==== MISC TEST ====\n");
	quotes = 0;
	stk_top = 0;
	quote_buffer_len = 0;

	char source[] = "[ ] quote dup concat eval";
	word_t code[100];
	parse(source, code);
	eval_all(code);

	printf("STACK STATE:\n");
	for (int i = 0; i < stk_top; ++i) {
		show(stk[i], stdout);
	}
}

int main() {
	basic_test();
	parser_test();
	misc_test();
}
