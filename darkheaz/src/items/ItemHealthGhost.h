#ifndef BRAKEDA3D_ITEMHEALTHGHOST_H
#define BRAKEDA3D_ITEMHEALTHGHOST_H


#include "../../../include/Physics/Mesh3DGhost.h"

class ItemHealthGhost : public Mesh3DGhost {
    float aid;
public:
    ItemHealthGhost();

    void setAid(float aid);

    float getAid() const;

    void resolveCollision(Collisionable *collisionable) override;
};


#endif //BRAKEDA3D_ITEMHEALTHGHOST_H
