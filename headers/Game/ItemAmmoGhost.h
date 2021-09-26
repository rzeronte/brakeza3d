//
// Created by darkhead on 2/1/20.
//

#ifndef BRAKEDA3D_ITEMAMMOGHOST_H
#define BRAKEDA3D_ITEMAMMOGHOST_H


#include "../Physics/Mesh3DGhost.h"

class ItemAmmoGhost : public Mesh3DGhost {
public:
    int amount = 50;
    std::string ammoTypeClassname;


    const std::string &getAmmoTypeClassname() const;

    void setAmmoTypeClassname(const std::string &ammoTypeClassname);

    int getAmount() const;

    void setAmount(int amount);

    void onUpdate();
};


#endif //BRAKEDA3D_ITEMAMMOGHOST_H
