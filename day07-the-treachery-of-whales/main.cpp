#include <cmath>
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
    using std::string;
    using std::vector;

    const auto filename = "input1.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    std::vector<int> positions;
    {
        // read in initial state
        string str;
        if (!std::getline(ifs, str)) std::terminate();
        for (const string& s : split(str, ",")) {
            const int pos = std::stoi(s);
            positions.push_back(pos);
        }
    }


    {
        std::cout << " --- Part 1 ---\n";
        // find the median (or any of the two middle values if size is even)
        // could sort, but quickselect is better (which is hopefully done by std::nth_element)
        assert(positions.size() > 0);
        auto it_median = positions.begin() + positions.size()/2;
        std::nth_element(positions.begin(), it_median, positions.end());

        const int target_pos = *it_median;
        auto cost = [&positions](const int target) -> int {
            // cost if going to target
            int sum = 0;
            for (const int pos : positions) {
                sum += std::abs(pos - target);
            }
            return sum;
        };

        std::cout << "(Possible) target position: " << target_pos << ", optimal cost: " << cost(target_pos) << " fuel\n";
    }

    {
        std::cout << " --- Part 2 ---\n";
        // Calculate mean but check integer values below and above for lowest cost
        auto avg = [](const vector<int>& values) -> double {
            if (values.empty()) throw std::invalid_argument("Cannot calc avg of empty vector");
            // assumes sum is small enough. Could use alternative formula.
            return std::reduce(values.begin(), values.end(), 0) / static_cast<double>(values.size());
        };

        const double dbl_mean = avg(positions);
        const int cand1 = std::floor(dbl_mean);
        const int cand2 = std::ceil(dbl_mean);

        auto cost = [&positions](const int target) -> int {
            int sum = 0;
            for (const int pos : positions) {
                const int dist = std::abs(target - pos);
                sum += dist*(dist+1)/2; // Gaussian sum for 1,2,...,dist
            }
            return sum;
        };
        std::cout << "Minimal cost: " << std::min(cost(cand1), cost(cand2)) << "\n";
    }
}