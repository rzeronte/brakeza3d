
#ifndef BRAKEDA3D_BILLBOARDDIRECTIONAL_H
#define BRAKEDA3D_BILLBOARDDIRECTIONAL_H


#include "Billboard.h"

class BillboardDirectional : public Billboard {
public:
    Texture **textures = new Texture*[7];

    BillboardDirectional();

    void loadTextureDirectional(std::string);
    void updateTextureFromCameraAngle(Object3D *, Camera *);
};


#endif //BRAKEDA3D_BILLBOARDDIRECTIONAL_H
