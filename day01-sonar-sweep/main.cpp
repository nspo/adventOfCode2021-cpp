#include <iostream>
#include <fstream>
#include <vector>
#include <optional>

int main()
{
    std::cout << "--- Part 1 ---\n";
    const auto filename = "input1.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    // read data into vector as it's needed again for part 2
    std::vector<int> input_values;
    {
        int curr_value{};
        while (ifs >> curr_value)
        {
            input_values.push_back(curr_value);
        }
    }

    std::optional<int> prev_value{};
    int increases{0}; ///< number of increases relative to previous value
    for (const int curr_value : input_values) 
    {
        if (prev_value.has_value() && prev_value.value() < curr_value) {
            // increase detected
            ++increases;
        }
        prev_value = curr_value;
    }

    std::cout << increases << " increases detected in 1-value windows in " << filename << "\n";

    std::cout << "--- Part 2 ---\n";
    increases = 0; // reset counter
    for (size_t i=3; i < input_values.size(); ++i)
    {
        // could calculate window sum without reading N values each step, but for N=3 it doesn't seem worth it
        const int prev_sum = input_values[i-1] + input_values[i-2] + input_values[i-3];
        const int curr_sum = prev_sum - input_values[i-3] + input_values[i];
        if (curr_sum > prev_sum) {
            ++increases;
        }
    }
    std::cout << increases << " increases detected in 3-value windows in " << filename << "\n";
}