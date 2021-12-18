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

// append binary values to binary vector
// EXAMPLE: append(bin, 0, 0, 0, 1)
template <typename... Args>
void append(std::vector<bool>& bin, Args... values) {
    auto append_lambda = [&bin](auto& append_lambda, int first, auto... rest) {
        auto append_single = [&bin](const bool val) { bin.push_back(val); };

        append_single(first == 0 ? false : true);
        if constexpr (sizeof...(rest) > 0) {
            append_lambda(append_lambda, rest...);
        }
    };

    append_lambda(append_lambda, values...);
}

struct Packet {
    int version;
    int type_id;
    // may only contain either literal OR packets
    std::optional<int> literal;
    std::vector<Packet> packets;

    void check() const {
        if (literal.has_value() && !packets.empty()) {
            throw std::logic_error("Packet may not contain both a literal and sub-packets");
        }
    }
};

std::ostream& operator<<(std::ostream& os, const Packet& pk) {
    os << "Packet[v=" << pk.version << ", type_id=" << pk.type_id << ", ";
    pk.check();
    if (pk.literal.has_value()) {
        os << "literal=" << pk.literal.value();
    } else {
        os << "sub=[";
        for (const auto& subpk : pk.packets) {
            os << subpk << ", ";
        }
        os << "]";
    }
    os << "]";
    return os;
}

std::vector<bool> hex2bin(const std::string& hex) {
    using std::string;
    using std::vector;

    vector<bool> bin;
    bin.reserve(hex.size() * 4);

    for (const char c : hex) {
        switch (c) {
            case '0':
                append(bin, 0, 0, 0, 0);
                break;
            case '1':
                append(bin, 0, 0, 0, 1);
                break;
            case '2':
                append(bin, 0, 0, 1, 0);
                break;
            case '3':
                append(bin, 0, 0, 1, 1);
                break;
            case '4':
                append(bin, 0, 1, 0, 0);
                break;
            case '5':
                append(bin, 0, 1, 0, 1);
                break;
            case '6':
                append(bin, 0, 1, 1, 0);
                break;
            case '7':
                append(bin, 0, 1, 1, 1);
                break;
            case '8':
                append(bin, 1, 0, 0, 0);
                break;
            case '9':
                append(bin, 1, 0, 0, 1);
                break;
            case 'A':
                append(bin, 1, 0, 1, 0);
                break;
            case 'B':
                append(bin, 1, 0, 1, 1);
                break;
            case 'C':
                append(bin, 1, 1, 0, 0);
                break;
            case 'D':
                append(bin, 1, 1, 0, 1);
                break;
            case 'E':
                append(bin, 1, 1, 1, 0);
                break;
            case 'F':
                append(bin, 1, 1, 1, 1);
                break;
            default:
                throw std::invalid_argument("Unknown hex character");
                break;
        }
    }

    return bin;
}

template <typename T>
void print(const std::vector<T>& vec, const char* const sep = "") {
    for (const auto elem : vec) {
        std::cout << elem << sep;
    }
    std::cout << "\n";
}

int parse_int(const std::vector<bool>& bin, size_t& idx, const size_t len) {
    assert(len > 0);
    assert(idx + len <= bin.size());

    const size_t end = idx + len;
    int val = 0;
    for (; idx < end; ++idx) {
        val = val * 2 + (bin[idx] ? 1 : 0);
    }
    return val;
}

// parse literal value of unknown length
int parse_literal(const std::vector<bool>& bin, size_t& idx) {
    int val = 0;

    bool last_group_found = false;
    while (!last_group_found) {
        assert(idx + 5 <= bin.size());
        last_group_found = bin[idx++] == 0;
        for (int i = 0; i < 4; ++i) {
            val = val * 2 + bin[idx++];
        }
    }
    return val;
}

// parse packet of unknown length
Packet parse_packet(const std::vector<bool>& bin, size_t& idx) {
    Packet packet{};
    packet.version = parse_int(bin, idx, 3);
    packet.type_id = parse_int(bin, idx, 3);

    if (packet.type_id == 4) {
        // literal value
        packet.literal = parse_literal(bin, idx);
    } else {
        // sub packets

        const bool length_type = bin.at(idx++);
        if (length_type == 0) {
            const size_t total_length_in_bits = static_cast<size_t>(parse_int(bin, idx, 15));
            const size_t started_at = idx;

            while (idx < started_at + total_length_in_bits) {
                packet.packets.emplace_back(parse_packet(bin, idx));
            }
        } else {
            const int num_sub_packets = parse_int(bin, idx, 11);
            for (int i = 0; i < num_sub_packets; ++i) {
                packet.packets.emplace_back(parse_packet(bin, idx));
            }
        }
    }

    return packet;
}

int version_sum(const Packet& packet) {
    int sum = packet.version;

    for (const Packet& sub : packet.packets) {
        sum += version_sum(sub);
    }

    return sum;
}

int main() {
    using std::array;
    using std::string;
    using std::vector;

    const auto filename = "input.txt";

    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    vector<bool> data;
    {
        string line;
        if (!std::getline(ifs, line)) std::terminate();
        assert(!line.empty());
        data = hex2bin(line);
    }

    {
        std::cout << " --- Part 1 ---\n";
        size_t i = 0;
        const Packet top_packet = parse_packet(data, i);
        std::cout << "Version sum: " << version_sum(top_packet) << "\n";
    }

    { std::cout << " --- Part 2 ---\n"; }
}