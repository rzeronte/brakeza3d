
#ifndef SDL2_3D_ENGINE_BILLBOARD_H
#define SDL2_3D_ENGINE_BILLBOARD_H

#include "Object3D.h"
#include "Texture.h"
#include "Triangle.h"

class Object3D;

class Billboard {
public:
    float width;
    float height;

    Triangle T1;
    Triangle T2;

    bool drawable;

    Vertex Q1;
    Vertex Q2;
    Vertex Q3;
    Vertex Q4;


    Texture *texture = new Texture();

    Billboard();

    void updateUnconstrainedQuad(float w, float h, Object3D *o, Vertex up, Vertex right);
    void loadTexture(std::string);

    bool isDrawable() const;
    void setDrawable(bool drawable);

};


#endif //SDL2_3D_ENGINE_BILLBOARD_H
