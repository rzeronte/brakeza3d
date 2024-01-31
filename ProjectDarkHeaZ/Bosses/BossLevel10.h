//
// Created by eduardo on 17/04/22.
//

#ifndef BRAKEDA3D_BOSSLEVEL10_H
#define BRAKEDA3D_BOSSLEVEL10_H

#include "../Weapons/AmmoProjectileBodyEmitter.h"
#include "BossEnemy.h"
#include "../Behaviors/EnemyBehaviorPatrol.h"
#include "../Behaviors/EnemyBehaviorPath.h"

class BossLevel10 : public BossEnemy {
private:
    Bosses10Phase phase;

    EnemyBehaviorPath *behaviorPhaseOne;
    EnemyBehaviorPatrol *behaviorPhaseTwo;
public:
    explicit BossLevel10();

    ~BossLevel10() override;

    void onUpdate() override;

    void invertPhase();
};


#endif //BRAKEDA3D_BOSSLEVEL10_H
