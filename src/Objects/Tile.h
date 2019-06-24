#ifndef SDL2_3D_ENGINE_TILE_H
#define SDL2_3D_ENGINE_TILE_H

#include "../../headers/Objects/Triangle3D.h"
#include <vector>

struct Tile {
    bool draw;
    int id;
    int id_x;
    int id_y;
    int start_x;
    int start_y;
    std::vector <Triangle> triangles;
    unsigned int *buffer;
};
#endif //SDL2_3D_ENGINE_GUI_H
