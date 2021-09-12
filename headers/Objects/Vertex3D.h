
#ifndef SDL2_3D_ENGINE_VERTEX_H
#define SDL2_3D_ENGINE_VERTEX_H

#include <SDL2/SDL_surface.h>
#include <string>
#include <bullet/LinearMath/btVector3.h>


class Vertex3D {
public:
    Vertex3D operator +(const Vertex3D &pm);
    Vertex3D operator -(const Vertex3D &pm);

    Vertex3D operator %(const Vertex3D &pm);
    float    operator *(const Vertex3D &pm);
    bool     operator !=(const Vertex3D &pm);
    bool     operator ==(const Vertex3D &pm);

    float x = 0;
    float y = 0;
    float z = 0;

    // texture coordenates
    float u;
    float v;

    Vertex3D();
    Vertex3D(float, float, float);
    Vertex3D(float[3]);

    Vertex3D getNormalize();
    Vertex3D getInverse();
    Vertex3D getAbsolute();

    float getModule();
    float squaredLength();

    Vertex3D getScaled(float);
    Vertex3D getScaled(float xs, float ys, float zs);
    void setScaled(float);

    void setLength(float);

    float distance(Vertex3D);

    static Vertex3D zero();
    void saveToFloat3(float *v);
    void saveToBtVector3(btVector3 *v);

    void consoleInfo(std::string label, bool returnLine);
};

#endif //SDL2_3D_ENGINE_VERTEX_H
