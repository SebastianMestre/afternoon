#include <iostream>
#include <vector>

std::vector<bool> advance(std::vector<bool> world, uint8_t rule) {
	world.insert(std::begin(world), world[0]);
	world.insert(std::end(world), world.back());
	std::vector<bool> result(world.size() - 2);
	for (int i = 1; i+1 < world.size(); ++i) {
		int neighbors = 0b111 ^ (int(world[i-1]) * 4 + int(world[i]) * 2 + int(world[i+1]));
		result[i-1] = bool(rule & (1 << (7 - neighbors)));
	}
	return result;
}

void show_line(std::vector<bool> const& line) {
	for (int i = 0; i < line.size(); ++i) {
		if (line[i]) {
			std::cout << "\xe2\x96\x88";
			std::cout << "\xe2\x96\x88";
		} else {
			std::cout << " ";
			std::cout << " ";
		}
	}
	std::cout << "\n";
}

int main(int argc, char** argv) {
	if (argc != 2 and argc != 3) {
		std::cerr << "usage: " << argv[0] << " <rule-number> [line-count]\n";
		return 1;
	}

	int const rule = atoi(argv[1]);
	int const lines = argc == 3 ? atoi(argv[2]) : 30;

	std::vector<bool> state(lines*2+1, false);
	state[lines] = true;

	show_line(state);

	for (int i = 1; i <= lines; ++i) {
		state = advance(std::move(state), rule);
		show_line(state);
	}
}
