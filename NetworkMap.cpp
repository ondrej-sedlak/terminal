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
    buildGraph();
    return true;
}

void NetworkMap::buildGraph() {
    nodes_.clear();
    adj_.clear();
    nodeIdGrid_.assign(rows(), std::vector<int>(cols(), -1));

    // A cell is a node if it is S/X or not a plain corridor cell
    // (a corridor cell has exactly 2 open neighbours).
    for (int r = 0; r < rows(); ++r) {
        for (int c = 0; c < cols(); ++c) {
            if (!isOpen(r, c)) {
                continue;
            }
            int open = 0;
            const int dr[4] = {-1, 1, 0, 0};
            const int dc[4] = {0, 0, -1, 1};
            for (int k = 0; k < 4; ++k) {
                if (isOpen(r + dr[k], c + dc[k])) {
                    ++open;
                }
            }
            if (grid_[r][c] == 'S' || grid_[r][c] == 'X' || open != 2) {
                nodeIdGrid_[r][c] = static_cast<int>(nodes_.size());
                nodes_.push_back({r, c});
            }
        }
    }

    // From every node, walk along each open direction until the next node.
    adj_.resize(nodes_.size());
    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = {0, 0, -1, 1};
    for (size_t i = 0; i < nodes_.size(); ++i) {
        int r = nodes_[i].first;
        int c = nodes_[i].second;
        for (int k = 0; k < 4; ++k) {
            int nr = r + dr[k];
            int nc = c + dc[k];
            if (!isOpen(nr, nc)) {
                continue;
            }
            Edge edge;
            edge.dirRow = dr[k];
            edge.dirCol = dc[k];
            int pr = r, pc = c;   // where we came from
            int cr = nr, cc = nc; // current cell
            edge.path.push_back({cr, cc});
            while (nodeIdGrid_[cr][cc] == -1) {
                // corridor cell: continue to the open neighbour != previous cell
                bool advanced = false;
                for (int k2 = 0; k2 < 4; ++k2) {
                    int tr = cr + dr[k2];
                    int tc = cc + dc[k2];
                    if (tr == pr && tc == pc) {
                        continue;
                    }
                    if (isOpen(tr, tc)) {
                        pr = cr; pc = cc;
                        cr = tr; cc = tc;
                        edge.path.push_back({cr, cc});
                        advanced = true;
                        break;
                    }
                }
                if (!advanced) {
                    break; // safety, should not happen on a valid map
                }
            }
            edge.toNode = nodeIdGrid_[cr][cc];
            adj_[i].push_back(edge);
        }
    }
}

int NetworkMap::nodeIdAt(int row, int col) const {
    if (row < 0 || row >= rows() || col < 0 || col >= cols()) {
        return -1;
    }
    return nodeIdGrid_[row][col];
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
