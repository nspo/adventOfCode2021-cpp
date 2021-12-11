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
    using std::string;
    using std::vector;

    // const auto filename = "input_sample.txt";
    const auto filename = "input.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    vector<string> lines;
    {
        // read in lines
        string line;
        while (std::getline(ifs, line)) {
            // check content
            for (const char c : line) {
                switch (c) {
                    case '(':
                    case ')':
                    case '[':
                    case ']':
                    case '{':
                    case '}':
                    case '<':
                    case '>': {
                        break;
                    }
                    default: {
                        throw std::runtime_error(std::string("Invalid character: ") + c);
                    }
                }
            }
            lines.emplace_back(std::move(line));
        }
    }

    const std::unordered_map<char, char> expected_closing_char{
        {'(', ')'}, {'[', ']'}, {'{', '}'}, {'<', '>'}};

    auto find_first_invalid_char =
        [&lines, &expected_closing_char](const string& line) -> std::optional<char> {
        vector<char> expected_stack;  // could also use string
        for (const char c : line) {
            switch (c) {
                case '(':
                case '[':
                case '{':
                case '<': {
                    expected_stack.push_back(expected_closing_char.find(c)->second);
                    break;
                }
                default: {
                    // closing char
                    if (!expected_stack.empty() && expected_stack.back() == c) {
                        expected_stack.pop_back();
                    } else {
                        // invalid char
                        return c;
                    }
                    break;
                }
            }
        }
        return {};
    };

    {
        std::cout << " --- Part 1 ---\n";

        // stop at first incorrect closing bracket and count the scores of the illegal char
        int total_score = 0;
        const std::unordered_map<char, int> score_by_char{
            {')', 3}, {']', 57}, {'}', 1197}, {'>', 25137}};

        for (const string& line : lines) {
            const std::optional<char> first_invalid = find_first_invalid_char(line);
            if (first_invalid.has_value()) {
                total_score += score_by_char.find(first_invalid.value())->second;
            }
        }
        std::cout << "Total score: " << total_score << "\n";
    }

    {
        std::cout << " --- Part 2 ---\n";

        vector<string> incomplete_lines;
        for (const string& line : lines) {
            if (!find_first_invalid_char(line).has_value()) {
                // is not invalid, but may be not complete
                incomplete_lines.push_back(line);
            }
        }

        auto find_completion_string = [&expected_closing_char](const string& line) -> string {
            // find the string that completes the incomplete (but not invalid) line

            string expected_stack;
            for (const char c : line) {
                switch (c) {
                    case '(':
                    case '[':
                    case '{':
                    case '<': {
                        expected_stack.push_back(expected_closing_char.find(c)->second);
                        break;
                    }
                    default: {
                        // closing char
                        if (!expected_stack.empty() && expected_stack.back() == c) {
                            expected_stack.pop_back();
                        } else {
                            // invalid char
                            throw std::runtime_error(
                                "Cannot find completion string for invalid string");
                        }
                        break;
                    }
                }
            }
            std::reverse(expected_stack.begin(), expected_stack.end());
            return expected_stack;
        };

        auto find_completion_score =
            [&find_completion_string](const string& incomplete_line) -> int64_t {
            int64_t score = 0;
            for (const char c : find_completion_string(incomplete_line)) {
                score *= 5;
                switch (c) {
                    case ')': {
                        score += 1;
                        break;
                    }
                    case ']': {
                        score += 2;
                        break;
                    }
                    case '}': {
                        score += 3;
                        break;
                    }
                    case '>': {
                        score += 4;
                        break;
                    }
                    default: {
                        throw std::runtime_error("Insensible completion char");
                        break;
                    }
                }
            }
            return score;
        };

        vector<int64_t> scores;
        for (const string& line : incomplete_lines) {
            // std::cout << find_completion_string(line) << " -> " << find_completion_score(line) <<
            // "\n";
            scores.push_back(find_completion_score(line));
        }

        assert(scores.size() > 0);
        auto it_mid = scores.begin() + scores.size() / 2;
        std::nth_element(scores.begin(), it_mid, scores.end());
        std::cout << "Middle score: " << *it_mid << "\n";
    }
}