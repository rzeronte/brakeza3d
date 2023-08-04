//
// Created by eduardo on 10/04/22.
//

#ifndef BRAKEDA3D_BOSSENEMY_H
#define BRAKEDA3D_BOSSENEMY_H


#include "../Enemies/EnemyGhost.h"
#include "../Weapons/AmmoProjectileBodyEmitter.h"

class BossEnemy : public EnemyGhost {

public:
    BossEnemy();

    void onUpdate() override;

    virtual void load();
};


#endif //BRAKEDA3D_BOSSENEMY_H
