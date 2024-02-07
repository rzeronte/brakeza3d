//
// Created by edu on 25/01/24.
//

#ifndef BRAKEZA3D_BOSSLEVEL25_H
#define BRAKEZA3D_BOSSLEVEL25_H


#include "BossEnemy.h"
#include "../Behaviors/EnemyBehaviorPath.h"
#include "../Behaviors/EnemyBehaviorPatrol.h"

#define SPEED_TRANSPARENCY 0.3

class BossLevel25 : public BossEnemy {
    Counter changeStopPhase;
    Bosses10Phase phase;
    float progress;

    EnemyBehaviorPath *behaviorPhaseOne;
    EnemyBehaviorPatrol *behaviorPhaseTwo;
    EnemyBehaviorPath *behaviorPhaseFour;
public:
    BossLevel25();

    void onUpdate() override;

    void invertPhase();

    void postUpdate() override;
};


#endif //BRAKEZA3D_BOSSLEVEL25_H
