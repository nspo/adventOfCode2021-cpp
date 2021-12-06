#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
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

    const auto filename = "input1.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    std::array<size_t, 9> timers{};  // number of fish with respective timers t at index timers[t]
    {
        // read in initial state
        string initial_str;
        if (!std::getline(ifs, initial_str)) std::terminate();
        for (const string& s : split(initial_str, ",")) {
            const size_t timer = std::stoul(s);
            if (timer > 8) throw std::runtime_error("invalid timer value");

            ++timers[timer];
        }
    }

    auto simulate_days = [](array<size_t,9> timers, const int days) -> decltype(timers) {
        for (int day = 0; day < days; ++day) {
            // propagate timers
            const size_t old_count_0 = timers[0];
            for (int idx = 0; idx < 8; ++idx) {
                timers[idx] = timers[idx + 1];
            }
            timers[6] += old_count_0;  // parents
            timers[8] = old_count_0;   // newborns
        }

        return timers;
    };

    {
        std::cout << " --- Part 1 ---\n";
        const auto timers_after_80_days = simulate_days(timers, 80);

        std::cout << "At day 80 there are "
                  << std::accumulate(timers_after_80_days.begin(), timers_after_80_days.end(), 0UL)
                  << " fish\n";
    }

    {
        std::cout << " --- Part 2 ---\n";
        const auto timers_after_256_days = simulate_days(timers, 256);

        std::cout << "At day 256 there are "
                  << std::accumulate(timers_after_256_days.begin(), timers_after_256_days.end(), 0UL)
                  << " fish\n";
    }
}