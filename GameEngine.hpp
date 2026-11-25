#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include <string>

#include "InputHandler.hpp"
#include "NetworkMap.hpp"
#include "Renderer.hpp"

// Dispatch-style movement: the packet sits on a node; one keypress sends
// it along a whole path to the next node (animated over a few ticks).
// Keypresses while travelling are ignored - the next move is chosen
// when the packet is standing on a node again.
class GameEngine {
public:
    GameEngine(const NetworkMap& map, InputHandler& input);

    void run();
    bool won() const { return won_; }

private:
    void handleCommand(const std::string& cmd);
    bool tryStart(int dr, int dc); // begin traversing an edge from the current node
    void movePacket();             // advance one cell along the current edge
    bool atExit() const;
    char dirChar() const;          // direction shown in the prompt line

    const NetworkMap& map_;
    InputHandler& input_;
    Renderer renderer_;

    int node_ = -1;                 // current node id
    int playerRow_ = 0;             // rendered position (a cell on the current edge)
    int playerCol_ = 0;
    bool moving_ = false;
    NetworkMap::Edge curEdge_;
    int edgePos_ = 0;               // index of the current cell inside curEdge_.path
    int moveCounter_ = 0;
    bool running_ = true;
    bool won_ = false;
};

#endif // GAME_ENGINE_HPP
