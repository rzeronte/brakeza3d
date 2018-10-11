
#ifndef BRAKEDA3D_SPRITE_H
#define BRAKEDA3D_SPRITE_H


#include "Object3D.h"
#include "BillboardDirectional.h"

class Sprite : public Object3D {
    BillboardDirectional *billboard;

public:
    std::string base_name;

    Sprite(std::string base_name);

    BillboardDirectional *getBillboard() const;

    void setBillboard(BillboardDirectional *billboard);
    void draw(Camera *cam);
};


#endif //BRAKEDA3D_SPRITE_H
