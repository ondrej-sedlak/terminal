#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "NetworkMap.hpp"

// Draws the game to the terminal: the centered map, the player packet
// and the prompt line. Dispatch-style look with basic ANSI colors.
class Renderer {
public:
    void hideCursor();
    void showCursor();

    // Redraws the whole frame (cursor-home + clear-below, no screen clutter).
    void drawFrame(const NetworkMap& map, int playerRow, int playerCol);

    // Prompt line below the map. No trailing newline on purpose: a newline
    // while the cursor sits on the last terminal row would scroll the screen.
    void drawPrompt(char dirChar);

    void message(const char* text); // one-shot line after the game ends

private:
    // The terminal size cannot be queried with the allowed headers, so the
    // map is centered assuming a standard 80x24 terminal.
    static const int kTermCols = 80;
    static const int kTermRows = 24;
};

#endif // RENDERER_HPP
