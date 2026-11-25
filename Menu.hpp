#ifndef MENU_HPP
#define MENU_HPP

#include <string>
#include <vector>

#include "InputHandler.hpp"

// The main menu: start (play all levels from the first one),
// choose a single level, or exit.
class Menu {
public:
    explicit Menu(InputHandler& input);

    void run(); // blocks until the user exits or stdin closes

private:
    std::vector<std::string> discoverLevels();
    void printMenu() const;
    void playFrom(int startIndex); // plays levels[startIndex..end] in order
    std::string readLine();         // waits for one input line via the queue

    InputHandler& input_;
    std::vector<std::string> levels_;
};

#endif // MENU_HPP
