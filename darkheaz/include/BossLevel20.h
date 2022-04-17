//
// Created by eduardo on 17/04/22.
//

#ifndef BRAKEDA3D_BOSSLEVEL20_H
#define BRAKEDA3D_BOSSLEVEL20_H


#include "AmmoProjectileBodyEmissor.h"
#include "BossEnemy.h"

class BossLevel20 : public BossEnemy {
private:
    AmmoProjectileBodyEmissor *projectileEmissorLeft;
    AmmoProjectileBodyEmissor *projectileEmissorRight;

public:
    AmmoProjectileBodyEmissor *getProjectileEmissorLeft() const;

    void setProjectileEmissorLeft(AmmoProjectileBodyEmissor *projectileEmissorLeft);

    AmmoProjectileBodyEmissor *getProjectileEmissorRight() const;

    void setProjectileEmissorRight(AmmoProjectileBodyEmissor *projectileEmissorRight);
};


#endif //BRAKEDA3D_BOSSLEVEL20_H
