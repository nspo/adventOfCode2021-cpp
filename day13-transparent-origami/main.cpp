#include <array>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <set>
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

class Grid {
   public:
    struct Fold {
        bool alongX;
        int pos;
    };

    void setPoint(const int x, const int y) {
        m_data[x].insert(y);
        m_x_size = std::max(m_x_size, x);
        m_y_size = std::max(m_y_size, y);
    }

    void print() const {
        checkSize();

        for (int y = 0; y < m_y_size; ++y) {
            for (int x = 0; x < m_x_size; ++x) {
                const auto it_col = m_data.find(x);
                const bool filled = it_col != m_data.end() && it_col->second.contains(y);

                if (filled)
                    std::cout << "#";
                else
                    std::cout << " ";
            }
            std::cout << "\n";
        }
    }

    int count_visible_dots() const {
        checkSize();
        int count = 0;
        for (const auto& [x, column] : m_data) {
            count += static_cast<int>(column.size());
        }
        return count;
    }

    void execute_fold(const Fold& fold) {
        checkSize();

        if (fold.alongX) {
            fold_x(fold.pos);
        } else {
            fold_y(fold.pos);
        }
    }

   private:
    std::map<int, std::set<int>> m_data;
    int m_x_size{0};
    int m_y_size{0};

    void checkSize() const {
        if (m_x_size == 0 || m_y_size == 0) {
            throw std::runtime_error("Size may not be 0 for this operation");
        }
    }

    // fold the grid part right of at_x left
    void fold_x(const int at_x) {
        assert(at_x > 0);

        // walk through everything right of the vertical line at at_x
        // project x-coord left and set cell there to true

        const auto project_x_to_left = [at_x](const int x) -> int {
            assert(x >= at_x);
            const int dist = x - at_x;
            const int new_x = at_x - dist;
            assert(new_x >= 0);
            return new_x;
        };

        for (const auto& [x, column] : m_data) {
            if (x <= at_x) {
                // should not be projected
                continue;
            }
            // this column should be projected left
            const int to_x = project_x_to_left(x);
            for (const int y : column) {
                m_data[to_x].insert(y);
            }
        }

        // delete the unnecessary columns which were folded
        m_data.erase(m_data.lower_bound(at_x), m_data.end());

        m_x_size = at_x;
    }

    // fold the grid part below at_y up
    void fold_y(const int at_y) {
        assert(at_y > 0);

        const auto project_y_up = [at_y](const int y) -> int {
            assert(y >= at_y);
            const int dist = y - at_y;
            const int new_y = at_y - dist;
            assert(new_y >= 0);
            return new_y;
        };

        std::vector<int> empty_x_columns;
        for (auto& [x, column] : m_data) {
            for (const int y : column) {
                if (y <= at_y) continue;

                const int to_y = project_y_up(y);
                m_data[x].insert(to_y);
            }
            column.erase(column.lower_bound(at_y), column.end());
            if (column.size() == 0) empty_x_columns.push_back(x);
        }

        for (const int x : empty_x_columns) {
            m_data.erase(x);
        }

        m_y_size = at_y;
    }
};

int main() {
    using std::array;
    using std::string;
    using std::vector;

    // const auto filename = "input_sample.txt";
    const auto filename = "input.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    Grid grid;
    vector<Grid::Fold> folds;

    {
        // read in lines
        string line;
        while (std::getline(ifs, line) && line != "") {
            // read grid
            const auto splitted = split(line, ",");
            const int x = std::stoi(splitted.at(0));
            const int y = std::stoi(splitted.at(1));
            grid.setPoint(x, y);
        }

        while (std::getline(ifs, line)) {
            // read folds
            const auto splitted = split(line, "fold along ");
            const bool alongX = splitted.at(1)[0] == 'x';
            const int pos = std::stoi(split(splitted.at(1), "=").at(1));
            folds.emplace_back(Grid::Fold{.alongX = alongX, .pos = pos});
        }

        std::cout << "Read grid and " << folds.size() << " folds\n";
    }

    {
        std::cout << " --- Part 1 ---\n";

        grid.execute_fold(folds.at(0));
        std::cout << "Count of visible dots after 1 fold: " << grid.count_visible_dots() << "\n";
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
            grid.execute_fold(fold);
        }
        grid.print();
    }
}