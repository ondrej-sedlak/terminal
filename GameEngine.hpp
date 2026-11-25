#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include "InputHandler.hpp"
#include "NetworkMap.hpp"
#include "Renderer.hpp"

// The real-time game loop: consumes input commands, slides the packet
// across the network and checks the win condition.
class GameEngine {
public:
    GameEngine(const NetworkMap& map, InputHandler& input);

    void run();          // blocks until the player wins, quits or stdin closes
    bool won() const { return won_; } // true if the run ended on the exit node

private:
    void handleCommand(const std::string& cmd);
    void movePacket();
    bool atExit() const;

    const NetworkMap& map_;
    InputHandler& input_;
    Renderer renderer_;

    int playerRow_;
    int playerCol_;
    int velRow_ = 0;
    int velCol_ = 0;
    // Buffered direction: the packet turns into it as soon as the way is
    // open, so the player does not have to hit the exact tick at a junction.
    int bufRow_ = 0;
    int bufCol_ = 0;
    bool hasBuf_ = false;
    int moveCounter_ = 0;
    bool running_ = true;
    bool won_ = false;
};

#endif // GAME_ENGINE_HPP
