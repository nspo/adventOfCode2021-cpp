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

class LookupHelper {
   public:
    using RawLookupType = std::unordered_map<std::string, char>;
    LookupHelper(RawLookupType subs) : m_subs{std::move(subs)} {}

    class LookupResult {
       public:
        std::unordered_map<char, size_t> count;  // count of chars in lookup

        LookupResult& merge(const LookupResult& rhs) {
            for (const auto [ch, ch_count] : rhs.count) {
                this->count[ch] += ch_count;
            }
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& os, const LookupResult& obj) {
            for (const auto [ch, ch_count] : obj.count) {
                os << ch << " (" << ch_count << ") ";
            }
            os << "\n";
            return os;
        }
    };

    // count chars after iterating on whole string for steps
    LookupResult count_all(const std::string& s, const int steps) {
        assert(s.size() >= 2);
        assert(steps >= 0);

        LookupResult res{};
        ++res.count[s[0]];
        // count result for two-letter elements
        for (size_t i = 1; i < s.size(); ++i) {
            ++res.count[s[i]];
            const std::string partial_string{s[i - 1], s[i]};
            res.merge(count_new(partial_string, steps));
        }
        return res;
    }

   private:
    RawLookupType m_subs;

    std::map<std::pair<std::string, int>, LookupResult> m_cache;

    // count how many characters are added by iterating on two-char string s steps times
    LookupResult count_new(const std::string& s, const int steps) {
        assert(s.size() == 2);
        assert(steps >= 0);
        assert('A' <= s[0] && s[0] <= 'Z');
        assert('A' <= s[1] && s[1] <= 'Z');
        assert(m_subs.find(s) != m_subs.end());

        // std::cout << "Checking " << s << ", " << steps << "\n";

        if (steps == 0) return LookupResult{};
        if (steps == 1) {
            LookupResult res{};
            ++res.count[m_subs[s]];
            return res;
        }

        // check cache
        if (auto it_cache = m_cache.find(std::make_pair(s, steps)); it_cache != m_cache.end()) {
            return it_cache->second;
        }

        // split the calculation up
        const std::string new_left{s[0], m_subs[s]};
        const std::string new_right{m_subs[s], s[1]};
        LookupResult res{};
        ++res.count[m_subs[s]];
        res.merge(count_new(new_left, steps - 1)).merge(count_new(new_right, steps - 1));

        m_cache[std::make_pair(s, steps)] = res;

        return res;
    }
};

int main() {
    using std::array;
    using std::string;
    using std::unordered_map;
    using std::vector;

    // const auto filename = "input_sample.txt";
    const auto filename = "input.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    string polymer;
    unordered_map<string, char> substitutions;
    {
        // read in lines
        if (!std::getline(ifs, polymer)) std::terminate();

        string line;
        while (std::getline(ifs, line)) {
            if (line == "") continue;

            const auto splitted = split(line, " -> ");
            substitutions[splitted.at(0)] = splitted.at(1).at(0);
        }
    }

    struct CountResult {
        char most_common;
        size_t most_common_count;
        char least_common;
        size_t least_common_count;
    };

    const auto count_common = [](const LookupHelper::LookupResult& res) -> CountResult {
        assert(res.count.size() > 1);

        char most_common{};
        size_t most_common_count{0};
        char least_common{};
        size_t least_common_count{std::numeric_limits<size_t>::max()};
        for (const auto [ch, count] : res.count) {
            if (count > most_common_count) {
                most_common_count = count;
                most_common = ch;
            }
            if (count < least_common_count) {
                least_common_count = count;
                least_common = ch;
            }
        }

        return CountResult{.most_common = most_common,
                           .most_common_count = most_common_count,
                           .least_common = least_common,
                           .least_common_count = least_common_count};
    };

    const auto print_stats = [&count_common](const LookupHelper::LookupResult& res) -> void {
        const auto counted = count_common(res);
        std::cout << "Most common: " << counted.most_common << " (" << counted.most_common_count
                  << "), least common: " << counted.least_common << " ("
                  << counted.least_common_count << ")\n";
        std::cout << "Difference: " << counted.most_common_count << " - "
                  << counted.least_common_count << " = "
                  << counted.most_common_count - counted.least_common_count << "\n";
    };

    LookupHelper look(substitutions);
    {
        std::cout << " --- Part 1 ---\n";

        std::cout << "After 10 steps:\n";
        print_stats(look.count_all(polymer, 10));
    }

    {
        std::cout << " --- Part 2 ---\n";

        std::cout << "After 40 steps:\n";
        print_stats(look.count_all(polymer, 40));
    }
}