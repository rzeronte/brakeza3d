
#ifndef SDL2_3D_ENGINE_BILLBOARD_H
#define SDL2_3D_ENGINE_BILLBOARD_H

#include "../Objects/Object3D.h"
#include "../Objects/Triangle3D.h"
#include "Texture.h"

class Billboard {
public:
    float width;
    float height;

    Triangle T1;
    Triangle T2;

    bool drawable;

    Vertex3D Q1;
    Vertex3D Q2;
    Vertex3D Q3;
    Vertex3D Q4;

    Texture *texture = new Texture();

    Billboard();

    void updateUnconstrainedQuad(Object3D *o, Vertex3D up, Vertex3D right);
    void loadTexture(std::string);

    void setTrianglesTexture(Texture *);
    void reassignTexture();

    bool isDrawable() const;
    void setDrawable(bool drawable);

};


#endif //SDL2_3D_ENGINE_BILLBOARD_H
