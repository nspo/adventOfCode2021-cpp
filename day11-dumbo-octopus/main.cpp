#include <array>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
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

int main() {
    using std::array;
    using std::string;
    using std::vector;

    // const auto filename = "input_small.txt";
    // const auto filename = "input_sample.txt";
    const auto filename = "input.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    vector<vector<int>> grid;
    {
        // read in lines
        string line;
        while (std::getline(ifs, line)) {
            grid.emplace_back();
            for (const char c : line) {
                const int val = c - '0';
                assert(0 <= val && val <= 9);
                grid.back().push_back(val);
            }
        }
        assert(!grid.empty() && !grid[0].empty());
        // check if valid
        for (size_t row = 0; row < grid.size(); ++row) {
            assert(grid[row].size() == grid[0].size());
        }
    }

    const auto orig_grid = grid;

    [[maybe_unused]] auto print = [&grid]() -> void {
        for (const auto& row : grid) {
            for (const int elem : row) {
                std::cout << elem << "|";
            }
            std::cout << "\n";
        }
    };

    auto increment_all = [&grid]() -> void {
        // increment all cells in grid by 1 without any checks or recursion
        for (auto& row : grid) {
            for (int& elem : row) {
                ++elem;
            }
        }
    };

    const int rows = grid.size();
    const int cols = grid[0].size();

    auto is_valid = [rows, cols](const int row, const int col) -> bool {
        return row >= 0 && row < rows && col >= 0 && col < cols;
    };

    auto shine_on = [&grid, &is_valid](const int row, const int col, auto& shine_on,
                                       auto& check_flash, auto& flash_handler) -> void {
        // simulate that a light shines on this cell. May recurse.
        if (!is_valid(row, col)) throw std::runtime_error("Cannot shine_on invalid cell");
        int& val = grid[row][col];
        if (val == 0) return;   // did already flash this step
        if (val == 10) return;  // will flash anyway this step later

        ++val;
        check_flash(row, col, shine_on, check_flash, flash_handler);
    };

    auto check_flash = [&grid, &is_valid](const int row, const int col, auto& shine_on,
                                          auto& check_flash, auto& flash_handler) -> void {
        // check if this field must flash, and flash if so and shine on others
        if (!is_valid(row, col)) throw std::runtime_error("Cannot check_flash on invalid cell");
        int& val = grid[row][col];
        if (val != 10) return;  // will not flash

        // flash
        val = 0;
        flash_handler();

        // and shine on neighbors
        const array<array<int, 2>, 8> deltas = {
            {{-1, -1}, {-1, 0}, {-1, +1}, {0, -1}, {0, +1}, {+1, -1}, {+1, 0}, {+1, +1}}};

        for (const auto [drow, dcol] : deltas) {
            const int nb_row = row + drow;
            const int nb_col = col + dcol;
            if (is_valid(nb_row, nb_col))
                shine_on(nb_row, nb_col, shine_on, check_flash, flash_handler);
        }
    };

    auto step = [&grid, &increment_all, &rows, &cols, &check_flash,
                 &shine_on](auto& flash_handler) -> void {
        increment_all();

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                check_flash(row, col, shine_on, check_flash, flash_handler);
            }
        }
    };

    {
        std::cout << " --- Part 1 ---\n";
        int num_flashes = 0;
        auto flash_handler = [&num_flashes]() -> void { ++num_flashes; };

        const int steps = 100;
        // print();
        for (int i = 0; i < steps; ++i) {
            step(flash_handler);
            // std::cout << "---\n";
            // print();
        }
        std::cout << num_flashes << " flashes happened\n";
    }

    {
        std::cout << " --- Part 2 ---\n";
        grid = orig_grid;

        auto is_all_zeros = [&grid]() -> bool {
            for (const vector<int>& row : grid) {
                for (const int elem : row) {
                    if (elem != 0) return false;
                }
            }
            return true;
        };

        auto flash_handler = []() -> void {};

        for (int i = 0; i < 9999; ++i) {
            if (is_all_zeros()) {
                std::cout << "grid is all zeros after step " << i << "\n";
                break;
            }
            step(flash_handler);
        }
    }
}