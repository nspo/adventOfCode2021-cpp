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

using Vec3 = std::array<int, 3>;                // x,y,z
using Rot = std::array<std::array<int, 3>, 3>;  // Rotation matrix, row-first. I.e. new_x =
                                                // rotm[0][0]*old_x + rotm[0][1]*old_y ...

std::ostream& operator<<(std::ostream& os, const Vec3& coord) {
    os << "(" << coord[0] << "," << coord[1] << "," << coord[2] << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Rot& rot) {
    os << "(" << rot[0] << "," << rot[1] << "," << rot[2] << ")";
    return os;
}

// Rotate a coordinate using rotation matrix
constexpr Vec3 rotate(const Rot& rotm, const Vec3& old_coord) {
    Vec3 new_coord{};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            new_coord[i] += rotm[i][j] * old_coord[j];
        }
    }
    return new_coord;
}

constexpr Vec3 subtract(const Vec3& lhs, const Vec3& rhs) {
    return {lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]};
}

constexpr Vec3 add(const Vec3& lhs, const Vec3& rhs) {
    return {lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]};
}

constexpr int abs(const Vec3& obj) {
    return std::sqrt(obj[0] * obj[0] + obj[1] * obj[1] + obj[2] * obj[2]);
}

constexpr Vec3 negate(Vec3 obj) {
    obj[0] *= -1;
    obj[1] *= -1;
    obj[2] *= -1;
    return obj;
}

// Rotate a rotation matrix rhs using lhs
constexpr Rot mult(const Rot& lhs, const Rot& rhs) {
    Rot new_rotm{};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                new_rotm[i][j] += lhs[i][k] * rhs[k][j];
            }
        }
    }
    return new_rotm;
}

// Transpose a rotation matrix (i.e. invert it, assuming it is valid)
constexpr Rot transpose(Rot rot) {
    for (int row = 0; row < 3; ++row) {
        for (int col = row + 1; col < 3; ++col) {
            std::swap(rot[row][col], rot[col][row]);
        }
    }
    return rot;
}

