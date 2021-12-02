#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

class Submarine
{
public:
    void followCommand(const std::string &raw_command)
    {
        const auto [cmd, units] = Submarine::splitCommand(raw_command);

        if (cmd == "forward")
        {
            m_pos_horiz += units;
        }
        else if (cmd == "down")
        {
            m_depth += units;
        }
        else
        {
            // up
            m_depth -= units;
        }
    }

    int getPosHoriz() const { return m_pos_horiz; }
    int getDepth() const { return m_depth; }

protected:
    // split command into command string and units
    static std::pair<std::string, int> splitCommand(const std::string &raw_command)
    {
        // C++ regex actually has bad perf, but should be alright for this
        const std::regex pattern{"^(forward|down|up) ([0-9]+)$"};
        std::smatch match;
        if (!std::regex_match(raw_command, match, pattern))
        {
            throw std::invalid_argument("Invalid raw command: " + raw_command);
        }

        return {match[1], std::stoi(match[2])};
    }

    int m_pos_horiz{0};
    int m_depth{0};
};

class Submarine2 : public Submarine
{
public:
    void followCommand(const std::string &raw_command)
    {
        const auto [cmd, units] = Submarine::splitCommand(raw_command);

        if (cmd == "down")
        {
            m_aim += units;
        }
        else if (cmd == "up")
        {
            m_aim -= units;
        }
        else
        {
            // forward
            m_pos_horiz += units;
            m_depth += m_aim * units;
        }
    }

protected:
    int m_aim{0};
};

int main()
{
    const auto filename = "input1.txt";
    std::ifstream ifs(filename);
    if (!ifs)
        std::terminate();

    std::vector<std::string> commands;
    {
        std::string line{};
        while (std::getline(ifs, line))
        {
            commands.emplace_back(std::move(line));
        }
    }

    {
        std::cout << "--- Part 1 ---\n";
        Submarine sub;
        for (const auto &command : commands)
        {
            sub.followCommand(command);
        }

        const int answer1 = sub.getPosHoriz() * sub.getDepth();
        std::cout << "Pos horiz: " << sub.getPosHoriz() << ", depth: " << sub.getDepth() << ", product: " << answer1 << "\n";
    }

    {
        std::cout << "--- Part 2 ---\n";
        Submarine2 sub2;
        for (const auto &command : commands)
        {
            sub2.followCommand(command);
        }
        const int answer2 = sub2.getPosHoriz() * sub2.getDepth();
        std::cout << "Pos horiz: " << sub2.getPosHoriz() << ", depth: " << sub2.getDepth() << ", product: " << answer2 << "\n";
    }
}