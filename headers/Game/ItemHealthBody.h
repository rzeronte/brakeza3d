//
// Created by darkhead on 2/1/20.
//

#ifndef BRAKEDA3D_ITEMHEALTHBODY_H
#define BRAKEDA3D_ITEMHEALTHBODY_H


#include "../Physics/BillboardBody.h"

class ItemHealthBody: public BillboardBody{
    float aid;
public:
    ItemHealthBody();

    void  setAid(float aid);
    float getAid() const;
};


#endif //BRAKEDA3D_ITEMHEALTHBODY_H
