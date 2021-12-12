
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

void searchGraph1(const std::string& node,
                  std::unordered_map<std::string, std::vector<std::string>>& adj,
                  std::unordered_set<std::string>& visited, int& path_count) {
    if (node == "end") {
        ++path_count;
        return;
    }
    if (visited.find(node) != visited.end()) {
        // already visited this small cave
        return;
    }
    if (node[0] >= 'a' && node[0] <= 'z') {
        // small cave -> mark as visited
        visited.insert(node);
    }

    for (const std::string& neighbor : adj[node]) {
        searchGraph1(neighbor, adj, visited, path_count);
    }

    visited.erase(node);
}

void searchGraph2(const std::string& node,
                  std::unordered_map<std::string, std::vector<std::string>>& adj,
                  std::unordered_map<std::string, int>& visited_count,
                  bool& visited_a_small_cave_twice, int& path_count) {
    if (node == "end") {
        ++path_count;
        return;
    }

    const bool is_small_cave = node[0] >= 'a' && node[0] <= 'z';

    if (is_small_cave) {
        if (visited_count[node] == 2) {
            // already visited this small cave twice
            return;
        } else if (visited_count[node] == 1) {
            // can visit this once more if no other small cave was yet visited twice
            if (visited_a_small_cave_twice) {
                return;
            } else {
                visited_a_small_cave_twice = true;
            }
        } 
        // mark as visited once more
        ++visited_count[node];
    }

    for (const std::string& neighbor : adj[node]) {
        if (neighbor == "start") continue;
        searchGraph2(neighbor, adj, visited_count, visited_a_small_cave_twice, path_count);
    }

    if (is_small_cave) {
        if (visited_count[node] == 2) {
            // this was the one visited twice
            visited_a_small_cave_twice = false;
        }
        --visited_count[node];
    }
}

int main() {
    using std::array;
    using std::string;
    using std::vector;

    // const auto filename = "input_sample.txt";
    const auto filename = "input.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    std::unordered_map<string, vector<string>> adj;
    {
        // read in lines
        string line;
        while (std::getline(ifs, line)) {
            const auto edge = split(line, "-");

            adj[edge[0]].push_back(edge[1]);
            adj[edge[1]].push_back(edge[0]);
        }
    }

    {
        std::cout << " --- Part 1 ---\n";

        std::unordered_set<string> visited;
        int path_count{0};
        searchGraph1("start", adj, visited, path_count);

        std::cout << "Path count: " << path_count << "\n";
    }

    {
        std::cout << " --- Part 2 ---\n";
        std::unordered_map<string, int> visited_count;
        int path_count{0};
        bool visited_a_small_cave_twice{false};
        searchGraph2("start", adj, visited_count, visited_a_small_cave_twice, path_count);

        std::cout << "Path count: " << path_count << "\n";
    }
}