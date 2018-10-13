
#ifndef BRAKEDA3D_SPRITE_H
#define BRAKEDA3D_SPRITE_H


#include "Object3D.h"
#include "BillboardDirectional.h"

class Sprite3D : public Object3D {
    BillboardDirectional *billboard;

public:
    float width;
    float height;

    Sprite3D();

    BillboardDirectional *getBillboard() const;

    void draw(Camera *cam);
};


#endif //BRAKEDA3D_SPRITE_H
