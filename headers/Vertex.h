//
// Created by darkhead on 28/4/18.
//

#ifndef SDL2_3D_ENGINE_VERTEX_H
#define SDL2_3D_ENGINE_VERTEX_H


#include <SDL_surface.h>
#include <string>

class Vertex {
public:
    bool debug = false;
    float x, y, z;

    float angle;
    // texture coordenates
    float u, v;

    Vertex();
    Vertex(float, float, float);

    Vertex getNormalize();

    void consoleInfo(std::string label, bool);

    Vertex getInverse();
    void setOrigin();
    void addVertex(Vertex v);
    void subVertex(Vertex v);

    static float dotProduct(Vertex, Vertex);
    float getNorm();
};


#endif //SDL2_3D_ENGINE_VERTEX_H
