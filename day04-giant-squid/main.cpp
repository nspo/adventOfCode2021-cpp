#include <array>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

using std::array;

// helper to keep track of board and matches on it
struct Board {
    array<array<int, 5>, 5>
        numbers;  // board itself, drawn numbers made negative (e.g. 5 to -5 when drawn)
    array<int, 5> matches_in_row;
    array<int, 5> matches_in_col;
};

// helper to reference a field on a board
struct BoardField {
    int board_index;
    int row;
    int col;
};

int main() {
    using std::string;
    using std::unordered_map;
    using std::vector;

    const auto filename = "input1.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    vector<int> drawn_numbers;
    {
        // parse first line into numbers. String processing is horrible in C++...
        string line;
        if (!std::getline(ifs, line)) std::terminate();

        size_t last = 0;
        size_t next = 0;
        string token;
        const auto delim = ",";
        while ((next = line.find(delim, last)) != string::npos) {
            const int num = std::stoi(line.substr(last, next - last));
            drawn_numbers.push_back(num);
            last = next + 1;
        }
        // also process last number
        const int num = std::stoi(line.substr(last, line.size() - last));
        drawn_numbers.push_back(num);
    }

    vector<Board> boards;
    while (true) {
        // read in the boards
        string line;
        if (!std::getline(ifs, line))
            break;  // empty line - when it's not there, assume we read everything
        boards.emplace_back(Board{});
        for (int row = 0; row < 5; ++row) {
            if (!(ifs >> boards.back().numbers[row][0] >> boards.back().numbers[row][1] >>
                  boards.back().numbers[row][2] >> boards.back().numbers[row][3] >>
                  boards.back().numbers[row][4])) {
                std::stringstream ss;
                ss << "Could not read in row " << row << " of board " << boards.size() - 1 << "\n";
                throw std::runtime_error(ss.str());
            }
        }
    }

    unordered_map<int, vector<BoardField>> fields_by_number;
    {
        // fill in hashmap for faster lookup
        for (int board_index = 0; board_index < static_cast<int>(boards.size()); ++board_index) {
            for (int row = 0; row < 5; ++row) {
                for (int col = 0; col < 5; ++col) {
                    fields_by_number[boards[board_index].numbers[row][col]].emplace_back(
                        BoardField{.board_index = board_index, .row = row, .col = col});
                }
            }
        }
    }

    std::cout << "Read " << drawn_numbers.size() << " numbers and " << boards.size() << " boards\n";

    {
        std::cout << "--- Part 1 ---\n";
        // use numbers until solution found
        bool winner_found = false;
        for (const int num : drawn_numbers) {
            if (fields_by_number.find(num) == fields_by_number.end()) continue;
            auto& fields = fields_by_number[num];
            for (const auto& field : fields) {
                boards[field.board_index].matches_in_row[field.row] += 1;
                boards[field.board_index].matches_in_col[field.col] += 1;
                boards[field.board_index].numbers[field.row][field.col] *= (-1);  // mark as matched

                if (boards[field.board_index].matches_in_row[field.row] == 5 ||
                    boards[field.board_index].matches_in_col[field.col] == 5) {
                    winner_found = true;
                    std::cout << "Board " << field.board_index << " has won with number " << num
                              << "\n";
                    // calculate score
                    int sum_unmarked = 0;
                    for (const auto& row_values : boards[field.board_index].numbers) {
                        for (const int val : row_values) {
                            if (val > 0) sum_unmarked += val;
                        }
                    }
                    std::cout << "Its winning score is " << sum_unmarked << "*" << num << " = "
                              << sum_unmarked * num << "\n";
                }
            }
            fields.clear();  // if number is drawn twice, don't do anything else

            if (winner_found) break;
        }
    }

    {
        std::cout << "--- Part 2 ---\n";

        vector<bool> board_won(boards.size(), false);
        int num_boards_won = 0;
        vector<string> winning_results;
        for (const int num : drawn_numbers) {
            if (fields_by_number.find(num) == fields_by_number.end()) continue;
            auto& fields = fields_by_number[num];
            for (const auto& field : fields) {
                if (board_won[field.board_index]) continue;

                boards[field.board_index].matches_in_row[field.row] += 1;
                boards[field.board_index].matches_in_col[field.col] += 1;
                boards[field.board_index].numbers[field.row][field.col] *= (-1);  // mark as matched

                if (boards[field.board_index].matches_in_row[field.row] == 5 ||
                    boards[field.board_index].matches_in_col[field.col] == 5) {
                    board_won[field.board_index] = true;
                    ++num_boards_won;

                    std::stringstream ss; // save result string
                    ss << "Board " << field.board_index << " has won with number " << num
                              << " ";
                    // calculate score
                    int sum_unmarked = 0;
                    for (const auto& row_values : boards[field.board_index].numbers) {
                        for (const int val : row_values) {
                            if (val > 0) sum_unmarked += val;
                        }
                    }
                    ss << "and score " << sum_unmarked << "*" << num << " = "
                              << sum_unmarked * num;
                    winning_results.emplace_back(ss.str());
                }
            }
            fields.clear();  // if number is drawn twice, don't do anything else

            if (num_boards_won == static_cast<int>(boards.size())) break;
        }

        assert(winning_results.size() == boards.size());
        std::cout << "Last winner: " << winning_results.back() << "\n";
    }
}