//
// Created by darkhead on 28/4/18.
//

#ifndef SDL2_3D_ENGINE_VERTEX_H
#define SDL2_3D_ENGINE_VERTEX_H


#include <SDL_surface.h>
#include <string>

class Vertex3D {
public:
    bool debug = false;
    float x, y, z;

    float angle;
    // texture coordenates
    float u, v;

    Vertex3D();
    Vertex3D(float, float, float);

    Vertex3D getNormalize();

    void consoleInfo(std::string label, bool);

    Vertex3D getInverse();
    void setOrigin();
    void addVertex(Vertex3D v);
    void subVertex(Vertex3D v);

    static float dotProduct(Vertex3D, Vertex3D);
    float getNorm();
};


#endif //SDL2_3D_ENGINE_VERTEX_H
