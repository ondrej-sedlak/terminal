#include "NetworkMap.hpp"

#include <fstream>

bool NetworkMap::load(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    grid_.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            grid_.push_back(line);
        }
    }

    if (grid_.empty()) {
        return false;
    }

    for (int r = 0; r < rows(); ++r) {
        for (int c = 0; c < cols(); ++c) {
            if (grid_[r][c] == 'S') {
                startRow_ = r;
                startCol_ = c;
            } else if (grid_[r][c] == 'X') {
                exitRow_ = r;
                exitCol_ = c;
            }
        }
    }
    return true;
}

int NetworkMap::cols() const {
    return grid_.empty() ? 0 : static_cast<int>(grid_[0].size());
}

char NetworkMap::cell(int row, int col) const {
    if (row < 0 || row >= rows() || col < 0 || col >= cols()) {
        return '#';
    }
    return grid_[row][col];
}

bool NetworkMap::isOpen(int row, int col) const {
    return cell(row, col) != '#';
}
