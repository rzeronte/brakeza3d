#ifndef BRAKEDA3D_BOSSLEVEL20_H
#define BRAKEDA3D_BOSSLEVEL20_H


#include "../Weapons/AmmoProjectileBodyEmitter.h"
#include "BossEnemy.h"
#include "BossLevel10.h"

#define SPEED_BOSS_LASERS 0.5
#define BOSS_LASER_LENGTH 4

class BossLevel20 : public BossEnemy {
private:
    Bosses10Phase phase;
    Counter changePhase;
    float progress;

    std::vector<RayLight *> rays;
public:
    BossLevel20();

    void onUpdate() override;

    void invertPhase();

    void onStart() override;

    void updateRays();
};


#endif //BRAKEDA3D_BOSSLEVEL20_H
