#include "GameEngine.hpp"

#include <chrono>
#include <string>
#include <thread>

namespace
{

char dirCharOf(int dr, int dc)
{
    if (dr < 0)
        return '^';
    if (dr > 0)
        return 'v';
    if (dc < 0)
        return '<';
    if (dc > 0)
        return '>';
    return '.';
}

// Normalizes one input to a command: lowercase WASD and
// arrow-key escape sequences (Up/Down/Right/Left = ESC [ A/B/C/D).
std::string normalize(std::string cmd)
{
    if (cmd == "\x1b[A")
        return "w";
    if (cmd == "\x1b[B")
        return "s";
    if (cmd == "\x1b[C")
        return "d";
    if (cmd == "\x1b[D")
        return "a";
    if (cmd.size() == 1 && cmd[0] >= 'A' && cmd[0] <= 'Z')
    {
        cmd[0] = static_cast<char>(cmd[0] - 'A' + 'a');
    }
    return cmd;
}

} // namespace

GameEngine::GameEngine(const NetworkMap& map, InputHandler& input) :
    map_(map),
    input_(input),
    node_(map.startNode()),
    playerRow_(map.startRow()),
    playerCol_(map.startCol())
{}

void GameEngine::run()
{
    renderer_.hideCursor();

    using namespace std::chrono;
    const auto tick = milliseconds(100); // one path cell per 100 ms while travelling

    while (running_ && !input_.eof())
    {
        auto frameStart = steady_clock::now();

        // Consume all pending commands; the last one wins.
        std::string cmd;
        while (input_.poll(cmd))
        {
            handleCommand(normalize(cmd));
        }
        if (!running_)
        {
            break;
        }

        movePacket();

        renderer_.drawFrame(map_, playerRow_, playerCol_);
        renderer_.drawPrompt(dirChar());

        if (atExit())
        {
            won_ = true;
            renderer_.message("\033[32mACCESS GRANTED - you reached the exit node.\033[0m");
            break;
        }

        auto elapsed = steady_clock::now() - frameStart;
        if (elapsed < tick)
        {
            std::this_thread::sleep_for(tick - elapsed);
        }
    }

    renderer_.showCursor();
}

void GameEngine::handleCommand(const std::string& cmd)
{
    if (cmd == "q")
    {
        running_ = false;
        return;
    }
    if (moving_)
    {
        return; // travelling: wait until the packet reaches the next node
    }
    int dr = 0, dc = 0;
    if (cmd == "w")
    {
        dr = -1;
    }
    else if (cmd == "s")
    {
        dr = 1;
    }
    else if (cmd == "a")
    {
        dc = -1;
    }
    else if (cmd == "d")
    {
        dc = 1;
    }
    else
    {
        return; // not a direction key
    }
    tryStart(dr, dc); // standing on a node: leave immediately
}

bool GameEngine::tryStart(int dr, int dc)
{
    for (const auto& edge : map_.edgesFrom(node_))
    {
        if (edge.dirRow == dr && edge.dirCol == dc)
        {
            moving_ = true;
            curEdge_ = edge;
            edgePos_ = 0;
            playerRow_ = edge.path[0].first;
            playerCol_ = edge.path[0].second;
            return true;
        }
    }
    return false; // no path in that direction
}

void GameEngine::movePacket()
{
    if (!moving_)
    {
        return;
    }
    ++moveCounter_;
    int next = edgePos_ + 1;
    if (next >= static_cast<int>(curEdge_.path.size()))
    {
        return;
    }
    // Terminal cells are about twice as tall as they are wide, so step
    // vertically only every second tick -> all directions look equally fast.
    int dr = curEdge_.path[next].first - curEdge_.path[edgePos_].first;
    bool stepNow = (dr == 0) || (moveCounter_ % 2 == 0);
    if (!stepNow)
    {
        return;
    }
    edgePos_ = next;
    playerRow_ = curEdge_.path[edgePos_].first;
    playerCol_ = curEdge_.path[edgePos_].second;
    if (edgePos_ == static_cast<int>(curEdge_.path.size()) - 1)
    {
        // Arrived at the next node and wait for the next keypress.
        node_ = curEdge_.toNode;
        moving_ = false;
    }
}

bool GameEngine::atExit() const
{
    return node_ == map_.exitNode();
}

char GameEngine::dirChar() const
{
    if (moving_ && edgePos_ + 1 < static_cast<int>(curEdge_.path.size()))
    {
        int dr = curEdge_.path[edgePos_ + 1].first - curEdge_.path[edgePos_].first;
        int dc = curEdge_.path[edgePos_ + 1].second - curEdge_.path[edgePos_].second;
        return dirCharOf(dr, dc);
    }
    return '.';
}
