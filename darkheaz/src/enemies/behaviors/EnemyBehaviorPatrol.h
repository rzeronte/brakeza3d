//
// Created by eduardo on 19/3/22.
//

#ifndef BRAKEDA3D_ENEMYBEHAVIORPATROL_H
#define BRAKEDA3D_ENEMYBEHAVIORPATROL_H


#include "../EnemyBehavior.h"
#include "../../../../include/Objects/Vector3D.h"

class EnemyBehaviorPatrol : public EnemyBehavior {
    Vertex3D origin;
    Vector3D direction;
    float speed;
public:

    EnemyBehaviorPatrol(Vertex3D from, Vertex3D to, float speed);

    void onUpdate(Vertex3D &position) override;

    void setSpeed(float speed);

    EnemyBehavior *clone() override;

    [[nodiscard]] float getSpeed() const;
};


#endif //BRAKEDA3D_ENEMYBEHAVIORPATROL_H
