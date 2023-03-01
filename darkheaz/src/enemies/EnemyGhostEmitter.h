//
// Created by eduardo on 20/3/22.
//

#ifndef BRAKEDA3D_ENEMYGHOSTRESPAWNER_H
#define BRAKEDA3D_ENEMYGHOSTRESPAWNER_H


#include "../../../include/Misc/Counter.h"
#include "EnemyGhost.h"

class EnemyGhostEmitter: public Object3D {
public:
    EnemyGhostEmitter(EnemyGhost *object, float step);
    void onUpdate() override;
    void startCounter();
private:
    EnemyGhost *object;
    float step;
    Counter counter;
};


#endif //BRAKEDA3D_ENEMYGHOSTRESPAWNER_H
