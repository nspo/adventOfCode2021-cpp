#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

// convert string of binary digits to decimal number
int binStr2dec(const std::string& binStr) {
    int exp = 0;
    int result = 0;
    for (auto it = binStr.rbegin(); it != binStr.rend(); ++it) {
        assert(*it == '0' || *it == '1');
        result += (*it == '1' ? 1 : 0) * std::pow(2, exp);
        ++exp;
    }

    return result;
}

int main() {
    using std::string;
    using std::vector;

    const auto filename = "input1.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    vector<string> readings;
    {
        string line{};
        while (std::getline(ifs, line)) {
            readings.emplace_back(std::move(line));
        }
    }

    const int num_readings = readings.size();
    assert(num_readings > 0);

    const int num_bits = readings[0].size();
    assert(num_bits > 0);

    // count number of zeros at each index
    vector<int> zero_count_at_place(num_bits, 0);

    for (const auto& reading : readings) {
        assert(reading.size() == static_cast<size_t>(num_bits));
        for (int i = 0; i < num_bits; ++i) {
            if (reading[i] == '0') {
                ++zero_count_at_place[i];
            }
        }
    }

    {
        std::cout << "--- Part 1 ---\n";

        // interpret results (ignores case if exactly 50% zero readings)
        string gamma_reading;
        string epsilon_reading;
        const int needed_for_most_common = num_readings / 2 + 1;

        for (int i = 0; i < num_bits; ++i) {
            if (zero_count_at_place[i] >= needed_for_most_common) {
                gamma_reading.push_back('0');
                epsilon_reading.push_back('1');
            } else {
                gamma_reading.push_back('1');
                epsilon_reading.push_back('0');
            }
        }

        std::cout << "Gamma reading: " << gamma_reading << "b (" << binStr2dec(gamma_reading)
                  << "d), epsilon reading: " << epsilon_reading << "b ("
                  << binStr2dec(epsilon_reading) << "d)\n";
        std::cout << "Power consumption: "
                  << binStr2dec(gamma_reading) * binStr2dec(epsilon_reading) << "\n";
    }

    {
        std::cout << "--- Part 2 ---\n";

        // find oxygen rating by only keeping readings with most common bit
        // left-to-right, if equal keeping 1s
        vector<string> candidates = readings;
        {
            int index = 0;
            while (candidates.size() > 1) {
                if (index == num_bits) throw std::runtime_error("Too many possibilities");
                const int needed_for_most = static_cast<int>(candidates.size()) / 2 + 1;
                const int zero_count = std::count_if(
                    candidates.begin(), candidates.end(),
                    [index](const string& reading) -> bool { return reading[index] == '0'; });
                const char only_keep = zero_count >= needed_for_most ? '0' : '1';
                vector<string> remaining;
                std::copy_if(candidates.begin(), candidates.end(), std::back_inserter(remaining),
                             [only_keep, index](const string& reading) -> bool {
                                 return reading[index] == only_keep;
                             });

                std::swap(candidates, remaining);
                ++index;
            }
        }

        const int oxygen_rating = binStr2dec(candidates[0]);
        std::cout << "Oxygen reading: " << candidates[0] << "b (" << oxygen_rating << "d)"
                  << "\n";

        // find CO2 scrubber rating by keeping readings with least common value
        // bitwise left-to-right (0 if equal)
        // this has quite a bit of code duplication but removing that might make
        // it unreadable
        candidates = readings;
        {
            int index = 0;
            while (candidates.size() > 1) {
                if (index == num_bits) throw std::runtime_error("Too many possibilities");
                const int needed_for_least = static_cast<int>(candidates.size()) / 2;
                const int zero_count = std::count_if(
                    candidates.begin(), candidates.end(),
                    [index](const string& reading) -> bool { return reading[index] == '0'; });
                const char only_keep = zero_count <= needed_for_least ? '0' : '1';
                vector<string> remaining;
                std::copy_if(candidates.begin(), candidates.end(), std::back_inserter(remaining),
                             [only_keep, index](const string& reading) -> bool {
                                 return reading[index] == only_keep;
                             });

                std::swap(candidates, remaining);
                ++index;
            }
        }
        const int co2_rating = binStr2dec(candidates[0]);
        std::cout << "CO2 scrubber reading: " << candidates[0] << "b (" << co2_rating << "d)"
                  << "\n";

        const int life_support_rating = oxygen_rating * co2_rating;
        std::cout << "Life support rating: " << life_support_rating << "\n";
    }
}