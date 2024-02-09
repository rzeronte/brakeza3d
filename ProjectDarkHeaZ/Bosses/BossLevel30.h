#ifndef BRAKEZA3D_BOSSLEVEL30_H
#define BRAKEZA3D_BOSSLEVEL30_H


#include "BossEnemy.h"
#include "../Behaviors/EnemyBehaviorPath.h"

#define SPEED_BOSS_LASERS 0.3
#define BOSS_LASER_LENGTH 2.5

class BossLevel30 : public BossEnemy {
    std::vector<RayLight *> rays;
    std::vector<RayLight *> horizontal;
    float progress;
    Counter changePhase;
    Bosses10Phase phase;

    EnemyBehaviorPath *behaviorPhaseOne;

public:
    BossLevel30();

    void onUpdate() override;

    void updateRays();

    void invertPhase();
};


#endif //BRAKEZA3D_BOSSLEVEL30_H
