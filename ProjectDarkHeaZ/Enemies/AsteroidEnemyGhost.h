//
// Created by eduardo on 15/04/22.
//

#ifndef BRAKEDA3D_ASTEROIDENEMYGHOST_H
#define BRAKEDA3D_ASTEROIDENEMYGHOST_H


#include "EnemyGhost.h"

class AsteroidEnemyGhost: public EnemyGhost {
private:
    bool explode;
    int explodeNumberPartitions;

    Mesh3D* modelPartitions;
public:
    void onUpdate() override;

    [[nodiscard]] bool isExplode() const;

    void setExplode(bool explode);

    [[nodiscard]] Mesh3D *getModelPartitions() const;

    void setModelPartitions(Mesh3D *modelPartitions);

    void addAsteroid();

    [[nodiscard]] int getExplodeNumberPartitions() const;

    void setExplodeNumberPartitions(int explodeNumberPartitions);
};


#endif //BRAKEDA3D_ASTEROIDENEMYGHOST_H
