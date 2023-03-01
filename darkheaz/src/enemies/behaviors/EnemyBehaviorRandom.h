//
// Created by eduardo on 15/04/22.
//

#ifndef BRAKEDA3D_ENEMYBEHAVIORRANDOM_H
#define BRAKEDA3D_ENEMYBEHAVIORRANDOM_H


#include "../EnemyBehavior.h"

class EnemyBehaviorRandom: public EnemyBehavior {
private:
    Vertex3D direction;
    float speed;
public:
    explicit EnemyBehaviorRandom(float speed);

    void onUpdate(Vertex3D &position) override;

    [[nodiscard]] float getSpeed() const;

    EnemyBehavior *clone() override;
};


#endif //BRAKEDA3D_ENEMYBEHAVIORRANDOM_H
