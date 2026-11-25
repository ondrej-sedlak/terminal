#include "GameEngine.hpp"

#include <chrono>
#include <string>
#include <thread>

namespace {

char dirChar(int velRow, int velCol) {
    if (velRow < 0) return '^';
    if (velRow > 0) return 'v';
    if (velCol < 0) return '<';
    if (velCol > 0) return '>';
    return '.';
}

// Normalizes one input line to a command: lowercase WASD and
// arrow-key escape sequences (Up/Down/Right/Left = ESC [ A/B/C/D).
std::string normalize(std::string cmd) {
    if (cmd == "\x1b[A") return "w";
    if (cmd == "\x1b[B") return "s";
    if (cmd == "\x1b[C") return "d";
    if (cmd == "\x1b[D") return "a";
    if (cmd.size() == 1 && cmd[0] >= 'A' && cmd[0] <= 'Z') {
        cmd[0] = static_cast<char>(cmd[0] - 'A' + 'a');
    }
    return cmd;
}

} // namespace

GameEngine::GameEngine(const NetworkMap& map, InputHandler& input)
    : map_(map),
      input_(input),
      playerRow_(map.startRow()),
      playerCol_(map.startCol()) {}

void GameEngine::run() {
    renderer_.hideCursor();

    using namespace std::chrono;
    const auto tick = milliseconds(100); // one cell per 100 ms while sliding

    while (running_ && !input_.eof()) {
        auto frameStart = steady_clock::now();

        // Consume all pending commands; the last one wins.
        std::string cmd;
        while (input_.poll(cmd)) {
            handleCommand(normalize(cmd));
        }
        if (!running_) {
            break;
        }

        movePacket();

        renderer_.drawFrame(map_, playerRow_, playerCol_);
        renderer_.drawPrompt(dirChar(velRow_, velCol_));

        if (atExit()) {
            won_ = true;
            renderer_.message("\033[32mACCESS GRANTED - you reached the exit node.\033[0m");
            break;
        }

        auto elapsed = steady_clock::now() - frameStart;
        if (elapsed < tick) {
            std::this_thread::sleep_for(tick - elapsed);
        }
    }

    renderer_.showCursor();
}

void GameEngine::handleCommand(const std::string& cmd) {
    int dr = 0, dc = 0;
    bool steer = true;
    if (cmd == "q") {
        running_ = false;
        steer = false;
    } else if (cmd == "w") {
        dr = -1;
    } else if (cmd == "s") {
        dr = 1;
    } else if (cmd == "a") {
        dc = -1;
    } else if (cmd == "d") {
        dc = 1;
    } else if (cmd == "x") {
        velRow_ = 0; velCol_ = 0; // stop now
        hasBuf_ = false;
        steer = false;
    } else {
        steer = false;
    }
    if (steer) {
        bufRow_ = dr; bufCol_ = dc; hasBuf_ = true;
        if (velRow_ == 0 && velCol_ == 0) {
            velRow_ = dr; velCol_ = dc; // start moving immediately
        }
    }
}

void GameEngine::movePacket() {
    // Pac-Man style: the packet keeps moving in its current direction.
    if (velRow_ == 0 && velCol_ == 0) {
        return;
    }
    ++moveCounter_;
    // Terminal cells are about twice as tall as they are wide, so step
    // vertically only every second tick -> all directions look equally fast.
    bool stepNow = (velCol_ != 0) || (moveCounter_ % 2 == 0);
    if (!stepNow) {
        return;
    }
    // Turn buffer: if the buffered direction is open right now, take it.
    if (hasBuf_ && (bufRow_ != velRow_ || bufCol_ != velCol_) &&
        map_.isOpen(playerRow_ + bufRow_, playerCol_ + bufCol_)) {
        velRow_ = bufRow_;
        velCol_ = bufCol_;
        hasBuf_ = false;
    }
    int newRow = playerRow_ + velRow_;
    int newCol = playerCol_ + velCol_;
    if (map_.isOpen(newRow, newCol)) {
        playerRow_ = newRow;
        playerCol_ = newCol;
    } else {
        velRow_ = 0; // slammed into a wall, packet stops
        velCol_ = 0;
        hasBuf_ = false;
    }
}

bool GameEngine::atExit() const {
    return playerRow_ == map_.exitRow() && playerCol_ == map_.exitCol();
}
