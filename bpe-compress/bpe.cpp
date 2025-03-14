#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>

#include <cmath>

using namespace std;

int main() {

	stringstream ss;
	string line;
	while (getline(cin, line)) {
		ss << line << "\n";
	}
	string file_content = move(ss).str();

	vector<int> data;
	data.reserve(file_content.size());
	for (char c : file_content) {
		data.push_back(static_cast<int>(static_cast<unsigned char>(c)));
	}

	int counter = 256;
	map<pair<int,int>, int> mapping;

	int iter = 0;
	while (true) {
		std::cerr << "START ITER " << iter++ << "\n";

		map<pair<int,int>, int> histogram;

		{
			int prev = -1;
			for (int i = 0; i < int(data.size()); ++i) {
				int x = data[i];
				if (x == -1) continue;
				if (prev != -1) {
					histogram[{prev, data[i]}] += 1;
				}
				prev = x;
			}
		}

		pair<int, pair<int,int>> most_common = {-1, {0, 0}};
		for (auto kv : histogram) {
			most_common = max(most_common, {kv.second, kv.first});
		}

		if (most_common.first < 2) {
			break;
		}

		int new_symbol = counter++;
		mapping[most_common.second] = new_symbol;

		int prev = -1;
		int prev_pos = -1;
		for (int i = 0; i < int(data.size()); ++i) {
			int x = data[i];
			if (x == -1) continue;
			if (prev != -1) {
				if (make_pair(prev, x) == most_common.second) {
					data[prev_pos] = new_symbol;
					data[i] = -1;
					prev = -1;
					prev_pos = -1;
				} else {
					prev = x;
					prev_pos = i;
				}
			} else {
				prev = x;
				prev_pos = i;
			}
		}

	}

	std::cout << mapping.size() << "\n";
	for (auto kv : mapping) {
		std::cout << kv.first.first << " " << kv.first.second << " -> " << kv.second << "\n";
	}

	auto sep = "";
	for (int x : data) {
		if (x == -1) continue;
		std::cout << sep << x;
		sep = " ";
	}
	std::cout << "\n";

	int const distinct_token_count = 256 + int(mapping.size());
	int const token_count = int(mapping.size()) * 2 + int(data.size()) - int(count(begin(data), end(data), -1));

	double const bits_per_token = log2(distinct_token_count);
	int const full_bits_per_token = int(ceil(bits_per_token));
	int const full_bytes_per_token = (full_bits_per_token + 7) / 8;

	std::cerr << "DISTINCT TOKENS: " << distinct_token_count << "\n";
	std::cerr << "UNCOMPRESSED SIZE:            " << int(file_content.size()) << "\n";
	std::cerr << "ESTIMATED SIZE (byte packed): " << 4 + token_count * full_bytes_per_token << "\n";
	std::cerr << "ESTIMATED SIZE (bit packed):  " << 4 + (token_count * full_bits_per_token + 7) / 8 << "\n";
	std::cerr << "ESTIMATED SIZE (poly packed): " << 4 + (int(ceil(token_count * bits_per_token)) + 7) / 8<< "\n";

}
