//
// Created by darkhead on 3/1/20.
//

#include "../../headers/Game/AmmoType.h"

AmmoType::AmmoType() {

}

const std::string &AmmoType::getName() const {
    return name;
}

void AmmoType::setName(const std::string &name) {
    AmmoType::name = name;
}

int AmmoType::getAmount() const {
    return amount;
}

void AmmoType::setAmount(int amount) {
    AmmoType::amount = amount;
}

const std::string &AmmoType::getClassname() const {
    return classname;
}

void AmmoType::setClassname(const std::string &classname) {
    AmmoType::classname = classname;
}

Mesh3DAnimated* AmmoType::getModelProjectile() {
    return &model_projectile;
}

Mesh3DBody *AmmoType::getModelBox() {
    return &model_box;
}
