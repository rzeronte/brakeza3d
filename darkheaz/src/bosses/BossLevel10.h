//
// Created by eduardo on 17/04/22.
//

#ifndef BRAKEDA3D_BOSSLEVEL10_H
#define BRAKEDA3D_BOSSLEVEL10_H


#include "../weapons/AmmoProjectileBodyEmissor.h"
#include "BossEnemy.h"

class BossLevel10 : public BossEnemy {
private:
    AmmoProjectileBodyEmissor *projectileEmissor;
public:
    AmmoProjectileBodyEmissor *getProjectileEmissor() const;

    void setProjectileEmissor(AmmoProjectileBodyEmissor *projectileEmissor);
};


#endif //BRAKEDA3D_BOSSLEVEL10_H
