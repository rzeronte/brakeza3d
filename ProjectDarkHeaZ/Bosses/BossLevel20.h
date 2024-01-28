//
// Created by edu on 25/01/24.
//

#ifndef BRAKEZA3D_BOSSLEVEL20_H
#define BRAKEZA3D_BOSSLEVEL20_H


#include "BossEnemy.h"
#include "../Behaviors/EnemyBehaviorPath.h"
#include "../Behaviors/EnemyBehaviorPatrol.h"

class BossLevel20 : public BossEnemy {
    Bosses10Phase phase;

    EnemyBehaviorPath *behaviorPhaseOne;
    EnemyBehaviorPath *behaviorPhaseTwo;
    float progress;

    Counter counter;
    M3 r1;
    M3 r2;
    M3 r3;
public:
    BossLevel20();

    void onUpdate() override;
    void invertPhase();

    void setEmitterIndexEnabled(int i, bool value);

    void onStart() override;
};


#endif //BRAKEZA3D_BOSSLEVEL20_H
