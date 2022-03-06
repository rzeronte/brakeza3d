
#ifndef SDL2_3D_ENGINE_VERTEX_H
#define SDL2_3D_ENGINE_VERTEX_H

#include <SDL2/SDL_surface.h>
#include <string>
#include <bullet/LinearMath/btVector3.h>
#include "Vertex4D.h"


class Vertex3D {
public:
    Vertex3D();

    Vertex3D(Vertex4D v);

    Vertex3D operator+(const Vertex3D &pm) const;

    Vertex3D operator-(const Vertex3D &pm) const;

    Vertex3D operator%(const Vertex3D &pm) const;

    float operator*(const Vertex3D &pm) const;

    bool operator!=(const Vertex3D &pm) const;

    bool operator==(const Vertex3D &pm) const;

    float x;
    float y;
    float z;

    // texture coordenates
    float u;
    float v;

    Vertex3D(float, float, float);

    Vertex3D(const float[3]);

    Vertex3D getNormalize() const;

    Vertex3D getInverse() const;

    Vertex3D getAbsolute() const;

    float getModule() const;

    float getSquaredLength() const;

    Vertex3D getScaled(float) const;

    Vertex3D getScaled(float xs, float ys, float zs) const;

    void setScaled(float);

    void setLength(float);

    float distance(Vertex3D);

    static Vertex3D zero();

    void saveToFloat3(float *v) const;

    void saveToBtVector3(btVector3 *v) const;

    void consoleInfo(const std::string& label, bool returnLine) const;

    Vertex4D createVertex4D();

};

#endif //SDL2_3D_ENGINE_VERTEX_H
