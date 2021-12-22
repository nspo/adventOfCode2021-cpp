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

// Snailfish number
struct Number {
    using UPtr = std::unique_ptr<Number>;

    // std::variant would be better than two optionals, but visitors are eh...
    std::optional<int> regular;                 // if regular, the value
    std::optional<std::pair<UPtr, UPtr>> pair;  // if non-regular, the children

    bool isRegular() const { return regular.has_value(); }

    int magnitude() const {
        if (isRegular()) {
            return regular.value();
        }

        return 3 * pair.value().first->magnitude() + 2 * pair.value().second->magnitude();
    }

    static UPtr makeRegular(const int val) {
        return std::make_unique<Number>(Number{.regular = val, .pair = {}});
    }

    // make from pair - without reduction!
    static UPtr makePair(UPtr first, UPtr second) {
        return std::make_unique<Number>(
            Number{.regular = {}, .pair = std::make_pair(std::move(first), std::move(second))});
    }

    static UPtr makeEmpty() { return std::make_unique<Number>(); }

    // Parse valid snailfish number string into Number
    static UPtr makeFromString(const std::string_view sv) {
        assert(!sv.empty());
        auto top_num = makeEmpty();

        std::vector<Number*> stack;
        stack.push_back(top_num.get());
        for (size_t idx = 0; idx < sv.size(); ++idx) {
            const auto isNum = [](const char c) -> bool { return '0' <= c && c <= '9'; };
            if (sv[idx] == '[') {
                // create pair
                assert(!stack.empty());
                Number* parent = stack.back();
                stack.pop_back();
                parent->pair = std::make_pair(makeEmpty(), makeEmpty());
                stack.push_back(parent->pair.value().second.get());
                stack.push_back(parent->pair.value().first.get());
            } else if (isNum(sv[idx])) {
                // parse number
                assert(!stack.empty());
                int val = 0;
                do {
                    val = 10 * val + (sv[idx] - '0');
                } while (idx + 1 < sv.size() && isNum(sv[idx + 1]) && ++idx);
                Number* num = stack.back();
                stack.pop_back();
                num->regular = val;
            } else {
                // should be , or ] which can be skipped
                assert(sv[idx] == ',' || sv[idx] == ']');
            }
        }
        assert(stack.empty());

        return top_num;
    }
};

std::ostream& operator<<(std::ostream& os, const Number& num) {
    if (num.regular.has_value()) {
        os << num.regular.value();
    } else {
        os << "[" << *num.pair.value().first << "," << *num.pair.value().second << "]";
    }
    return os;
}

// Find the target number, if any, to explode and the regular numbers before and after if they exist
// with DFS
void findToExplode(const int depth, Number* curr, Number*& prev, Number*& target, Number*& post) {
    assert(curr != nullptr);
    if (post != nullptr) return;

    if (curr->isRegular()) {
        if (target == nullptr) {
            prev = curr;
        } else {
            post = curr;
        }
    } else {
        // pair
        if (depth == 4 && target == nullptr) {
            // pair inside 4 pairs and no target found yet
            assert(curr->pair.value().first->isRegular() && curr->pair.value().second->isRegular());
            target = curr;
        } else {
            // recurse
            findToExplode(depth + 1, curr->pair.value().first.get(), prev, target, post);
            findToExplode(depth + 1, curr->pair.value().second.get(), prev, target, post);
        }
    }
}

bool explodeIfNecessary(Number* top_num) {
    Number* prev = nullptr;
    Number* target = nullptr;
    Number* post = nullptr;
    findToExplode(0, top_num, prev, target, post);

    if (target == nullptr) {
        return false;
    }

    // explode
    if (prev != nullptr) {
        prev->regular.value() += target->pair.value().first->regular.value();
    }
    if (post != nullptr) {
        post->regular.value() += target->pair.value().second->regular.value();
    }
    target->pair = {};
    target->regular = 0;
    return true;
}

// Find the target number, if any, to split
void findToSplit(Number* curr, Number*& target) {
    assert(curr != nullptr);
    if (target != nullptr) return;

    if (curr->isRegular()) {
        if (curr->regular.value() >= 10) {
            target = curr;
        }
    } else {
        // pair
        findToSplit(curr->pair.value().first.get(), target);
        findToSplit(curr->pair.value().second.get(), target);
    }
}

bool splitIfNecessary(Number* top_num) {
    Number* target = nullptr;

    findToSplit(top_num, target);

    if (target == nullptr) {
        return false;
    }

    // actually split
    assert(target->isRegular());
    const int old_val = target->regular.value();
    target->regular = {};
    const int val_floor = old_val / 2;
    const int val_ceil = std::ceil(old_val / 2.0);
    target->pair = std::make_pair(Number::makeRegular(val_floor), Number::makeRegular(val_ceil));
    return true;
}

void reduce(Number* top_num) {
    assert(top_num != nullptr);

    // keep exploding or splitting until nothing to do
    while (explodeIfNecessary(top_num) || splitIfNecessary(top_num)) {
    }
}

// Add two snailfish numbers and reduce
Number::UPtr add(Number::UPtr first, Number::UPtr second) {
    Number::UPtr top_num = std::make_unique<Number>(
        Number{.regular = {}, .pair = std::make_pair(std::move(first), std::move(second))});
    reduce(top_num.get());
    return top_num;
}

int main() {
    using std::array;
    using std::string;
    using std::vector;

    const auto filename = "input.txt";

    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    vector<string> input_strings;  // for creating numbers on-demand
    {
        string line;
        while (std::getline(ifs, line)) {
            input_strings.emplace_back(std::move(line));
        }
        assert(!input_strings.empty());
    }

    {
        std::cout << " --- Part 1 ---\n";
        assert(!input_strings.empty());

        Number::UPtr num = Number::makeFromString(input_strings.front());
        for (size_t i = 1; i < input_strings.size(); ++i) {
            num = add(std::move(num), Number::makeFromString(input_strings[i]));
        }

        std::cout << "Final sum: " << *num << "\n";
        std::cout << "Magnitude: " << num->magnitude() << "\n";
    }

    {
        std::cout << " --- Part 2 ---\n";

        // Find largest possible magnitude of addition result of any two numbers
        int largest_magnitude = std::numeric_limits<int>::min();
        for (size_t i = 0; i < input_strings.size(); ++i) {
            // iterate from the start again as result is non-commutative
            for (size_t j = 0; j < input_strings.size(); ++j) {
                if (i == j) continue;
                Number::UPtr num1 = Number::makeFromString(input_strings[i]);
                Number::UPtr num2 = Number::makeFromString(input_strings[j]);
                Number::UPtr res = add(std::move(num1), std::move(num2));
                largest_magnitude = std::max(largest_magnitude, res->magnitude());
            }
        }
        std::cout << "Largest possible magnitude by adding two numbers: " << largest_magnitude
                  << "\n";
    }
}