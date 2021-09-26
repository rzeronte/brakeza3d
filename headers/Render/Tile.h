#ifndef SDL2_3D_ENGINE_TILE_H
#define SDL2_3D_ENGINE_TILE_H

#include "../Objects/Triangle3D.h"
#include <vector>

#define MAX_TILE_TRIANGLES 3000

struct Tile {
    bool draw;
    int id;
    int id_x;
    int id_y;
    int start_x;
    int start_y;
    std::vector<int> triangleIds;
    unsigned int *buffer;
    float *bufferDepth;
};


#endif //SDL2_3D_ENGINE_GUI_H
