//
// Created by eduardo on 10/04/22.
//

#ifndef BRAKEDA3D_BOSSENEMY_H
#define BRAKEDA3D_BOSSENEMY_H


#include "../Enemies/EnemyGhost.h"
#include "../Weapons/AmmoProjectileBodyEmitter.h"

typedef enum {
    BOSS10_PHASE_ONE = 1,
    BOSS10_PHASE_TWO = 2
} Bosses10Phase;

class BossEnemy : public EnemyGhost {

public:
    BossEnemy();

    void onUpdate() override;

    virtual void load();
};


#endif //BRAKEDA3D_BOSSENEMY_H
