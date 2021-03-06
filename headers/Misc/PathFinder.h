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

#define FLT_MAX 10
#define ROW 9
#define COL 10

class PathFinder {

    // Creating a shortcut for int, int pair type
    typedef std::pair<int, int> Pair;

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

    bool isValid(int row, int col);
    bool isUnBlocked(int grid[][COL], int row, int col);
    bool isDestination(int row, int col, Pair dest);
    double calculateHValue(int row, int col, Pair dest);
    void tracePath(cell cellDetails[][COL], Pair dest);
public:
    void demo();
    void aStarSearch(int grid[][COL], Pair src, Pair dest);
};


#endif //BRAKEDA3D_PATHFINDER_H
