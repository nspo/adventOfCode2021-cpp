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

using RawLookupType = std::unordered_map<std::string, char>;

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

    struct CountResult {
        char most_common;
        size_t most_common_count;
        char least_common;
        size_t least_common_count;
    };

    CountResult count_common() const {
        assert(count.size() > 0);

        char most_common{};
        size_t most_common_count{0};
        char least_common{};
        size_t least_common_count{std::numeric_limits<size_t>::max()};
        for (const auto [ch, count] : count) {
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

    void print_stats() {
        const auto counted = count_common();
        std::cout << "Most common: " << counted.most_common << " (" << counted.most_common_count
                  << "), least common: " << counted.least_common << " ("
                  << counted.least_common_count << ")\n";
        std::cout << "Difference: " << counted.most_common_count << " - "
                  << counted.least_common_count << " = "
                  << counted.most_common_count - counted.least_common_count << "\n";
    };
};

LookupResult lookupPolymerByFreqCount(const std::string& polymer, const RawLookupType& subs,
                                      const int steps) {
    // more efficient solution than below
    assert(polymer.size() >= 2);
    assert(steps >= 0);
    using std::string;

    // init pair and letter count
    std::unordered_map<std::string, size_t> freq_pairs;  // number of pairs like NN
    std::unordered_map<char, size_t> freq_char;          // number of chars like N

    ++freq_char[polymer[0]];
    for (size_t i = 1; i < polymer.size(); ++i) {
        ++freq_pairs[string{polymer[i - 1], polymer[i]}];
        ++freq_char[polymer[i]];
    }

    // perform steps
    for (int i = 0; i < steps; ++i) {
        // create completely new count of pairs as they are replaced but only increase letter count
        std::unordered_map<std::string, size_t> new_freq_pairs;

        for (const auto [pair, pair_count] : freq_pairs) {
            assert(subs.find(pair) != subs.end());
            const char ch_new = subs.find(pair)->second;
            freq_char[ch_new] += pair_count;
            new_freq_pairs[string{pair[0], ch_new}] += pair_count;
            new_freq_pairs[string{ch_new, pair[1]}] += pair_count;
        }
        std::swap(new_freq_pairs, freq_pairs);
    }

    return LookupResult{.count = freq_char};
}

class LookupHelper {
    // solution which uses dynamic programming + memoization
   public:
    LookupHelper(RawLookupType subs) : m_subs{std::move(subs)} {}

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

    LookupHelper look(substitutions);
    {
        std::cout << " --- Part 1 ---\n";

        std::cout << "After 10 steps:\n";
        look.count_all(polymer, 10).print_stats();
        std::cout << "\nUsing frequency count:\n";
        lookupPolymerByFreqCount(polymer, substitutions, 10).print_stats();
    }

    {
        std::cout << " --- Part 2 ---\n";

        std::cout << "After 40 steps:\n";
        look.count_all(polymer, 40).print_stats();

        std::cout << "\nUsing frequency count:\n";
        lookupPolymerByFreqCount(polymer, substitutions, 40).print_stats();
    }
}