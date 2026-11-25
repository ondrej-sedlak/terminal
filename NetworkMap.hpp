#ifndef NETWORK_MAP_HPP
#define NETWORK_MAP_HPP

#include <string>
#include <vector>

// Loads and renders the 2D network grid.
// '#' = wall, '.' = open path, 'S' = player start, 'X' = exit node.
class NetworkMap {
public:
    // Reads the map from a text file. Returns false if the file cannot be opened.
    bool load(const std::string& filePath);

    int rows() const { return static_cast<int>(grid_.size()); }
    int cols() const;

    char cell(int row, int col) const;
    bool isOpen(int row, int col) const; // true if the player can enter the cell

    int startRow() const { return startRow_; }
    int startCol() const { return startCol_; }
    int exitRow() const { return exitRow_; }
    int exitCol() const { return exitCol_; }

private:
    std::vector<std::string> grid_;
    int startRow_ = 0;
    int startCol_ = 0;
    int exitRow_ = 0;
    int exitCol_ = 0;
};

#endif // NETWORK_MAP_HPP
