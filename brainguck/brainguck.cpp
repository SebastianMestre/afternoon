#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <cassert>

struct Stmt {
	enum class Tag { Inc, Dec, Left, Right, Read, Write, Loop };
	Stmt(Tag tag) : tag_{tag} {}
	Tag tag() { return tag_; }
private:
	Tag tag_;
};

struct Inc   : Stmt { Inc()   : Stmt(Tag::Inc)   {} };
struct Dec   : Stmt { Dec()   : Stmt(Tag::Dec)   {} };
struct Left  : Stmt { Left()  : Stmt(Tag::Left)  {} };
struct Right : Stmt { Right() : Stmt(Tag::Right) {} };
struct Read  : Stmt { Read()  : Stmt(Tag::Read)  {} };
struct Write : Stmt { Write() : Stmt(Tag::Write) {} };
struct Loop  : Stmt {
	std::vector<Stmt*> body;
	Loop(std::vector<Stmt*> body_) : Stmt(Tag::Loop), body{std::move(body_)} {}
};

void codegen(Stmt* s);

int tag_counter = 0;

void codegen_many(std::vector<Stmt*>& v) {
	int i = 0;
	while (i < v.size()) {
		if (v[i]->tag() == Stmt::Tag::Inc || v[i]->tag() == Stmt::Tag::Dec) {
			int acum = 0;
			if (v[i]->tag() == Stmt::Tag::Inc) acum += 1;
			if (v[i]->tag() == Stmt::Tag::Dec) acum -= 1;
			int j = i+1;
			while (j < v.size() && (v[j]->tag() == Stmt::Tag::Inc || v[j]->tag() == Stmt::Tag::Dec)) {
				if (v[j]->tag() == Stmt::Tag::Inc) acum += 1;
				if (v[j]->tag() == Stmt::Tag::Dec) acum -= 1;
				j++;
			}
			acum %= 256;
			if (acum != 0) {
				std::cout << " addb $" << acum << ", (%rax)\n";
			}
			i = j;
		} else if (v[i]->tag() == Stmt::Tag::Right || v[i]->tag() == Stmt::Tag::Left) {
			int acum = 0;
			if (v[i]->tag() == Stmt::Tag::Right) acum += 1;
			if (v[i]->tag() == Stmt::Tag::Left) acum -= 1;
			int j = i+1;
			while (j < v.size() && (v[j]->tag() == Stmt::Tag::Right || v[j]->tag() == Stmt::Tag::Left)) {
				if (v[j]->tag() == Stmt::Tag::Right) acum += 1;
				if (v[j]->tag() == Stmt::Tag::Left) acum -= 1;
				j++;
			}
			if (acum != 0) {
				std::cout << " addq $" << acum << ", %rax\n";
			}
			i = j;
		} else {
			codegen(v[i]);
			i++;
		}
	}
}

void codegen(Stmt* s) {
	switch (s->tag()) {
		case Stmt::Tag::Inc: 
			std::cout << " incb (%rax)\n";
			break;
		case Stmt::Tag::Dec: 
			std::cout << " decb (%rax)\n";
			break;
		case Stmt::Tag::Right: 
			std::cout << " inc %rax\n";
			break;
		case Stmt::Tag::Left: 
			std::cout << " dec %rax\n";
			break;
		case Stmt::Tag::Read:
			std::cout << " movq %rax, %rbx\n";
			std::cout << " call getchar\n";
			std::cout << " movb %al, (%rbx)\n";
			std::cout << " movq %rbx, %rax\n";
			break;
		case Stmt::Tag::Write:
			std::cout << " movq %rax, %rbx\n";
			std::cout << " movb (%rax), %dil\n";
			std::cout << " call putchar\n";
			std::cout << " movq %rbx, %rax\n";
			break;
		case Stmt::Tag::Loop: {
			int start_label = tag_counter++;
			int end_label = tag_counter++;
			std::cout << "L" << start_label << ":\n";
			std::cout << " cmpb $0, (%rax)\n";
			std::cout << " je L" << end_label << "\n";
			codegen_many(static_cast<Loop*>(s)->body);
			std::cout << " jmp L" << start_label << "\n";
			std::cout << "L" << end_label << ":\n";
		} break;
	}
}

void skip_noise(char const*& p) {
	while (*p != '\0' && *p != '[' && *p != ']' && *p != '+' && *p != '-' && *p != '>' && *p != '<' && *p != '.' && *p != ',') ++p;
}

Stmt* parse(char const*& p) {
	switch (*p) {
		case '[': {
			++p;
			skip_noise(p);
			std::vector<Stmt*> body;
			while (*p && *p != ']') {
				body.push_back(parse(p));
				skip_noise(p);
			}
			assert(*p);
			++p;
			return new Loop(std::move(body));
		} break;
		case '+': return (++p, new Inc());
		case '-': return (++p, new Dec());
		case '>': return (++p, new Right());
		case '<': return (++p, new Left());
		case '.': return (++p, new Write());
		case ',': return (++p, new Read());
	}
	assert(0);
}

std::vector<Stmt*> parse_many(char const*& p) {
	std::vector<Stmt*> result;
	skip_noise(p);
	while (*p) {
		result.push_back(parse(p));
		skip_noise(p);
	}
	return result;
}

int main(int argc, char** argv) {
	std::ifstream input_file(argv[1]);
	std::stringstream ss;
	std::string line;
	while (std::getline(input_file, line)) {
		ss << line << "\n";
	}
	std::string file_contents = std::move(ss).str();

	char const* p = file_contents.c_str();
	std::vector<Stmt*> program = parse_many(p);

	std::cout << ".text\n";
	std::cout << ".global runner\n";
	std::cout << "runner:\n";
	std::cout << "movq %rdi, %rax\n";
	codegen_many(program);
	std::cout << "ret\n";
}
