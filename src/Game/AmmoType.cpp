#include "../../include/Game/AmmoType.h"

AmmoType::AmmoType() {
    this->amount = 0;
    this->reloads = 0;
    this->reload_amount = 0;

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

Mesh3DBody *AmmoType::getModelProjectile() {
    return &model_projectile;
}

Mesh3DBody *AmmoType::getModelBox() {
    return &model_box;
}

int AmmoType::getReloads() const {
    return reloads;
}

void AmmoType::setReloads(int reloads) {
    AmmoType::reloads = reloads;
}

int AmmoType::getReloadAmount() const {
    return reload_amount;
}

void AmmoType::setReloadAmount(int reloadAmount) {
    reload_amount = reloadAmount;
}