// For combining hashes; stolen from boost
template <typename T>
void hash_combine(size_t& seed, T const& v) {
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std {
template <>
struct hash<Vec3> {
    size_t operator()(const Vec3& obj) const {
        size_t seed = 0;
        for (const auto elem : obj) {
            hash_combine(seed, elem);
        }
        return seed;
    }
};
}  // namespace std

constexpr Rot ROTATION_IDENTITY = Rot{{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};

// All relevant rotation matrices
constexpr std::array<Rot, 24> ALL_ROTATIONS = []() {
    using std::array;

    array<Rot, 24> res{};

    const Rot once_around_x = Rot{{{1, 0, 0}, {0, 0, 1}, {0, -1, 0}}};
    const Rot once_around_y = Rot{{{0, 0, -1}, {0, 1, 0}, {1, 0, 0}}};
    const Rot once_around_z = Rot{{{0, 1, 0}, {-1, 0, 0}, {0, 0, 1}}};

    int idx = 0;
    // Strategy: bring x axis into the 6 possible directions x, z, -x, -z, y, -y
    // then for each of these, rotate four times around (new) x axis
    Rot rot = ROTATION_IDENTITY;  // start with identity matrix
    for (int y = 0; y < 4; ++y) {
        // x_new = x, z, -x, -z
        for (int x = 0; x < 4; ++x) {
            res[idx++] = rot;
            rot = mult(once_around_x, rot);
        }
        rot = mult(once_around_y, rot);
    }

    rot = mult(once_around_z, rot);
    // x_new = y
    for (int x = 0; x < 4; ++x) {
        res[idx++] = rot;
        rot = mult(once_around_x, rot);
    }
    rot = mult(once_around_z, rot);
    rot = mult(once_around_z, rot);
    // x_new = -y
    for (int x = 0; x < 4; ++x) {
        res[idx++] = rot;
        rot = mult(once_around_x, rot);
    }

    return res;
}();

auto best_match(const std::vector<Vec3>& points1, const std::vector<Vec3>& points2) {
    assert(!points1.empty() && !points2.empty());
    std::unordered_map<Vec3, int> count_delta;
    for (const Vec3 a : points1) {
        for (const Vec3& b : points2) {
            const Vec3 delta = subtract(b, a);
            ++count_delta[delta];
        }
    }
    int max_count = -1;
    Vec3 max_count_delta{};
    for (const auto& [delta, count] : count_delta) {
        if (count > max_count) {
            max_count = count;
            max_count_delta = delta;
        }
    }

    struct BestMatchResult {
        int count_matches;  // how many points can be matched if delta is added to points1
        Vec3 delta;
    };
    return BestMatchResult{.count_matches = max_count, .delta = max_count_delta};
}

struct ScannerMatchResult {
    Vec3 translation;  // delta to apply to original points
    Rot rotation;      // rotation to apply to original points
    size_t other;      // index of other scanner which can be sufficiently matched with this
};

// rotate and translate each scanner's beacons to match with other scanner's beacons
/// @return adjacency list of matches with >= 12 beacons
std::vector<std::vector<ScannerMatchResult>> compare_beacons(
    const std::vector<std::vector<Vec3>>& scanner_data) {
    using std::vector;

    vector<vector<ScannerMatchResult>> graph(
        scanner_data.size(), vector<ScannerMatchResult>{});  // graph of valid match results

    for (size_t i1 = 0; i1 < scanner_data.size(); ++i1) {
        // use scanner at i1 as base
        // rotate all detected beacons in all possible ways, then compare with other scanners
        for (const Rot& rot : ALL_ROTATIONS) {
            vector<Vec3> rotated_beacons;
            rotated_beacons.reserve(scanner_data[i1].size());
            for (const Vec3& beacon : scanner_data[i1]) {
                rotated_beacons.emplace_back(rotate(rot, beacon));
            }

            for (size_t i2 = i1 + 1; i2 < scanner_data.size(); ++i2) {
                const auto match_result = best_match(rotated_beacons, scanner_data[i2]);
                if (match_result.count_matches >= 12) {
                    // std::cout << match_result.count_matches << " matches between " << i1 << " and "
                    //           << i2 << "\n";

                    graph[i1].emplace_back(ScannerMatchResult{
                        .translation = match_result.delta, .rotation = rot, .other = i2});
                    graph[i2].emplace_back(ScannerMatchResult{
                        .translation = negate(rotate(transpose(rot), match_result.delta)),
                        .rotation = transpose(rot),
                        .other = i1});
                }
            }
        }
    }

    return graph;
}

/// @return whether the match result is enough to match all scanner's data
bool is_valid_result(const std::vector<std::vector<ScannerMatchResult>>& graph) {
    using std::vector;
    assert(!graph.empty());
    vector<bool> visited(graph.size(), false);

    auto dfs = [&visited, &graph](const size_t i, auto& self) {
        assert(i < visited.size());
        if (visited[i]) return;
        visited[i] = true;
        for (const auto& other_result : graph[i]) {
            self(other_result.other, self);
        }
    };

    dfs(0, dfs);

    return std::all_of(visited.begin(), visited.end(), [](const bool b) -> bool { return b; });
}

std::unordered_set<Vec3> build_beacon_map(const std::vector<std::vector<ScannerMatchResult>>& graph,
                                          const std::vector<std::vector<Vec3>>& scanner_data) {
    using std::vector;
    std::unordered_set<Vec3> map;

    (void)graph;
    (void)scanner_data;

    vector<bool> visited(graph.size(), false);

    struct QueueElem {
        Vec3 translation;  // translation from scanner 0 to this (in scanner 0 coordinates)
        Rot rotation;      // rotation from scanner 0 to this (in scanner 0 coordinates)
        size_t i;          // index of this scanner
    };

    std::queue<QueueElem> queue;
    queue.emplace(QueueElem{.translation = Vec3{}, .rotation = ROTATION_IDENTITY, .i = 0});

    while (!queue.empty()) {
        const QueueElem elem = queue.front();
        queue.pop();
        if (visited[elem.i]) continue;

        visited[elem.i] = true;
        // std::cout << "P in 0 to P in " << elem.i << ": translation=" << elem.translation
        //           << ", then rotation=" << elem.rotation << "\n";
        // add all beacons to map
        for (Vec3 point : scanner_data[elem.i]) {
            point = rotate(transpose(elem.rotation), point);
            point = subtract(point, elem.translation);

            map.insert(point);
        }

        // add matches which can be reached via this match to queue
        for (const auto& nb_match : graph[elem.i]) {
            Rot new_rotation = mult(nb_match.rotation, elem.rotation);
            Vec3 new_translation =
                add(elem.translation, rotate(transpose(new_rotation), nb_match.translation));
            queue.emplace(QueueElem{
                .translation = new_translation, .rotation = new_rotation, .i = nb_match.other});
        }
    }

    return map;
}

int main() {
    using std::array;
    using std::string;
    using std::vector;

    const auto filename = "input.txt";

    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    vector<vector<Vec3>> scanner_data;  // detected beacons by scanner, relative positions
    {
        string line;
        while (std::getline(ifs, line)) {
            assert(line.at(0) == '-' && line.at(1) == '-');  // --- scanner S ---
            scanner_data.emplace_back();
            while (std::getline(ifs, line) && line != "") {
                // beacon reading
                vector<string> coords_as_str = split(line, ",");
                assert(coords_as_str.size() == 3);
                scanner_data.back().emplace_back(Vec3{std::stoi(coords_as_str[0]),
                                                      std::stoi(coords_as_str[1]),
                                                      std::stoi(coords_as_str[2])});
            }
        }
        std::cout << "Read beacons from " << scanner_data.size() << " scanners\n";
    }

    {
        std::cout << " --- Part 1 ---\n";

        const auto matches = compare_beacons(scanner_data);
        if (is_valid_result(matches)) {
            std::cout << "Found valid match for all scanners!\n";
        } else {
            throw std::runtime_error("Could not build transformation tree between all scanners");
        }

        const auto beacon_map = build_beacon_map(matches, scanner_data);
        std::cout << beacon_map.size() << " beacons found in finished map\n";
    }

    { std::cout << " --- Part 2 ---\n"; }
}