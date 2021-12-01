#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <optional>

int countIncreases(const std::vector<int> &values, const int window_size)
{
    assert(window_size >= 1);
    assert(values.size() <= static_cast<size_t>(std::numeric_limits<int>::max()));

    // accumulate initial sum
    int prev_sum = 0;
    for (int i = 0; i < window_size; ++i)
    {
        prev_sum += values[i];
    }

    // actually start to compare values
    int increases = 0;
    for (int i = window_size; i < static_cast<int>(values.size()); ++i)
    {
        const int old_elem = values[i - window_size]; // will not be in new sum anymore
        const int new_elem = values[i];               // will only be in new sum
        const int new_sum = prev_sum - old_elem + new_elem;
        if (new_sum > prev_sum)
        {
            ++increases;
        }
        prev_sum = new_sum;
    }

    return increases;
}

int main()
{
    const auto filename = "input1.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    std::vector<int> values;
    {
        int curr_value{};
        while (ifs >> curr_value)
        {
            values.push_back(curr_value);
        }
    }

    std::cout << "--- Part 1 ---\n";
    std::cout << countIncreases(values, 1) << " increases detected in 1-value windows in " << filename << "\n";

    std::cout << "--- Part 2 ---\n";
    std::cout << countIncreases(values, 3) << " increases detected in 3-value windows in " << filename << "\n";
}