#include <array>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

struct Fold {
    bool alongX;
    int pos;
};

class Grid {
   public:
    void addPoint(const int x, const int y) {
        m_data[x][y] = true;
        m_x_size = std::max(m_x_size, x);
        m_y_size = std::max(m_y_size, y);
    }
   private:
    std::unordered_map<int, std::unordered_map<int, bool>> m_data;
    int m_x_size{0};
    int m_y_size{0};

    // void check() const {
    //     if (m_x_size)
    // }
};

using GridType = std::unordered_map<int, std::unordered_map<int, bool>>;

void print(const GridType& grid) {
    // calc size
    int x_size = -1;
    int y_size = -1;

    for (const auto& [x, column] : grid) {
        x_size = std::max(x_size, x + 1);
        for (const auto& [y, val] : column) {
            y_size = std::max(y_size, y + 1);
        }
    }

    for (int y = 0; y < y_size; ++y) {
        for (int x = 0; x < x_size; ++x) {
            bool filled = false;
            if (auto it_col = grid.find(x); it_col != grid.end()) {
                // column at x exists
                if (auto it_elem = it_col->second.find(y);
                    it_elem != it_col->second.end() && it_elem->second == true) {
                    // elem is defined and true
                    filled = true;
                }
            }

            if (filled)
                std::cout << "#";
            else
                std::cout << ".";
        }
        std::cout << "\n";
    }
}

// fold the grid part right of at_x left
void fold_x(GridType& grid, const int at_x) {
    assert(at_x >= 0);

    // walk through everything right of the vertical line at at_x
    // project x-coord left and set cell there to true

    const auto project_x_to_left = [at_x](const int x) -> int {
        assert(x >= at_x);
        const int dist = x - at_x;
        const int new_x = at_x - dist;
        assert(new_x >= 0);
        return new_x;
    };

    std::vector<int> to_delete_x{at_x};
    for (const auto& [x, column] : grid) {
        if (x <= at_x) {
            // should not be projected
            continue;
        }
        // this column should be projected left
        to_delete_x.push_back(x);
        const int to_x = project_x_to_left(x);
        for (const auto& [y, val] : column) {
            if (val) {
                grid[to_x][y] = true;
            }
        }
    }

    // delete the unnecessary columns which were folded
    for (const int x : to_delete_x) {
        grid.erase(x);
    }

    if (at_x > 0) {
        // add a pseudo entry to make sure empty but existing lines are printed
        grid[at_x - 1][0];
    }
}

// fold the grid part below at_y up
void fold_y(GridType& grid, const int at_y) {
    assert(at_y >= 0);

    const auto project_y_up = [at_y](const int y) -> int {
        assert(y >= at_y);
        const int dist = y - at_y;
        const int new_y = at_y - dist;
        // assert(new_y >= 0);
        return new_y;
    };

    for (auto& [x, column] : grid) {
        std::vector<int> to_delete_y{at_y};

        for (const auto& [y, val] : column) {
            if (y <= at_y) continue;

            to_delete_y.push_back(y);
            if (val) {
                const int to_y = project_y_up(y);
                grid[x][to_y] = true;
            }
        }
        for (const int y : to_delete_y) {
            column.erase(y);
        }
    }

    if (at_y > 0) {
        // add a pseudo entry to make sure empty but existing lines are printed
        grid[0][at_y - 1];
    }
}

void execute_fold(GridType& grid, const Fold& fold) {
    if (fold.alongX) {
        fold_x(grid, fold.pos);
    } else {
        fold_y(grid, fold.pos);
    }
}

int count_visible_dots(const GridType& grid) {
    int count = 0;
    for (const auto& [x, column] : grid) {
        for (const auto& [y, val] : column) {
            if (val) {
                ++count;
            }
        }
    }
    return count;
}

int main() {
    using std::array;
    using std::string;
    using std::vector;

    // const auto filename = "input_sample.txt";
    const auto filename = "input.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    GridType grid;
    vector<Fold> folds;

    {
        // read in lines
        string line;
        while (std::getline(ifs, line) && line != "") {
            // read grid
            const auto splitted = split(line, ",");
            const int x = std::stoi(splitted.at(0));
            const int y = std::stoi(splitted.at(1));
            grid[x][y] = true;
        }

        while (std::getline(ifs, line)) {
            // read folds
            const auto splitted = split(line, "fold along ");
            const bool alongX = splitted.at(1)[0] == 'x';
            const int pos = std::stoi(split(splitted.at(1), "=").at(1));
            folds.emplace_back(Fold{.alongX = alongX, .pos = pos});
        }

        std::cout << "Read grid with " << folds.size() << " folds\n";
    }

    {
        std::cout << " --- Part 1 ---\n";

        execute_fold(grid, folds.at(0));
        std::cout << "Count of visible dots after 1 fold: " << count_visible_dots(grid) << "\n";
    }

    {
        std::cout << " --- Part 2 ---\n";
        bool first = true;
        for (const auto& fold : folds) {
            if (first) {
                // skip first fold due to part 1
                first = false;
                continue;
            }
            execute_fold(grid, fold);
        }
        print(grid);
    }
}