//
// Created by eduardo on 15/04/22.
//

#ifndef BRAKEDA3D_ENEMYBEHAVIORRANDOM_H
#define BRAKEDA3D_ENEMYBEHAVIORRANDOM_H

#include "../../include/Behaviors/Object3DBehavior.h"

class EnemyBehaviorRandom: public Object3DBehavior {
private:
    Vertex3D direction;
    float speed;
public:
    explicit EnemyBehaviorRandom(float speed);

    void onUpdate(Vertex3D &position) override;

    [[nodiscard]] float getSpeed() const;

    Object3DBehavior *clone() override;

    void setStartPosition(Vertex3D &position) override;
};


#endif //BRAKEDA3D_ENEMYBEHAVIORRANDOM_H
