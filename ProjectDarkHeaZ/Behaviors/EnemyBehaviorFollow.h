//
// Created by eduardo on 20/3/22.
//

#ifndef BRAKEDA3D_ENEMYBEHAVIORFOLLOW_H
#define BRAKEDA3D_ENEMYBEHAVIORFOLLOW_H


#include "../../include/Objects/Object3D.h"

class EnemyBehaviorFollow : public Object3DBehavior {
    float speed;
    float separation;
    Object3D *target;
public:
    EnemyBehaviorFollow(Object3D *target, float speed, float separation);

    void onUpdate(Vertex3D &position) override;

    float getSpeed() const;

    void setSpeed(float speed);

    float getSeparation() const;

    void setSeparation(float separation);

    Object3D *getTarget() const;

    void setTarget(Object3D *target);

    Object3DBehavior *clone() override;

    void setStartPosition(Vertex3D &position) override;
};


#endif //BRAKEDA3D_ENEMYBEHAVIORFOLLOW_H
