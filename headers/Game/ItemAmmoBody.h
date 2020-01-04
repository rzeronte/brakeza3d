//
// Created by darkhead on 2/1/20.
//

#ifndef BRAKEDA3D_ITEMAMMOBODY_H
#define BRAKEDA3D_ITEMAMMOBODY_H


#include "../Physics/BillboardBody.h"

class ItemAmmoBody : public BillboardBody {
public:
    int amount = 50;
    std::string ammoTypeClassname;

    const std::string &getAmmoTypeClassname() const;
    void setAmmoTypeClassname(const std::string &ammoTypeClassname);

    int  getAmount() const;
    void setAmount(int amount);
};


#endif //BRAKEDA3D_ITEMAMMOBODY_H
