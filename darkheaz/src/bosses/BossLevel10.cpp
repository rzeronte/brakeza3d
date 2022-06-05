//
// Created by eduardo on 17/04/22.
//

#include "BossLevel10.h"

BossLevel10::BossLevel10() {
    this->projectileEmissor = nullptr;
}

AmmoProjectileBodyEmissor *BossLevel10::getProjectileEmissor() const {
    return projectileEmissor;
}

void BossLevel10::setProjectileEmissor(AmmoProjectileBodyEmissor *projectileEmissor) {
    BossLevel10::projectileEmissor = projectileEmissor;
}
