//
// Created by darkhead on 2/1/20.
//

#include "../../headers/Game/ItemAmmoBody.h"

int ItemAmmoBody::getAmount() const {
    return amount;
}

void ItemAmmoBody::setAmount(int amount) {
    ItemAmmoBody::amount = amount;
}

const std::string &ItemAmmoBody::getAmmoTypeClassname() const {
    return ammoTypeClassname;
}

void ItemAmmoBody::setAmmoTypeClassname(const std::string &ammoTypeClassname) {
    ItemAmmoBody::ammoTypeClassname = ammoTypeClassname;
}

void ItemAmmoBody::onUpdate()
{
    this->setRotation ( this->getRotation() * M3::getMatrixRotationForEulerAngles(0, 3, 0));
    Mesh3D::onUpdate();
}
