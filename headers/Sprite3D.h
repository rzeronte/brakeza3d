
#ifndef BRAKEDA3D_SPRITE_H
#define BRAKEDA3D_SPRITE_H


#include "Object3D.h"
#include "BillboardDirectional.h"

class Sprite3D : public Object3D {
    BillboardDirectional *billboard;

public:
    float width;
    float height;
    LTimer *timer;

    Sprite3D();

    BillboardDirectional *getBillboard() const;

    void draw(Camera *cam);
    void updateTrianglesCoordinates(Camera *cam);
    void setTimer(LTimer *);
};


#endif //BRAKEDA3D_SPRITE_H
