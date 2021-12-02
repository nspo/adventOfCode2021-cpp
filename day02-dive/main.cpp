#include <fstream>
#include <iostream>
#include <vector>

class Submarine {
   public:
    void followCommands(const std::vector<std::pair<std::string, int>> &commands) {
        for (const auto &[cmd, units] : commands) {
            followCommand(cmd, units);
        }
    }

    virtual void followCommand(const std::string &cmd, const int units) {
        if (cmd == "forward") {
            m_pos_horiz += units;
        } else if (cmd == "down") {
            m_depth += units;
        } else if (cmd == "up") {
            // up
            m_depth -= units;
        } else {
            throw std::invalid_argument("Unknown command: "+cmd);
        }
    }

    void printLocation() const {
        std::cout << "Horizontal position: " << m_pos_horiz
                  << ", depth: " << m_depth
                  << ", product of both: " << m_pos_horiz * m_depth << "\n";
    }

    int getPosHoriz() const { return m_pos_horiz; }
    int getDepth() const { return m_depth; }

    virtual ~Submarine() = default;

   protected:
    int m_pos_horiz{0};
    int m_depth{0};
};

class Submarine2 : public Submarine {
   public:
    void followCommand(const std::string &cmd, const int units) override {
        if (cmd == "down") {
            m_aim += units;
        } else if (cmd == "up") {
            m_aim -= units;
        } else if (cmd == "forward") {
            // forward
            m_pos_horiz += units;
            m_depth += m_aim * units;
        } else {
            throw std::invalid_argument("Unknown command: "+cmd);
        }
    }

   protected:
    int m_aim{0};
};

int main() {
    const auto filename = "input1.txt";
    std::ifstream ifs(filename);
    if (!ifs) std::terminate();

    std::vector<std::pair<std::string, int>> commands;
    {
        std::string cmd{};
        int units{};
        while (ifs >> cmd >> units) {
            commands.emplace_back(std::make_pair(std::move(cmd), units));
        }
    }

    {
        std::cout << "--- Part 1 ---\n";
        Submarine sub;
        sub.followCommands(commands);
        sub.printLocation();
    }

    {
        std::cout << "--- Part 2 ---\n";
        Submarine2 sub2;
        sub2.followCommands(commands);
        sub2.printLocation();
    }
}