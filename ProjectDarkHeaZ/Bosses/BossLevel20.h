//
// Created by edu on 25/01/24.
//

#ifndef BRAKEZA3D_BOSSLEVEL20_H
#define BRAKEZA3D_BOSSLEVEL20_H


#include "BossEnemy.h"

class BossLevel20 : public BossEnemy {
    Counter changePhase;

public:
    void onUpdate() override;
};


#endif //BRAKEZA3D_BOSSLEVEL20_H
