#include "Menu.hpp"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <thread>

#include "GameEngine.hpp"
#include "NetworkMap.hpp"

Menu::Menu(InputHandler& input) :
    input_(input)
{
    levels_ = discoverLevels();
}

std::vector<std::string> Menu::discoverLevels()
{
    // Level files are levels/level1.txt, level2.txt, ...
    std::vector<std::string> found;
    std::string base = "levels/";
    {
        std::ifstream f("levels/level1.txt");
        if (!f.is_open())
        {
            base = "../levels/"; // running from inside build/
        }
    }
    for (int i = 1;; ++i)
    {
        std::string path = base + "level" + std::to_string(i) + ".txt";
        std::ifstream f(path);
        if (!f.is_open())
        {
            break;
        }
        found.push_back(path);
    }
    return found;
}

std::string Menu::readLine()
{
    std::string line;
    while (!input_.eof())
    {
        if (input_.poll(line))
        {
            return line;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return "";
}

void Menu::printMenu() const
{
    std::cout << "\033[2J\033[H"; // clear screen once for the menu
    std::cout << "=== TERMINAL - hacking minigame ===\n\n"
              << "  1) Start (play all levels from the first one)\n"
              << "  2) Choose level\n"
              << "  3) Exit\n\n";
    if (levels_.empty())
    {
        std::cout << "  (no levels found in levels/)\n\n";
    }
    std::cout << "Choice: " << std::flush;
}

void Menu::run()
{
    for (;;)
    {
        printMenu();
        std::string line = readLine();
        if (line.empty() && input_.eof())
        {
            return;
        }
        if (line == "1")
        {
            playFrom(0);
        }
        else if (line == "2")
        {
            std::cout << '\n';
            for (size_t i = 0; i < levels_.size(); ++i)
            {
                std::cout << "  " << (i + 1) << ") " << levels_[i] << '\n';
            }
            std::cout << "  0) Back\nLevel: " << std::flush;
            std::string pick = readLine();
            int idx = std::atoi(pick.c_str()) - 1;
            if (idx >= 0 && idx < static_cast<int>(levels_.size()))
            {
                playFrom(idx);
            }
        }
        else if (line == "3" || line == "q")
        {
            return;
        }
        if (input_.eof())
        {
            return;
        }
    }
}

void Menu::playFrom(int startIndex)
{
    for (size_t i = static_cast<size_t>(startIndex); i < levels_.size(); ++i)
    {
        NetworkMap map;
        if (!map.load(levels_[i]))
        {
            std::cerr << "Error: could not load map file: " << levels_[i] << '\n';
            return;
        }
        std::cout << "\033[2J\033[H"; // clear the menu before the game frame
        GameEngine engine(map, input_);
        engine.run();
        if (!engine.won())
        {
            return; // player quit -> back to the menu
        }
        if (i + 1 < levels_.size())
        {
            std::cout << "\nLevel complete! Next level...\n" << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }
    }
    std::cout << "\n\033[32mAll levels completed. You are a true hacker.\033[0m\n" << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
}
