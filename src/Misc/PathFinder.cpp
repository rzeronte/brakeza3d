#include <SDL_surface.h>
#include <SDL_image.h>
#include "../../headers/Misc/PathFinder.h"
#include "../../headers/Misc/Color.h"
#include "../../headers/EngineSetup.h"
#include "../../headers/Misc/Tools.h"
#include "../../headers/Render/Logging.h"

PathFinder::PathFinder(int sizeX, int sizeY) : sizeX(sizeX), sizeY(sizeY) {
    grid = new int *[this->sizeY];
    for (int i = 0; i < this->sizeY; i++) {
        grid[i] = new int[this->sizeX];
    }

    for (int x = 0; x < this->sizeX; x++) {
        for (int y = 0; y < this->sizeY; y++) {
            grid[y][x] = 0;
        }
    }
}

// A Utility Function to check whether given cell (row, col)
// is a valid cell or not.
bool PathFinder::isValid(int row, int col) const {
    // Returns true if row number and column number
    // is in range
    return (row >= 0) && (row < this->sizeY) && (col >= 0) && (col < this->sizeX);
}

// A Utility Function to check whether the given cell is
// blocked or not
bool PathFinder::isUnBlocked(int **grid, int row, int col) {
    if (grid[row][col] == 1)
        return (true);
    else
        return (false);
}

// A Utility Function to check whether destination cell has
// been reached or not
bool PathFinder::isDestination(int row, int col, PairData dest) {
    if (row == dest.first && col == dest.second)
        return (true);
    else
        return (false);
}

// A Utility Function to calculate the 'h' heuristics.
double PathFinder::calculateHValue(int row, int col, PairData dest) {
    // Return using the distance formula
    return sqrt(
            (row - dest.first) * (row - dest.first)
            + (col - dest.second) * (col - dest.second)
    );
}

// A Utility Function to trace the path from the source
// to destination
std::stack<PairData> PathFinder::tracePath(cell **cellDetails, PairData dest) {
    int row = dest.first;
    int col = dest.second;

    std::stack<PairData> Path;

    while (!(cellDetails[row][col].parent_i == row && cellDetails[row][col].parent_j == col)) {
        Path.push(std::make_pair(row, col));
        int temp_row = cellDetails[row][col].parent_i;
        int temp_col = cellDetails[row][col].parent_j;
        row = temp_row;
        col = temp_col;
    }

    Path.push(std::make_pair(row, col));

    // Path running example
    /*while (!Path.empty()) {
        std::pair<int, int> p = Path.top();
        Path.pop();
        printf("-> (%d,%d) ", p.first, p.second);
    }*/

    return Path;
}

