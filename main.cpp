#include <cstdlib>
#include <iostream>
#include <string>

#include "GameEngine.hpp"
#include "InputHandler.hpp"
#include "Menu.hpp"
#include "NetworkMap.hpp"

int main(int argc, char* argv[]) {
    std::string mapPath; // empty = show the menu

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: terminal [map_file]\n"
                      << "Terminal hacking puzzle. Without arguments a menu opens:\n"
                      << "start (all levels in order), choose level, or exit.\n"
                      << "With a map file the game starts directly on that map.\n"
                      << "In game (raw mode, single keypress, no Enter): w/a/s/d or arrow\n"
                      << "keys send the packet along a path to the next node; keys pressed\n"
                      << "while it travels are ignored, q quits. Reach the exit 'X'.\n";
            return 0;
        }
        mapPath = arg;
    }

    InputHandler input;
    input.start(); // one reader thread for the whole process

    if (!mapPath.empty()) {
        NetworkMap map;
        if (!map.load(mapPath)) {
            std::cerr << "Error: could not load map file: " << mapPath << '\n';
            return 1;
        }
        GameEngine engine(map, input);
        engine.run();
    } else {
        Menu menu(input);
        menu.run();
    }

    // The input thread may still be blocked inside std::getline on stdin.
    // Returning from main would hang in static destruction of std::cin,
    // so terminate without running static destructors (streams are flushed).
    std::quick_exit(0);
}
