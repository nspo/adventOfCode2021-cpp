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

struct Display {
    std::array<std::string, 10> patterns;
    std::array<std::string, 4> outputs;
};

int main() {
    using std::string;
    using std::vector;

    const auto filename = "input.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    vector<Display> displays;
    {
        // read in input
        std::string line;
        while (std::getline(ifs, line)) {
            vector<string> splitted = split(line, " | ");
            vector<string> patterns_as_str = split(splitted.at(0), " ");
            vector<string> outputs_as_str = split(splitted.at(1), " ");

            assert(patterns_as_str.size() == 10);
            assert(outputs_as_str.size() == 4);
            Display display{};
            for (size_t i = 0; i < 10; ++i) {
                const auto& pattern_str = patterns_as_str[i];
                assert(pattern_str.size() <= 7);
                display.patterns[i] = pattern_str;
            }
            for (size_t i = 0; i < 4; ++i) {
                const auto& output_str = outputs_as_str[i];
                assert(output_str.size() <= 7);
                display.outputs[i] = output_str;
            }
            displays.push_back(display);
        }
    }

    {
        std::cout << " --- Part 1 ---\n";
        // only consider outputs
        // count occurrences of 2/3/4/7 chars in output
        // this corresponds to digits 1/7/4/8 resp.
        int count_2347 = 0;
        for (const Display& display : displays) {
            for (const auto& output : display.outputs) {
                switch (output.size()) {
                    case 2:
                    case 3:
                    case 4:
                    case 7: {
                        ++count_2347;
                        break;
                    }
                }
            }
        }
        std::cout << "Number of 1/7/4/8 in output digits: " << count_2347 << "\n";
    }

    {
        std::cout << " --- Part 2 ---\n";

        auto find_common = [](const vector<string>& candidates, const string& other,
                              const size_t N) -> string {
            // find the single string in candidates which has N common chars with other
            bool found{false};
            string solution;
            for (const string& cand : candidates) {
                vector<char> intersection;
                std::set_intersection(cand.begin(), cand.end(), other.begin(), other.end(),
                                      std::back_inserter(intersection));

                // std::cout << "Cand: " << cand << " matches " << intersection.size() << " with "
                // << other << "\n";

                if (intersection.size() == N) {
                    if (found)
                        throw std::runtime_error(std::string("Multiple matches for ") +
                                                 std::to_string(N) + " with " + other);
                    solution = cand;
                    found = true;
                }
            }
            if (!found)
                throw std::runtime_error(std::string("No candidate matches ") + std::to_string(N) +
                                         " with " + other);
            return solution;
        };

        int sum_of_outputs = 0;
        for (const Display& display : displays) {
            const string s_all = "abcdefg";
            vector<string> candidates(display.patterns.begin(), display.patterns.end());

            // sort them so we can use std::set_intersection
            for (string& s : candidates) {
                std::sort(s.begin(), s.end());
            }

            const string s1 = find_common(candidates, s_all, 2);
            const string s7 = find_common(candidates, s_all, 3);
            const string s4 = find_common(candidates, s_all, 4);
            const string s8 = find_common(candidates, s_all, 7);

            // not very efficient... but delete known strings
            auto remove_from_candidates = [&candidates](const string& s) -> void {
                candidates.erase(std::find(candidates.begin(), candidates.end(), s));
            };

            for (const string& s : {s1, s7, s4, s8}) {
                remove_from_candidates(s);
            }

            auto only_n_letters = [](const vector<string>& all, const size_t N) -> vector<string> {
                // filter all so that only those strings with N letters remain
                vector<string> result;
                std::copy_if(all.begin(), all.end(), std::back_inserter(result),
                             [N](const string& s) { return s.size() == N; });
                return result;
            };
            const string s3 = find_common(only_n_letters(candidates, 5), s1, 2);
            remove_from_candidates(s3);

            const string s6 = find_common(only_n_letters(candidates, 6), s7, 2);
            remove_from_candidates(s6);

            const string s0 = find_common(only_n_letters(candidates, 6), s4, 3);
            remove_from_candidates(s0);

            const auto vec_6_let = only_n_letters(candidates, 6);
            assert(vec_6_let.size() == 1);
            const string s9 = vec_6_let[0];

            const string s2 = find_common(only_n_letters(candidates, 5), s4, 2);
            remove_from_candidates(s2);

            const auto vec_5_let = only_n_letters(candidates, 5);
            assert(vec_5_let.size() == 1);
            const string s5 = vec_5_let[0];

            std::unordered_map<string, int> s2val;
            s2val[s0] = 0;
            s2val[s1] = 1;
            s2val[s2] = 2;
            s2val[s3] = 3;
            s2val[s4] = 4;
            s2val[s5] = 5;
            s2val[s6] = 6;
            s2val[s7] = 7;
            s2val[s8] = 8;
            s2val[s9] = 9;
            const auto convert_string_to_num = [s2val](string s) -> int {
                std::sort(s.begin(), s.end());
                if (auto it = s2val.find(s); it != s2val.end()) {
                    return it->second;
                }
                throw std::runtime_error(std::string("No lookup value found for " + s));
            };

            {
                // convert output into decimal value
                int val = 0;
                for (auto it = display.outputs.begin(); it != display.outputs.end(); ++it) {
                    val = val * 10 + convert_string_to_num(*it);
                }
                sum_of_outputs += val;
            }
        }
        std::cout << "Sum of all display outputs: " << sum_of_outputs << "\n";
    }
}