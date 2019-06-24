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
    int numTriangles;
    int triangleIds[1024];
};

struct OCLTile {
    bool draw;
    int id;
    int id_x;
    int id_y;
    int start_x;
    int start_y;
    int numTriangles;
    int triangleIds[1024];
};

#endif //SDL2_3D_ENGINE_GUI_H
