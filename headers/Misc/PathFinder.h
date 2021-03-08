//
// Created by darkhead on 5/3/21.
//

#ifndef BRAKEDA3D_PATHFINDER_H
#define BRAKEDA3D_PATHFINDER_H

#include <iostream>
#include <utility>
#include <math.h>       /* sqrt */
#include <stack>          // std::stack
#include <set>
#include <vector>

#define FLT_MAX 10

class PathFinder
{
public:
    // Creating a shortcut for int, int pair type
    typedef std::pair<int, int> Pair;

    void demo();
    void consoleDebug();
    void consoleDebugPath(std::stack<Pair> path);
    bool AStarSearch(Pair src, Pair dest, std::stack<Pair> &path);
    void setValue(int posX, int posY, bool value);
    PathFinder(int sizeX, int sizeY);

private:
    // Creating a shortcut for pair<int, pair<int, int>> type
    typedef std::pair<double, std::pair<int, int> > pPair;

    // A structure to hold the neccesary parameters
    struct cell {
        // Row and Column index of its parent
        // Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
        int parent_i, parent_j;
        // f = g + h
        double f, g, h;
    };

    const int sizeX;
    const int sizeY;
    int **grid;

    bool isValid(int row, int col);
    bool isUnBlocked(int **grid, int row, int col);
    bool isDestination(int row, int col, Pair dest);
    double calculateHValue(int row, int col, Pair dest);
    std::stack<Pair> tracePath(cell **cellDetails, Pair dest);
};


#endif //BRAKEDA3D_PATHFINDER_H
