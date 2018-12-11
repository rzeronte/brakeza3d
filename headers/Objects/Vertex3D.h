//
// Created by darkhead on 28/4/18.
//

#ifndef SDL2_3D_ENGINE_VERTEX_H
#define SDL2_3D_ENGINE_VERTEX_H


#include <SDL_surface.h>
#include <string>

class Vertex3D {
public:
    Vertex3D operator +(const Vertex3D &pm);
    Vertex3D operator -(const Vertex3D &pm);

    Vertex3D operator %(const Vertex3D &pm);
    float    operator *(const Vertex3D &pm);

    float x = 0;
    float y = 0;
    float z = 0;

    // texture coordenates
    float u = 0;
    float v = 0;

    Vertex3D();
    Vertex3D(float, float, float);

    Vertex3D getNormalize();
    Vertex3D getInverse();
    float getModule();

    void consoleInfo(std::string label, bool);

    static float dotProduct(Vertex3D, Vertex3D);
};


#endif //SDL2_3D_ENGINE_VERTEX_H
