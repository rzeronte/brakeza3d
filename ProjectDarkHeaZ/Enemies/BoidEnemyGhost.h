//
// Created by eduardo on 26/08/23.
//

#ifndef BRAKEZA3D_BOIDENEMYGHOST_H
#define BRAKEZA3D_BOIDENEMYGHOST_H


#include "EnemyGhost.h"

class BoidEnemyGhost : public EnemyGhost {

    void shoot(Object3D *target) override;

};


#endif //BRAKEZA3D_BOIDENEMYGHOST_H
