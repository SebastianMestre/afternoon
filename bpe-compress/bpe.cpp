#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <utility>

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

		for (int i = 1; i < data.size(); ++i) {
			histogram[{data[i-1], data[i]}] += 1;
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

		vector<int> new_data;
		new_data.reserve(data.size());

		int prev = -1;
		for (int x : data) {
			if (prev != -1) {
				if (make_pair(prev, x) == most_common.second) {
					new_data.push_back(new_symbol);
					prev = -1;
				} else {
					new_data.push_back(prev);
					prev = x;
				}
			} else {
				prev = x;
			}
		}

		if (prev != -1) {
			new_data.push_back(prev);
		}

		data = move(new_data);
	}

	std::cout << mapping.size() << "\n";
	for (auto kv : mapping) {
		std::cout << kv.first.first << " " << kv.first.second << " -> " << kv.second << "\n";
	}

	auto sep = "";
	for (int x : data) {
		std::cout << sep << x;
		sep = " ";
	}
	std::cout << "\n";

	int const distinct_token_count = 256 + int(mapping.size());
	int const token_count = int(mapping.size()) * 2 + int(data.size());

	double const bits_per_token = log2(distinct_token_count);
	int const full_bits_per_token = int(ceil(bits_per_token));
	int const full_bytes_per_token = (full_bits_per_token + 7) / 8;

	std::cerr << "DISTINCT TOKENS: " << distinct_token_count << "\n";
	std::cerr << "UNCOMPRESSED SIZE:            " << int(file_content.size()) << "\n";
	std::cerr << "ESTIMATED SIZE (byte packed): " << 4 + token_count * full_bytes_per_token << "\n";
	std::cerr << "ESTIMATED SIZE (bit packed):  " << 4 + (token_count * full_bits_per_token + 7) / 8 << "\n";
	std::cerr << "ESTIMATED SIZE (poly packed): " << 4 + (int(ceil(token_count * bits_per_token)) + 7) / 8<< "\n";

}
