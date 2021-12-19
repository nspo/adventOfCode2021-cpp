#include <bits/stdc++.h>

// string split like in Python, nearly identical to https://stackoverflow.com/a/46931770/997151
std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    size_t pos_start = 0;
    size_t pos_end{};
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        std::string token = s.substr(pos_start, pos_end - pos_start);
        res.emplace_back(std::move(token));
        pos_start = pos_end + delimiter.length();
    }

    res.emplace_back(s.substr(pos_start));
    return res;
}

// find roots of a*x^2 + b*x + c = 0
std::pair<double, double> find_root(const int a, const int b, const int c) {
    const int under_root = b * b - 4 * a * c;
    if (under_root < 0) {
        throw std::runtime_error("No real solution");
    }
    const double first_root = 1.0 * (-b + std::sqrt(under_root)) / 2 * a;
    const double second_root = 1.0 * (-b - std::sqrt(under_root)) / 2 * a;
    return {first_root, second_root};
}

int main() {
    using std::array;
    using std::string;
    using std::vector;

    const auto filename = "input.txt";

    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    array<int, 2> x_range;
    array<int, 2> y_range;
    {
        string line;
        if (!std::getline(ifs, line)) std::terminate();
        const string x_str = split(split(line, "x=").at(1), ", ").at(0);
        x_range[0] = std::stoi(split(x_str, "..").at(0));
        x_range[1] = std::stoi(split(x_str, "..").at(1));
        const string y_str = split(line, "y=").at(1);
        y_range[0] = std::stoi(split(y_str, "..").at(0));
        y_range[1] = std::stoi(split(y_str, "..").at(1));

        std::cout << "Read target: x=" << x_range[0] << ".." << x_range[1] << ", y=" << y_range[0]
                  << ".." << y_range[1] << "\n";
        // some assumptons for this solution
        assert(x_range[0] >= 0 && x_range[1] > 0);
        assert(y_range[0] < 0 && y_range[1] <= 0);
    }

    {
        std::cout << " --- Part 1 ---\n";

        // with the given physics, a probe shot upwards with vy_start >= 0 will be at height 0 with
        // vy_new0 = -(vy_start+1) when it falls back down
        // the maximum height can thus be reached by shooting upward with such a velocity that the
        // probe later reaches 0 again and then immediately falls into the lowest row of the target
        // area
        // x velocity need not be considered
        assert(y_range[0] < 0);
        const int best_vy = -y_range[0] - 1;
        const int max_height = best_vy * (best_vy + 1) / 2;  // gaussian sum again
        std::cout << "Max height: " << max_height << "\n";
    }

    {
        std::cout << " --- Part 2 ---\n";
        // maybe there's a better way... but why not limit the possible vx and vy and then just
        // check by simulation
        // vx*(vx+1)/2 = final_x
        // => vx^2 + vx - 2*final_x = 0

        assert(x_range[0] > 0 && x_range[1] > 0);
        // calculate minimum/maximum x velocity
        const auto vx_min_cands_double = find_root(1, 1, -2 * x_range[0]);
        const double vx_min_double =
            vx_min_cands_double.first > 0 ? vx_min_cands_double.first : vx_min_cands_double.second;

        int vx_min = std::round(vx_min_double);
        while ((vx_min * (vx_min + 1) / 2) < x_range[0]) {
            ++vx_min;
        }

        const int vx_max = x_range[1];

        assert(y_range[0] < 0 && y_range[1] < 0);
        const int vy_min = y_range[0];
        const int vy_max = -y_range[0] - 1;

        std::cout << "vx_min=" << vx_min << ", vx_max=" << vx_max << "\n";
        std::cout << "vy_min=" << vy_min << ", vy_max=" << vy_max << "\n";

        auto goes_into_target = [x_range, y_range](int vx, int vy) -> bool {
            // whether this initial velocity leads to probe ever being in target area
            int x = 0;
            int y = 0;
            while (x <= x_range[1] && y >= y_range[0]) {
                if (x_range[0] <= x && x <= x_range[1] && y_range[0] <= y && y <= y_range[1]) {
                    return true;
                }
                // step
                x += vx;
                y += vy;

                if (vx > 0)
                    vx -= 1;
                else if (vx < 0)
                    vx += 1;

                vy -= 1;
            }
            return false;
        };

        int count_into_target = 0;
        for (int vx = vx_min; vx <= vx_max; ++vx) {
            for (int vy = vy_min; vy <= vy_max; ++vy) {
                if (goes_into_target(vx, vy)) {
                    ++count_into_target;
                }
            }
        }
        std::cout << count_into_target << " initial velocities lead into target\n";
    }
}