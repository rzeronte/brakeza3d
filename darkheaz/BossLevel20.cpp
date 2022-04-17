//
// Created by eduardo on 17/04/22.
//

#include "include/BossLevel20.h"

AmmoProjectileBodyEmissor *BossLevel20::getProjectileEmissorLeft() const {
    return projectileEmissorLeft;
}

void BossLevel20::setProjectileEmissorLeft(AmmoProjectileBodyEmissor *projectileEmissorLeft) {
    BossLevel20::projectileEmissorLeft = projectileEmissorLeft;
}

AmmoProjectileBodyEmissor *BossLevel20::getProjectileEmissorRight() const {
    return projectileEmissorRight;
}

void BossLevel20::setProjectileEmissorRight(AmmoProjectileBodyEmissor *projectileEmissorRight) {
    BossLevel20::projectileEmissorRight = projectileEmissorRight;
}
