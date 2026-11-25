#ifndef NETWORK_MAP_HPP
#define NETWORK_MAP_HPP

#include <string>
#include <utility>
#include <vector>

// Loads the 2D network grid and builds a node graph on top of it.
// '#' = wall, '.' = open path, 'S' = player start, 'X' = exit node.
// A cell is a NODE if it is S/X or does not have exactly 2 open
// neighbours (junctions and dead ends). Corridor cells connect nodes.
class NetworkMap {
public:
    // One path from a node to a neighbouring node.
    struct Edge {
        int toNode = -1;                        // target node id
        int dirRow = 0, dirCol = 0;             // first step out of the source node
        std::vector<std::pair<int, int>> path;  // cells to traverse, incl. target node cell
    };

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

    // Node graph (built by load()).
    int nodeIdAt(int row, int col) const;   // -1 if the cell is not a node
    bool isNode(int row, int col) const { return nodeIdAt(row, col) >= 0; }
    int startNode() const { return nodeIdAt(startRow_, startCol_); }
    int exitNode() const { return nodeIdAt(exitRow_, exitCol_); }
    const std::vector<Edge>& edgesFrom(int nodeId) const { return adj_[nodeId]; }

private:
    void buildGraph();

    std::vector<std::string> grid_;
    int startRow_ = 0;
    int startCol_ = 0;
    int exitRow_ = 0;
    int exitCol_ = 0;
    std::vector<std::pair<int, int>> nodes_;       // node id -> cell
    std::vector<std::vector<int>> nodeIdGrid_;     // cell -> node id (-1 = corridor)
    std::vector<std::vector<Edge>> adj_;           // node id -> outgoing edges
};

#endif // NETWORK_MAP_HPP
