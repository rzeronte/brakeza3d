//
// Created by darkhead on 2/1/20.
//

#ifndef BRAKEDA3D_ITEMHEALTHGHOST_H
#define BRAKEDA3D_ITEMHEALTHGHOST_H


#include "../Physics/Mesh3DGhost.h"

class ItemHealthGhost: public Mesh3DGhost{
    float aid;
public:
    ItemHealthGhost();

    void  setAid(float aid);
    float getAid() const;
};


#endif //BRAKEDA3D_ITEMHEALTHGHOST_H
