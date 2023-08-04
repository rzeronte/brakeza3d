#ifndef BRAKEZA3D_BOSSLEVEL30_H
#define BRAKEZA3D_BOSSLEVEL30_H


#include "BossEnemy.h"

typedef enum {
    ONE = 0,
    TWO = 1,
    THREE = 2,
} BossLevel30State;


class BossLevel30 : public BossEnemy {

    Counter *counter;

    EnemyGhost *turret;
public:
    BossLevel30State state;
    BossLevel30();

    void load();

    void onUpdate() override;

    [[nodiscard]] BossLevel30State getBossState() const;

    void setBossState(BossLevel30State state);

    void setRandomBossState();
};


#endif //BRAKEZA3D_BOSSLEVEL30_H
