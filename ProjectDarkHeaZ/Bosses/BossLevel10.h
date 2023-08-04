//
// Created by eduardo on 17/04/22.
//

#ifndef BRAKEDA3D_BOSSLEVEL10_H
#define BRAKEDA3D_BOSSLEVEL10_H

typedef enum {
    BOSS10_PHASE_ONE = 1,
    BOSS10_PHASE_TWO = 2
} Bosses10Phase;

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

    void onUpdate() override;

    void invertPhase();

    Point2D getScreenPoint(int x, int y);
};


#endif //BRAKEDA3D_BOSSLEVEL10_H
