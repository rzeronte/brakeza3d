//
// Created by eduardo on 10/04/22.
//

#ifndef BRAKEDA3D_ENEMYGHOSTRESPAWNEREMISSOR_H
#define BRAKEDA3D_ENEMYGHOSTRESPAWNEREMISSOR_H


#include "../Misc/Counter.h"
#include "EnemyGhost.h"
#include "EnemyGhostRespawner.h"

class EnemyGhostRespawnerEmissor: public EnemyGhost {
    Counter counter;
    float step;
    bool active;
    EnemyGhost *enemy;
    std::vector<EnemyGhostRespawner*> respawners;

    Counter *counterStopDuration;
    Counter *counterStopEvery;
    bool stop;
    float stopDuration;
    float stopEvery;
public:
    EnemyGhostRespawnerEmissor(float step, EnemyGhost *enemy);


    const Counter &getCounter() const;

    void setCounter(const Counter &counter);

    bool isActive() const;

    void setActive(bool active);

    void onUpdate() override;

    void addEnemy();

    bool isStop() const;

    void setStop(bool stop);

    float getStopDuration() const;

    void setStopDuration(float stopDuration);

    float getStopEvery() const;

    void setStopEvery(float stopEverySeconds);

    float getStep() const;

    void setStep(float step);

    EnemyGhost *getEnemy() const;

    void setEnemy(EnemyGhost *enemy);
};


#endif //BRAKEDA3D_ENEMYGHOSTRESPAWNEREMISSOR_H
