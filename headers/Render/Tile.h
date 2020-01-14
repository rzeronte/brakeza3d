#ifndef SDL2_3D_ENGINE_TILE_H
#define SDL2_3D_ENGINE_TILE_H

#include "../Objects/Triangle3D.h"
#include <vector>
#include <OpenCL/opencl.h>

struct Tile {
    bool draw;
    int id;
    int id_x;
    int id_y;
    int start_x;
    int start_y;
    int numTriangles;
    int triangleIds[1500];
    unsigned int *buffer;
    float *bufferDepth;
    cl_mem clBuffer;
    cl_mem clBufferDepth;
};

struct OCLTile {
    bool draw;
    int id;
    int id_x;
    int id_y;
    int start_x;
    int start_y;
    int numTriangles;
    int triangleIds[1500];
};

#endif //SDL2_3D_ENGINE_GUI_H
