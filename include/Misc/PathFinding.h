//
// Created by Eduardo on 09/02/2025.
//

#ifndef BRAKEZA3D_PATHFINDING_H
#define BRAKEZA3D_PATHFINDING_H

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <tuple> // Necesario para std::tuple
#include "../Objects/Vertex3D.h"

struct Node {
    int x, y, z;
    float g, h;
    Node* parent;
    Node(int x, int y, int z, float g, float h, Node* parent)
    : x(x), y(y), z(z), g(g), h(h), parent(parent)
    {

    }
    float f() const { return g + h; }
};

struct CompareNodes {
    bool operator()(const Node* a, const Node* b) {
        return a->f() > b->f();
    }
};

class PathFinding {
private:
    int width, height, depth;
    std::vector<std::vector<std::vector<bool>>> grid;
    std::vector<std::tuple<int, int, int>> directions = {
        { 1,  0,  0}, {-1,  0,  0},  // Movimiento en X
        { 0,  1,  0}, { 0, -1,  0},  // Movimiento en Y
        { 0,  0,  1}, { 0,  0, -1},  // Movimiento en Z

        { 1,  1,  0}, {-1, -1,  0},  // Diagonales en XY
        { 1, -1,  0}, {-1,  1,  0},
        { 1,  0,  1}, {-1,  0, -1},  // Diagonales en XZ
        { 1,  0, -1}, {-1,  0,  1},
        { 0,  1,  1}, { 0, -1, -1},  // Diagonales en YZ
        { 0,  1, -1}, { 0, -1,  1},

        { 1,  1,  1}, {-1, -1, -1},  // Diagonales en XYZ
        { 1, -1, -1}, {-1,  1,  1},
        { 1, -1,  1}, {-1,  1, -1},
        { 1,  1, -1}, {-1, -1,  1}
    };

    static float heuristic(int x1, int y1, int z1, int x2, int y2, int z2)
    {
        return (float) sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2));
    }

public:
    int from[3] = {};
    int to[3] = {};

    PathFinding(int w, int h, int d) : width(w), height(h), depth(d) {
        grid.resize(w, std::vector<std::vector<bool>>(h, std::vector<bool>(d, false)));
    }

    void setObstacle(int x, int y, int z) {
        grid[x][y][z] = true;
    }

    void removeObstacle(int x, int y, int z) {
        grid[x][y][z] = false;
    }

    bool isObstacle(int x, int y, int z) {
        return grid[x][y][z];
    }

    std::vector<std::tuple<int, int, int>> findPath(int x1, int y1, int z1, int x2, int y2, int z2) {
        std::priority_queue<Node*, std::vector<Node*>, CompareNodes> openSet;
        std::unordered_map<int, Node*> allNodes;

        Node* start = new Node(x1, y1, z1, 0, heuristic(x1, y1, z1, x2, y2, z2), nullptr);
        openSet.push(start);
        allNodes[x1 * width * depth + y1 * depth + z1] = start;

        while (!openSet.empty()) {
            Node* current = openSet.top();
            openSet.pop();

            if (current->x == x2 && current->y == y2 && current->z == z2) {
                std::vector<std::tuple<int, int, int>> path;
                while (current) {
                    path.emplace_back(current->x, current->y, current->z); // Agregamos z
                    current = current->parent;
                }
                std::reverse(path.begin(), path.end());
                return path;
            }

            for (auto [dx, dy, dz] : directions) { // Asegurar que 'directions' maneje 3D
                int nx = current->x + dx;
                int ny = current->y + dy;
                int nz = current->z + dz; // Ahora consideramos 'z'

                if (nx >= 0 && ny >= 0 && nz >= 0 && nx < width && ny < height && nz < depth && !isObstacle(nx, ny, nz)) {
                    float gNew = current->g + 1;
                    float hNew = heuristic(nx, ny, nz, x2, y2, z2);
                    Node* neighbor = new Node(nx, ny, nz, gNew, hNew, current);
                    openSet.push(neighbor);
                    allNodes[nx * width * depth + ny * depth + nz] = neighbor;
                }
            }
        }
        return {};
    }

    void setTravel(int x1, int y1, int z1, int x2, int y2, int z2)
    {
        from[0] = x1; from[1] = y1; from[2] = z1;
        to[0] = x2  ; to[1] = y2  ; to[2] = z2;
    }

    std::vector<std::tuple<int, int, int>> makeTravel() {
        return this->findPath(from[0], from[1], from[2], to[0], to[1], to[2]);
    }

    void reset(int w, int h, int d)
    {
        width = w;
        height = h;
        depth = d;
        grid.clear();
        grid.resize(w, std::vector<std::vector<bool>>(h, std::vector<bool>(d, false)));
    }
};

#endif //BRAKEZA3D_PATHFINDING_H
