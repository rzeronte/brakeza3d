#include "../../headers/Game/ItemAmmoGhost.h"

int ItemAmmoGhost::getAmount() const {
    return amount;
}

void ItemAmmoGhost::setAmount(int amount) {
    ItemAmmoGhost::amount = amount;
}

const std::string &ItemAmmoGhost::getAmmoTypeClassname() const {
    return ammoTypeClassname;
}

void ItemAmmoGhost::setAmmoTypeClassname(const std::string &ammoTypeClassname) {
    ItemAmmoGhost::ammoTypeClassname = ammoTypeClassname;
}

void ItemAmmoGhost::onUpdate() {
    this->setRotation(this->getRotation() * M3::getMatrixRotationForEulerAngles(0, 0, 2));
    Mesh3D::onUpdate();
}