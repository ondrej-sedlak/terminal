#include "Renderer.hpp"

#include <iostream>

void Renderer::hideCursor()
{
    std::cout << "\033[?25l";
}

void Renderer::showCursor()
{
    std::cout << "\033[?25h";
}

void Renderer::drawFrame(const NetworkMap& map, int playerRow, int playerCol)
{
    int offsetX = (kTermCols - map.cols()) / 2;
    int offsetY = (kTermRows - map.rows()) / 2;
    if (offsetX < 0)
        offsetX = 0;
    if (offsetY < 0)
        offsetY = 0;

    std::cout << "\033[H";
    std::cout << "\033[36m"; // cyan
    for (int i = 0; i < offsetY; ++i)
    {
        std::cout << '\n';
    }
    for (int r = 0; r < map.rows(); ++r)
    {
        for (int i = 0; i < offsetX; ++i)
        {
            std::cout << ' ';
        }
        for (int c = 0; c < map.cols(); ++c)
        {
            char ch = map.cell(r, c);
            if (r == playerRow && c == playerCol)
            {
                std::cout << "\033[97m@\033[36m"; // bright white packet
            }
            else if (ch == '#')
            {
                std::cout << ' '; // wall = empty black space
            }
            else if (ch == 'X')
            {
                std::cout << "\033[92mX\033[36m"; // bright green exit node
            }
            else if (ch == 'S')
            {
                std::cout << "\033[93mS\033[36m"; // yellow start node
            }
            else if (map.isNode(r, c))
            {
                std::cout << 'o'; // junction node
            }
            else
            {
                std::cout << '.'; // path cell between nodes
            }
        }
        std::cout << '\n';
    }
    std::cout << "\033[0m\033[J"; // reset color, wipe anything below the map
}

void Renderer::drawPrompt(char dir)
{
    // std::flush instead of std::endl: flush the frame but keep the cursor
    // off the last terminal row so echoed input cannot scroll the screen.
    std::cout << "dir: " << dir << "  (w/a/s/d or arrows: travel to the next node, q quit)" << std::flush;
}

void Renderer::message(const char* text)
{
    std::cout << '\n' << text << std::endl;
}
