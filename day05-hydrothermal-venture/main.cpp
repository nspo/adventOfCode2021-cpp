#include <array>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

struct PointXY {
    int x;
    int y;
};

bool operator==(const PointXY& lhs, const PointXY& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }

bool operator!=(const PointXY& lhs, const PointXY& rhs) { return !(lhs == rhs); }

struct Line {
    PointXY start;
    PointXY end;
};

std::ostream& operator<<(std::ostream& os, const Line& line) {
    os << "Line(from " << line.start.x << "/" << line.start.y << " to " << line.end.x << "/"
       << line.end.y << ")";
    return os;
}

namespace std {
template <>
struct hash<PointXY> {
    size_t operator()(const PointXY& rhs) const noexcept {
        static_assert(sizeof(int) * 2 == sizeof(size_t));

        return hash<size_t>()(static_cast<size_t>(rhs.x) << 32 | static_cast<size_t>(rhs.y));
    }
};
}  // namespace std

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

void die(const std::string_view reason) { throw std::runtime_error(reason.data()); }

Line parseLine(const std::string& line_str) {
    using std::string;
    using std::vector;

    auto die_wrong_definition = []() { die("invalid line definition"); };

    vector<string> two_points_strs = split(line_str, " -> ");
    if (two_points_strs.size() != 2) die_wrong_definition();

    auto parse_point = [&die_wrong_definition](const string& s) -> PointXY {
        auto postsplit = split(s, ",");
        if (postsplit.size() != 2) die_wrong_definition();
        return PointXY{.x = std::stoi(postsplit[0]), .y = std::stoi(postsplit[1])};
    };

    PointXY start = parse_point(two_points_strs[0]);
    PointXY end = parse_point(two_points_strs[1]);

    return Line{.start = start, .end = end};
}

int main() {
    using std::array;
    using std::string;
    using std::vector;

    const auto filename = "input1.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    vector<Line> lines;
    {
        // read in lines
        std::string line_str;
        while (std::getline(ifs, line_str)) {
            lines.emplace_back(parseLine(line_str));
        }
    }

    assert(lines.size() > 0);
    std::cout << "Read " << lines.size() << " lines\n";

    auto countLinesPerPoint = [](const vector<Line>& lines) -> int {
        std::unordered_map<PointXY, int> numberOfLinesAtPoint;

        for (const Line& line : lines) {
            // go from start to end
            const int dxFull = line.end.x - line.start.x;
            const int dyFull = line.end.y - line.start.y;
            const int dx = dxFull != 0 ? dxFull / std::abs(dxFull) : 0;
            const int dy = dyFull != 0 ? dyFull / std::abs(dyFull) : 0;

            int x = line.start.x;
            int y = line.start.y;
            do {
                const PointXY p{.x = x, .y = y};
                ++numberOfLinesAtPoint[p];
                x += dx;
                y += dy;
            } while (x != line.end.x || y != line.end.y);
            // add end point if end is not equal to start
            if (line.start != line.end) ++numberOfLinesAtPoint[line.end];
        }

        int numberOfPointsWithAtLeastTwoLines{0};
        for (const auto& [point, numLines] : numberOfLinesAtPoint) {
            if (numLines >= 2) ++numberOfPointsWithAtLeastTwoLines;
        }
        return numberOfPointsWithAtLeastTwoLines;
    };

    {
        std::cout << " --- Part 1 ---\n";

        vector<Line> relevant_lines;
        for (const Line& line : lines) {
            // check if horizontal/vertical
            const bool isHoriz = line.start.x == line.end.x;
            const bool isVert = line.start.y == line.end.y;
            if (!(isHoriz || isVert)) continue;

            relevant_lines.push_back(line);
        }

        std::cout << countLinesPerPoint(relevant_lines)
                  << " points have at least two lines on them\n";
    }
    {
        std::cout << " --- Part 2 ---\n";

        std::cout << countLinesPerPoint(lines) << " points have at least two lines on them\n";
    }
}