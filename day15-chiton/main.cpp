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

// Element which can be put into priority queue
struct QueueCell {
    int row;
    int col;
    int cost_until;
};

bool operator>(const QueueCell& lhs, const QueueCell& rhs) {
    return lhs.cost_until > rhs.cost_until;
}

int lowestCostDijkstra(const std::vector<std::vector<int>>& cost) {
    using std::vector;
    using std::array;
    using std::priority_queue;

    const int rows = static_cast<int>(cost.size());
    assert(rows > 0);
    const int cols = static_cast<int>(cost[0].size());
    assert(cols > 0);

    // build cost map - value is lowest cost from start to this cell
    vector<vector<int>> lowest_cost_until(rows, vector<int>(cols, std::numeric_limits<int>::max()));

    priority_queue<QueueCell, vector<QueueCell>, std::greater<>> queue; // min PQ
    queue.push(QueueCell{.row = 0, .col = 0, .cost_until = 0});

    while(!queue.empty()) {
        const auto top = queue.top(); queue.pop();

        if (top.cost_until >= lowest_cost_until[rows-1][cols-1]) {
            // ideal solution to target found
            break;
        }

        if (lowest_cost_until[top.row][top.col] <= top.cost_until) {
            // was already expanded and cost now is not better
            continue;
        }
        lowest_cost_until[top.row][top.col] = top.cost_until;

        const array<array<int, 2>, 4> deltas = {{{-1, 0}, {+1, 0}, {0, -1}, {0, +1}}};
        for (const auto& delta : deltas) {
            // add neighbor to queue if valid
            const int nb_row = top.row + delta[0];
            const int nb_col = top.col + delta[1];

            if (0 <= nb_row && nb_row < rows && 0 <= nb_col && nb_col < cols) {
                const int cost_to_next = top.cost_until + cost[nb_row][nb_col];
                queue.push(QueueCell { .row = nb_row, .col = nb_col, .cost_until = cost_to_next});
            }
        }
    }

    return lowest_cost_until[rows-1][cols-1];
}


int main() {
    using std::array;
    using std::string;
    using std::vector;

    // const auto filename = "input_sample.txt";
    const auto filename = "input.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    vector<vector<int>> cost;
    {
        string line;
        while (std::getline(ifs, line)) {
            if (!cost.empty()) assert(line.size() == cost.front().size());

            cost.emplace_back();
            cost.back().reserve(line.size());
            for (const char c : line) {
                assert('0' <= c && c <= '9');
                cost.back().push_back(c - '0');
            }
        }
        assert(!cost.empty());
    }

    {
        std::cout << " --- Part 1 ---\n";

        std::cout << "Lowest cost from upper left to lower right: " << lowestCostDijkstra(cost) << "\n";
    }

    {
        std::cout << " --- Part 2 ---\n";

        // create larger map - yes, this could also be done by changing the algorithm
        const int old_rows = static_cast<int>(cost.size());
        const int old_cols = static_cast<int>(cost[0].size());
        vector<vector<int>> new_cost(old_rows*5, vector<int>(old_cols*5, 0));
        for (int drow = 0; drow < 5; ++drow) {
            for (int dcol = 0; dcol < 5; ++dcol) {
                const int added_cost = drow + dcol;
                for (int row=0; row < old_rows; ++row) {
                    for (int col = 0; col < old_cols; ++col) {
                        const int old_val = cost[row][col];
                        const int virt_new_val = old_val + added_cost;
                        const int new_val = virt_new_val > 9 ? virt_new_val - 9 : virt_new_val;
                        new_cost[drow * old_rows + row][dcol * old_cols + col] = new_val;
                    }
                }
            }
        }
        std::cout << "Lowest cost after growing map: " << lowestCostDijkstra(new_cost) << "\n";
    }
}