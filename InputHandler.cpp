#include "InputHandler.hpp"

#include <cstdlib>
#include <iostream>

void InputHandler::start()
{
    // Single keypresses, no Enter needed, no echo. We do NOT use full
    // "stty raw" because that also disables output processing (opost),
    // which would turn every '\n' into a bare LF and break formatting.
    // -icanon gives us per-byte input, -isig makes Ctrl-C arrive as byte 3.
    std::system("stty -icanon -echo -isig 2>/dev/null");
    // quick_exit() runs at_quick_exit handlers, so the terminal settings
    // are restored even though the reader thread stays inside cin.get().
    std::at_quick_exit([] { std::system("stty sane 2>/dev/null"); });
    thread_ = std::thread(&InputHandler::readerLoop, this);
    thread_.detach(); // stopped by process exit; clean thread join comes in a later checkpoint
}

void InputHandler::readerLoop()
{
    for (;;)
    {
        int ci = std::cin.get();
        if (ci == EOF)
        {
            eof_ = true;
            return;
        }
        char c = static_cast<char>(ci);
        if (c == '\n' || c == '\r')
        {
            continue; // ignore Enter pressed out of habit
        }
        if (c == 3)
        {
            c = 'q'; // Ctrl-C arrives as a plain byte in raw mode
        }
        std::string cmd(1, c);
        if (c == '\x1b')
        {
            // Arrow keys send ESC [ A/B/C/D as three bytes at once.
            int c2 = std::cin.get();
            int c3 = std::cin.get();
            if (c2 == '[' && (c3 == 'A' || c3 == 'B' || c3 == 'C' || c3 == 'D'))
            {
                cmd = std::string("\x1b[") + static_cast<char>(c3);
            }
        }
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(cmd);
    }
}

bool InputHandler::poll(std::string& cmd)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty())
    {
        return false;
    }
    cmd = queue_.front();
    queue_.pop();
    return true;
}
