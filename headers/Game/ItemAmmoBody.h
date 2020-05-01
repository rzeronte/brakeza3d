//
// Created by darkhead on 2/1/20.
//

#ifndef BRAKEDA3D_ITEMAMMOBODY_H
#define BRAKEDA3D_ITEMAMMOBODY_H


#include "../Physics/Mesh3DBody.h"

class ItemAmmoBody : public Mesh3DBody {
public:
    int amount = 50;
    std::string ammoTypeClassname;

    const std::string &getAmmoTypeClassname() const;
    void setAmmoTypeClassname(const std::string &ammoTypeClassname);

    int  getAmount() const;
    void setAmount(int amount);

    void onUpdate();
};


#endif //BRAKEDA3D_ITEMAMMOBODY_H