// A Function to find the shortest path between
// a given source cell to a destination cell according
// to A* Search Algorithm
bool PathFinder::AStarSearch(PairData src, PairData dest, std::stack<PairData> &path) {
    // If the source is out of range
    if (!isValid(src.first, src.second)) {
        //printf("Source is invalid\n");
        return false;
    }

    // If the destination is out of range
    if (!isValid(dest.first, dest.second)) {
        //printf("Destination is invalid\n");
        return false;
    }

    // Either the source or the destination is blocked
    if (!isUnBlocked(grid, src.first, src.second) || !isUnBlocked(grid, dest.first, dest.second)) {
        //printf("Source or the destination is blocked\n");
        return false;
    }

    // If the destination cell is the same as source cell
    if (isDestination(src.first, src.second, dest)) {
        //printf("We are already at the destination\n");
        return false;
    }

    // Create a closed list and initialise it to false which
    // means that no cell has been included yet This closed
    // list is implemented as a boolean 2D array
    bool closedList[this->sizeY][this->sizeX];
    memset(closedList, false, sizeof(closedList));

    // Declare a 2D array of structure to hold the details
    // of that cell
    cell **cellDetails;
    cellDetails = new cell *[this->sizeY];
    for (int i = 0; i < this->sizeY; i++) {
        cellDetails[i] = new cell[this->sizeX];
    }

    int i, j;
    for (i = 0; i < this->sizeY; i++) {
        for (j = 0; j < this->sizeX; j++) {
            cellDetails[i][j].f = FLT_MAX;
            cellDetails[i][j].g = FLT_MAX;
            cellDetails[i][j].h = FLT_MAX;
            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
        }
    }

    // Initialising the parameters of the starting node
    i = src.first, j = src.second;
    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parent_i = i;
    cellDetails[i][j].parent_j = j;

    /*
     Create an open list having information as-
     <f, <i, j>>
     where f = g + h,
     and i, j are the row and column index of that cell
     Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
     This open list is implenented as a set of pair of
     pair.*/
    std::set<pPair> openList;

    // Put the starting cell on the open list and set its
    // 'f' as 0
    openList.insert(std::make_pair(0.0, std::make_pair(i, j)));

    // We set this boolean value as false as initially
    // the destination is not reached.
    bool foundDest = false;

    while (!openList.empty()) {
        pPair p = *openList.begin();

        // Remove this vertex from the open list
        openList.erase(openList.begin());

        // Add this vertex to the closed list
        i = p.second.first;
        j = p.second.second;
        closedList[i][j] = true;

        /*
         Generating all the 8 successor of this cell

             N.W   N   N.E
               \   |   /
                \  |  /
             W----Cell----E
                  / | \
                /   |  \
             S.W    S   S.E

         Cell-->Popped Cell (i, j)
         N -->  North       (i-1, j)
         S -->  South       (i+1, j)
         E -->  East        (i, j+1)
         W -->  West           (i, j-1)
         N.E--> North-East  (i-1, j+1)
         N.W--> North-West  (i-1, j-1)
         S.E--> South-East  (i+1, j+1)
         S.W--> South-West  (i+1, j-1)*/

        // To store the 'g', 'h' and 'f' of the 8 successors
        double gNew, hNew, fNew;

        //----------- 1st Successor (North) ------------

        // Only process this cell if this is a valid one
        if (isValid(i - 1, j)) {
            // If the destination cell is the same as the
            // current successor
            if (isDestination(i - 1, j, dest)) {
                // Set the Parent of the destination cell
                cellDetails[i - 1][j].parent_i = i;
                cellDetails[i - 1][j].parent_j = j;
                //printf("The destination cell is found\n");
                path = tracePath(cellDetails, dest);
                foundDest = true;
                return foundDest;
            }
                // If the successor is already on the closed
                // list or if it is blocked, then ignore it.
                // Else do the following
            else if (!closedList[i - 1][j] && isUnBlocked(grid, i - 1, j)) {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = calculateHValue(i - 1, j, dest);
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (cellDetails[i - 1][j].f == FLT_MAX || cellDetails[i - 1][j].f > fNew) {
                    openList.insert(std::make_pair(fNew, std::make_pair(i - 1, j)));

                    // Update the details of this cell
                    cellDetails[i - 1][j].f = fNew;
                    cellDetails[i - 1][j].g = gNew;
                    cellDetails[i - 1][j].h = hNew;
                    cellDetails[i - 1][j].parent_i = i;
                    cellDetails[i - 1][j].parent_j = j;
                }
            }
        }

        //----------- 2nd Successor (South) ------------

        // Only process this cell if this is a valid one
        if (isValid(i + 1, j)) {
            // If the destination cell is the same as the
            // current successor
            if (isDestination(i + 1, j, dest)) {
                // Set the Parent of the destination cell
                cellDetails[i + 1][j].parent_i = i;
                cellDetails[i + 1][j].parent_j = j;
                //printf("The destination cell is found\n");
                path = tracePath(cellDetails, dest);
                foundDest = true;
                return foundDest;
            }
                // If the successor is already on the closed
                // list or if it is blocked, then ignore it.
                // Else do the following
            else if (!closedList[i + 1][j] && isUnBlocked(grid, i + 1, j)) {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = calculateHValue(i + 1, j, dest);
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (cellDetails[i + 1][j].f == FLT_MAX || cellDetails[i + 1][j].f > fNew) {
                    openList.insert(std::make_pair(fNew, std::make_pair(i + 1, j)));
                    // Update the details of this cell
                    cellDetails[i + 1][j].f = fNew;
                    cellDetails[i + 1][j].g = gNew;
                    cellDetails[i + 1][j].h = hNew;
                    cellDetails[i + 1][j].parent_i = i;
                    cellDetails[i + 1][j].parent_j = j;
                }
            }
        }

        //----------- 3rd Successor (East) ------------

        // Only process this cell if this is a valid one
        if (isValid(i, j + 1)) {
            // If the destination cell is the same as the
            // current successor
            if (isDestination(i, j + 1, dest)) {
                // Set the Parent of the destination cell
                cellDetails[i][j + 1].parent_i = i;
                cellDetails[i][j + 1].parent_j = j;
                //printf("The destination cell is found\n");
                path = tracePath(cellDetails, dest);
                foundDest = true;
                return foundDest;
            }

                // If the successor is already on the closed
                // list or if it is blocked, then ignore it.
                // Else do the following
            else if (!closedList[i][j + 1] && isUnBlocked(grid, i, j + 1)) {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = calculateHValue(i, j + 1, dest);
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (cellDetails[i][j + 1].f == FLT_MAX || cellDetails[i][j + 1].f > fNew) {
                    openList.insert(std::make_pair(fNew, std::make_pair(i, j + 1)));

                    // Update the details of this cell
                    cellDetails[i][j + 1].f = fNew;
                    cellDetails[i][j + 1].g = gNew;
                    cellDetails[i][j + 1].h = hNew;
                    cellDetails[i][j + 1].parent_i = i;
                    cellDetails[i][j + 1].parent_j = j;
                }
            }
        }

        //----------- 4th Successor (West) ------------

        // Only process this cell if this is a valid one
        if (isValid(i, j - 1)) {
            // If the destination cell is the same as the
            // current successor
            if (isDestination(i, j - 1, dest)) {
                // Set the Parent of the destination cell
                cellDetails[i][j - 1].parent_i = i;
                cellDetails[i][j - 1].parent_j = j;
                //printf("The destination cell is found\n");
                path = tracePath(cellDetails, dest);
                foundDest = true;
                return foundDest;
            }

                // If the successor is already on the closed
                // list or if it is blocked, then ignore it.
                // Else do the following
            else if (!closedList[i][j - 1] && isUnBlocked(grid, i, j - 1)) {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = calculateHValue(i, j - 1, dest);
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (cellDetails[i][j - 1].f == FLT_MAX
                    || cellDetails[i][j - 1].f > fNew) {
                    openList.insert(std::make_pair(
                            fNew, std::make_pair(i, j - 1)));

                    // Update the details of this cell
                    cellDetails[i][j - 1].f = fNew;
                    cellDetails[i][j - 1].g = gNew;
                    cellDetails[i][j - 1].h = hNew;
                    cellDetails[i][j - 1].parent_i = i;
                    cellDetails[i][j - 1].parent_j = j;
                }
            }
        }

        //----------- 5th Successor (North-East)
        //------------

        // Only process this cell if this is a valid one
        if (isValid(i - 1, j + 1)) {
            // If the destination cell is the same as the
            // current successor
            if (isDestination(i - 1, j + 1, dest)) {
                // Set the Parent of the destination cell
                cellDetails[i - 1][j + 1].parent_i = i;
                cellDetails[i - 1][j + 1].parent_j = j;
                //printf("The destination cell is found\n");
                path = tracePath(cellDetails, dest);
                foundDest = true;
                return foundDest;
            }

                // If the successor is already on the closed
                // list or if it is blocked, then ignore it.
                // Else do the following
            else if (!closedList[i - 1][j + 1] && isUnBlocked(grid, i - 1, j + 1)) {
                gNew = cellDetails[i][j].g + 1.414;
                hNew = calculateHValue(i - 1, j + 1, dest);
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (cellDetails[i - 1][j + 1].f == FLT_MAX || cellDetails[i - 1][j + 1].f > fNew) {
                    openList.insert(std::make_pair(fNew, std::make_pair(i - 1, j + 1)));

                    // Update the details of this cell
                    cellDetails[i - 1][j + 1].f = fNew;
                    cellDetails[i - 1][j + 1].g = gNew;
                    cellDetails[i - 1][j + 1].h = hNew;
                    cellDetails[i - 1][j + 1].parent_i = i;
                    cellDetails[i - 1][j + 1].parent_j = j;
                }
            }
        }

        //----------- 6th Successor (North-West)
        //------------

        // Only process this cell if this is a valid one
        if (isValid(i - 1, j - 1)) {
            // If the destination cell is the same as the
            // current successor
            if (isDestination(i - 1, j - 1, dest)) {
                // Set the Parent of the destination cell
                cellDetails[i - 1][j - 1].parent_i = i;
                cellDetails[i - 1][j - 1].parent_j = j;
                //printf("The destination cell is found\n");
                path = tracePath(cellDetails, dest);
                foundDest = true;
                return foundDest;
            }

                // If the successor is already on the closed
                // list or if it is blocked, then ignore it.
                // Else do the following
            else if (!closedList[i - 1][j - 1] && isUnBlocked(grid, i - 1, j - 1)) {
                gNew = cellDetails[i][j].g + 1.414;
                hNew = calculateHValue(i - 1, j - 1, dest);
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (cellDetails[i - 1][j - 1].f == FLT_MAX || cellDetails[i - 1][j - 1].f > fNew) {
                    openList.insert(std::make_pair(fNew, std::make_pair(i - 1, j - 1)));
                    // Update the details of this cell
                    cellDetails[i - 1][j - 1].f = fNew;
                    cellDetails[i - 1][j - 1].g = gNew;
                    cellDetails[i - 1][j - 1].h = hNew;
                    cellDetails[i - 1][j - 1].parent_i = i;
                    cellDetails[i - 1][j - 1].parent_j = j;
                }
            }
        }

        //----------- 7th Successor (South-East)
        //------------

        // Only process this cell if this is a valid one
        if (isValid(i + 1, j + 1)) {
            // If the destination cell is the same as the
            // current successor
            if (isDestination(i + 1, j + 1, dest)) {
                // Set the Parent of the destination cell
                cellDetails[i + 1][j + 1].parent_i = i;
                cellDetails[i + 1][j + 1].parent_j = j;
                //printf("The destination cell is found\n");
                path = tracePath(cellDetails, dest);
                foundDest = true;
                return foundDest;
            }

                // If the successor is already on the closed
                // list or if it is blocked, then ignore it.
                // Else do the following
            else if (!closedList[i + 1][j + 1] && isUnBlocked(grid, i + 1, j + 1)) {
                gNew = cellDetails[i][j].g + 1.414;
                hNew = calculateHValue(i + 1, j + 1, dest);
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (cellDetails[i + 1][j + 1].f == FLT_MAX || cellDetails[i + 1][j + 1].f > fNew) {
                    openList.insert(std::make_pair(fNew, std::make_pair(i + 1, j + 1)));

                    // Update the details of this cell
                    cellDetails[i + 1][j + 1].f = fNew;
                    cellDetails[i + 1][j + 1].g = gNew;
                    cellDetails[i + 1][j + 1].h = hNew;
                    cellDetails[i + 1][j + 1].parent_i = i;
                    cellDetails[i + 1][j + 1].parent_j = j;
                }
            }
        }

        //----------- 8th Successor (South-West)
        //------------

        // Only process this cell if this is a valid one
        if (isValid(i + 1, j - 1)) {
            // If the destination cell is the same as the
            // current successor
            if (isDestination(i + 1, j - 1, dest)) {
                // Set the Parent of the destination cell
                cellDetails[i + 1][j - 1].parent_i = i;
                cellDetails[i + 1][j - 1].parent_j = j;
                //printf("The destination cell is found\n");
                path = tracePath(cellDetails, dest);
                foundDest = true;
                return foundDest;
            }

                // If the successor is already on the closed
                // list or if it is blocked, then ignore it.
                // Else do the following
            else if (!closedList[i + 1][j - 1] && isUnBlocked(grid, i + 1, j - 1)) {
                gNew = cellDetails[i][j].g + 1.414;
                hNew = calculateHValue(i + 1, j - 1, dest);
                fNew = gNew + hNew;

                // If it isn’t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (cellDetails[i + 1][j - 1].f == FLT_MAX
                    || cellDetails[i + 1][j - 1].f > fNew) {
                    openList.insert(std::make_pair(
                            fNew, std::make_pair(i + 1, j - 1)));

                    // Update the details of this cell
                    cellDetails[i + 1][j - 1].f = fNew;
                    cellDetails[i + 1][j - 1].g = gNew;
                    cellDetails[i + 1][j - 1].h = hNew;
                    cellDetails[i + 1][j - 1].parent_i = i;
                    cellDetails[i + 1][j - 1].parent_j = j;
                }
            }
        }
    }

    // When the destination cell is not found and the open
    // list is empty, then we conclude that we failed to
    // reach the destiantion cell. This may happen when the
    // there is no way to destination cell (due to
    // blockages)

    return foundDest;
}

