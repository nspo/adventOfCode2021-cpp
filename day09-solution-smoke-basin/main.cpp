#include <array>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

// string split like in Python, nearly identical to https://stackoverflow.com/a/46931770/997151
std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    using std::string;
    using std::vector;

    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

struct Coord {
    int row;
    int col;
};

int main() {
    using std::string;
    using std::vector;

    const auto filename = "input_sample.txt";
    // const auto filename = "input.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    vector<vector<int>> data;
    {
        // read in input
        std::string line;
        while (std::getline(ifs, line)) {
            data.emplace_back();
            for (const char c : line) {
                const int num = c - '0';
                assert(num >= 0 && num <= 9);
                data.back().push_back(num);
            }
        }
        assert(data.size() > 0);
        for (const auto& row : data) {
            assert(row.size() == data.front().size());
        }
    }

    auto is_valid = [&data](const int row, const int col) -> bool {
        return row >= 0 && row < static_cast<int>(data.size()) && col >= 0 &&
               col < static_cast<int>(data[0].size());
    };

    auto lowest_neighbor_val = [&data, &is_valid](const int row, const int col) -> int {
        int lowest = 10;
        auto check = [&lowest, &data, &is_valid](const int row, const int col) -> void {
            if (is_valid(row, col)) {
                lowest = std::min(lowest, data[row][col]);
            }
        };
        check(row - 1, col);
        check(row + 1, col);
        check(row, col - 1);
        check(row, col + 1);

        return lowest;
    };

    auto find_low_points = [&data, &lowest_neighbor_val]() -> vector<Coord> {
        vector<Coord> lowest_points;
        for (int row = 0; row < static_cast<int>(data.size()); ++row) {
            for (int col = 0; col < static_cast<int>(data[0].size()); ++col) {
                const int val = data[row][col];
                if (val < lowest_neighbor_val(row, col)) {
                    lowest_points.emplace_back(Coord{.row = row, .col = col});
                }
            }
        }
        return lowest_points;
    };

    {
        std::cout << " --- Part 1 ---\n";

        int sum_of_lowest_risks = 0;
        for (const Coord& coord : find_low_points()) {
            sum_of_lowest_risks += 1 + data[coord.row][coord.col];
        }
        std::cout << "Sum of lowest risks: " << sum_of_lowest_risks << "\n";
    }

    
}