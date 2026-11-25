#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

// Reads lines from stdin on a separate thread so the game loop
// can run in real time without blocking on input.
class InputHandler {
public:
    void start();                  // spawns the reader thread
    bool poll(std::string& cmd);   // pops one pending command; false if none
    bool eof() const { return eof_.load(); } // stdin closed

private:
    void readerLoop();

    std::queue<std::string> queue_;
    std::mutex mutex_;
    std::atomic<bool> eof_{false};
    std::thread thread_;
};

#endif // INPUT_HANDLER_HPP
