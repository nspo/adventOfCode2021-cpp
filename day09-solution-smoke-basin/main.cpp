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

    // const auto filename = "input_sample.txt";
    const auto filename = "input.txt";
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

    {
        std::cout << " --- Part 2 ---\n";

        const int rows = static_cast<int>(data.size());
        const int cols = static_cast<int>(data[0].size());
        vector<vector<bool>> visited(
            rows, vector<bool>(cols, false));  // could also mark visited as 9 fields

        auto find_basin_size = [&data, &is_valid, &visited](const Coord& low_point) -> int {
            // find size of basin of low point with DFS

            int num_points = 0;

            auto dfs = [&](const int row, const int col, const int last_val, auto& dfs) -> void {
                // DFS - only recurse if this point is valid and part of basin
                // Must be called from low-point initially with last_val = -1
                // Alternative would be to simply grow outwards until a 9 is encountered
                if (!is_valid(row, col)) return;
                if (visited[row][col]) return;
                const int val = data[row][col];
                if (val <= last_val || val == 9) return;

                visited[row][col] = true;
                ++num_points;

                dfs(row - 1, col, val, dfs);
                dfs(row + 1, col, val, dfs);
                dfs(row, col - 1, val, dfs);
                dfs(row, col + 1, val, dfs);
            };

            dfs(low_point.row, low_point.col, -1, dfs);
            return num_points;
        };

        // find 3 largest basins
        // could use a min-heap with size 3 for this... but not worth it for such a small map
        vector<int> basin_sizes;
        for (const auto& low_point : find_low_points()) {
            basin_sizes.push_back(find_basin_size(low_point));
        }
        std::sort(basin_sizes.begin(), basin_sizes.end(),
                  std::greater<>());  // so that largest are in front

        const std::array<int, 3> largest_basins = {basin_sizes.at(0), basin_sizes.at(1),
                                                   basin_sizes.at(2)};
        std::cout << "3 largest basin sizes: ";
        for (const int sz : largest_basins) std::cout << sz << " ";
        std::cout << "\n";
        const int prod = largest_basins[0] * largest_basins[1] * largest_basins[2];
        std::cout << "Their product: " << prod << "\n";
    }
}