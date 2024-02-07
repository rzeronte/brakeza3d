//
// Created by eduardo on 10/04/22.
//

#ifndef BRAKEDA3D_BOSSENEMY_H
#define BRAKEDA3D_BOSSENEMY_H


#include "../Enemies/EnemyGhost.h"
#include "../Weapons/AmmoProjectileBodyEmitter.h"

typedef enum {
    BOSS10_PHASE_ONE = 1,
    BOSS10_PHASE_TWO = 2,
    BOSS10_PHASE_THREE = 3,
    BOSS10_PHASE_FOUR = 4,

} Bosses10Phase;

class BossEnemy : public EnemyGhost {

public:
    BossEnemy();

    void onUpdate() override;

    virtual void load();

    static Point2D getScreenPoint(int x, int y)
    {
        const int xFrom = x * EngineSetup::get()->screenWidth / 100;
        const int yFrom = y * EngineSetup::get()->screenHeight / 100;

        return Point2D(xFrom, yFrom);
    }

};


#endif //BRAKEDA3D_BOSSENEMY_H