void PathFinder::setValue(int posX, int posY, bool value) {
    this->grid[posY][posX] = value;
}

void PathFinder::demo() {
    int data[9][10] = {
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
            {1, 1, 1, 0, 1, 1, 1, 0, 1, 1},
            {1, 1, 1, 0, 1, 1, 0, 1, 0, 1},
            {0, 0, 1, 0, 1, 0, 0, 0, 0, 1},
            {1, 1, 1, 0, 1, 1, 1, 0, 1, 0},
            {1, 0, 1, 1, 1, 1, 0, 1, 0, 0},
            {1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
            {1, 1, 1, 0, 0, 0, 1, 0, 0, 1}
    };


    // load data into grid
    for (int x = 0; x < this->sizeX; x++) {
        for (int y = 0; y < this->sizeY; y++) {
            setValue(x, y, data[y][x]);
        }
    }

    consoleDebug();

    // Source is the left-most bottom-most corner
    PairData src = std::make_pair(8, 0);

    // Destination is the left-most top-most corner
    PairData dest = std::make_pair(0, 9);

    std::stack<PairData> path;
    bool result = AStarSearch(src, dest, path);

    if (result) {
        printf("Destination has found\r\n");

        consoleDebugPath(path);
    }
}

void PathFinder::consoleDebug() {
    printf("\r\nDebug Grid: \r\n");
    for (int y = 0; y < this->sizeY; y++) {
        for (int x = 0; x < this->sizeX; x++) {
            printf("%d ", grid[y][x]);
        }
        printf("\r\n");
    }
}

void PathFinder::consoleDebugPath(std::stack<PairData> path) {
    char debugGrid[this->sizeY][this->sizeX];

    // reset all visual debug cells to '-'
    for (int y = 0; y < this->sizeY; y++) {
        for (int x = 0; x < this->sizeX; x++) {
            debugGrid[y][x] = '-';
        }
    }

    if (path.empty()) {
        printf("consoleDebugPath: path is empty");
        return;
    }

    printf("Path trace: ");
    // mark path cell with '*'
    while (!path.empty()) {
        std::pair<int, int> p = path.top();
        path.pop();
        debugGrid[p.first][p.second] = '*';
        printf("->(%d,%d)", p.first, p.second);
    }

    printf("\r\nDebug path: \r\n");
    for (int y = 0; y < this->sizeY; y++) {
        for (int x = 0; x < this->sizeX; x++) {
            printf("%c ", debugGrid[y][x]);
        }
        printf("\r\n");
    }
}

void PathFinder::saveGridToPNG(const std::string& filename) {
    SDL_Surface *s = SDL_CreateRGBSurface(0, this->sizeX, this->sizeY, 32, 0, 0, 0, 0);

    Uint32 black = Color::black();
    Uint32 red = Color::red();

    auto *buffer = new Uint32[this->sizeY * this->sizeX];

    for (int y = 0; y < this->sizeY; y++) {
        for (int x = 0; x < this->sizeX; x++) {
            int index = y * this->sizeX + x;
            if (this->grid[y][x] != 0) {
                buffer[index] = red;
            } else {
                buffer[index] = black;
            }
        }
    }
    memcpy(&s->pixels, &buffer, sizeof(s->pixels));
    IMG_SavePNG(s, (EngineSetup::getInstance()->GRIDS_FOLDER + filename).c_str());
    SDL_FreeSurface(s);
}

void PathFinder::loadGridFromPNG(const std::string& filename) {
    SDL_Surface *s = IMG_Load((filename).c_str());

    int sx = s->w;
    int sy = s->h;

    for (int y = 0; y < sy; y++) {
        for (int x = 0; x < sx; x++) {
            //int index = y * sx + x;
            Uint32 c = Tools::getSurfacePixel(s, x, y);
            Uint8 pred, pgreen, pblue, palpha;
            SDL_GetRGBA(c, s->format, &pred, &pgreen, &pblue, &palpha);
            if (pred == 0 && pgreen == 0 && pblue == 0) {
                grid[y][x] = 0; // blocked
            } else {
                grid[y][x] = 1; // not blocked
            }
        }
    }
}

std::stack<PairData> PathFinder::readPathFromPNG(const std::string& filename) {
    Logging::Log("PathFinder readPathFromPNG " + filename, "PathFinder");

    SDL_Surface *surface = IMG_Load((filename).c_str());

    int width = surface->w;
    int height = surface->h;

    PairData src, dest;

    // get start and end position
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            //int index = y * width + x;
            Uint32 color = Tools::getSurfacePixel(surface, x, y);
            Uint8 pred, pgreen, pblue, palpha;
            SDL_GetRGBA(color, surface->format, &pred, &pgreen, &pblue, &palpha);
            if (pred == 0 && pgreen == 255 && pblue == 0) { // green
                src.first = y;
                src.second = x;
                //printf("start!: %d, %d", x, y);
            }
            if (pred == 0 && pgreen == 0 && pblue == 255) { // blue
                dest.first = y;
                dest.second = x;
                //printf("ends!: %d, %d", x, y);
            }
        }
    }

    auto *pathfinder = new PathFinder(width, height);
    pathfinder->loadGridFromPNG(filename);

    std::stack<PairData> path;
    pathfinder->AStarSearch(src, dest, path);

    delete surface;
    delete pathfinder;

    return path;
}
