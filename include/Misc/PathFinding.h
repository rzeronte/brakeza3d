#ifndef BRAKEZA3D_PATHFINDING_H
#define BRAKEZA3D_PATHFINDING_H

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <tuple>

struct PathfindingNode {
    int x, y, z;
    float g, h;
    PathfindingNode* parent;

    PathfindingNode(int x, int y, int z, float g, float h, PathfindingNode* parent)
            : x(x), y(y), z(z), g(g), h(h), parent(parent) {}

    float f() const { return g + h; }
};

struct CompareNodes {
    bool operator()(const PathfindingNode* a, const PathfindingNode* b) {
        return a->f() > b->f();
    }
};

class PathFinding {
private:
    int width, height, depth;
    std::vector<std::vector<std::vector<bool>>> grid;
    std::vector<std::tuple<int, int, int>> directions = {
            {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1},
            {1, 1, 0}, {-1, -1, 0}, {1, -1, 0}, {-1, 1, 0},
            {1, 0, 1}, {-1, 0, -1}, {1, 0, -1}, {-1, 0, 1},
            {0, 1, 1}, {0, -1, -1}, {0, 1, -1}, {0, -1, 1},
            {1, 1, 1}, {-1, -1, -1}, {1, -1, -1}, {-1, 1, 1},
            {1, -1, 1}, {-1, 1, -1}, {1, 1, -1}, {-1, -1, 1}
    };

    static float heuristic(int x1, int y1, int z1, int x2, int y2, int z2) {
        return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2) + std::pow(z2 - z1, 2));
    }

public:
    int from[3] = {};
    int to[3] = {};

    PathFinding(int w, int h, int d) : width(w), height(h), depth(d)
    {
        grid.resize(w, std::vector<std::vector<bool>>(h, std::vector<bool>(d, false)));
    }

    void setObstacle(int x, int y, int z)
    {
        grid[x][y][z] = true;
    }

    void removeObstacle(int x, int y, int z)
    { grid[x][y][z] = false;
    }

    bool isObstacle(int x, int y, int z)
    {
        return grid[x][y][z];
    }

    std::vector<std::tuple<int, int, int>> findPath(int x1, int y1, int z1, int x2, int y2, int z2)
    {
        std::priority_queue<PathfindingNode*, std::vector<PathfindingNode*>, CompareNodes> openSet;
        std::unordered_map<int, PathfindingNode*> allNodes;
        std::unordered_map<int, bool> closedSet;

        auto getIndex = [&](int x, int y, int z) { return x * width * depth + y * depth + z; };

        PathfindingNode* start = new PathfindingNode(x1, y1, z1, 0, heuristic(x1, y1, z1, x2, y2, z2), nullptr);
        openSet.push(start);
        allNodes[getIndex(x1, y1, z1)] = start;

        while (!openSet.empty()) {
            PathfindingNode* current = openSet.top();
            openSet.pop();

            int index = getIndex(current->x, current->y, current->z);
            if (closedSet[index]) continue;
            closedSet[index] = true;

            if (current->x == x2 && current->y == y2 && current->z == z2) {
                std::vector<std::tuple<int, int, int>> path;
                while (current) {
                    path.emplace_back(current->x, current->y, current->z);
                    current = current->parent;
                }
                std::reverse(path.begin(), path.end());

                // Liberar memoria
                for (auto& pair : allNodes) delete pair.second;
                return path;
            }

            for (auto [dx, dy, dz] : directions) {
                int nx = current->x + dx;
                int ny = current->y + dy;
                int nz = current->z + dz;

                if (nx >= 0 && ny >= 0 && nz >= 0 && nx < width && ny < height && nz < depth && !isObstacle(nx, ny, nz)) {
                    int neighborIndex = getIndex(nx, ny, nz);
                    if (closedSet[neighborIndex]) continue;

                    float movementCost = (dx != 0 && dy != 0 && dz != 0) ? 1.732f :
                                         (dx != 0 && dy != 0 || dy != 0 && dz != 0 || dx != 0 && dz != 0) ? 1.414f : 1.0f;

                    float gNew = current->g + movementCost;
                    float hNew = heuristic(nx, ny, nz, x2, y2, z2);

                    if (allNodes.find(neighborIndex) == allNodes.end() || gNew < allNodes[neighborIndex]->g) {
                        PathfindingNode* neighbor = new PathfindingNode(nx, ny, nz, gNew, hNew, current);
                        openSet.push(neighbor);
                        allNodes[neighborIndex] = neighbor;
                    }
                }
            }
        }

        // Liberar memoria si no se encontró un camino
        for (auto& pair : allNodes) delete pair.second;
        return {};
    }

    void setTravel(int x1, int y1, int z1, int x2, int y2, int z2)
    {
        from[0] = x1; from[1] = y1; from[2] = z1;
        to[0] = x2; to[1] = y2; to[2] = z2;
    }

    std::vector<std::tuple<int, int, int>> makeTravelIndexes()
    {
        return this->findPath(from[0], from[1], from[2], to[0], to[1], to[2]);
    }

    void reset(int w, int h, int d)
    {
        width = w; height = h; depth = d;
        grid.assign(w, std::vector<std::vector<bool>>(h, std::vector<bool>(d, false)));
    }
};

#endif // BRAKEZA3D_PATHFINDING_H
