//
// Created by eduardo on 20/3/22.
//

#ifndef BRAKEDA3D_ENEMYGHOSTRESPAWNER_H
#define BRAKEDA3D_ENEMYGHOSTRESPAWNER_H


#include "../Misc/Counter.h"
#include "EnemyGhost.h"

class EnemyGhostRespawner: public Object3D {
public:
    EnemyGhostRespawner(EnemyGhost *object, float step);
    void onUpdate() override;
private:
    EnemyGhost *object;
    float step;
    Counter counter;
};


#endif //BRAKEDA3D_ENEMYGHOSTRESPAWNER_H
