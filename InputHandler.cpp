#include "InputHandler.hpp"

#include <iostream>

void InputHandler::start() {
    thread_ = std::thread(&InputHandler::readerLoop, this);
    thread_.detach(); // stopped by process exit; clean thread join comes in a later checkpoint
}

void InputHandler::readerLoop() {
    std::string line;
    while (std::getline(std::cin, line)) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(line);
    }
    eof_ = true;
}

bool InputHandler::poll(std::string& cmd) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
        return false;
    }
    cmd = queue_.front();
    queue_.pop();
    return true;
}
