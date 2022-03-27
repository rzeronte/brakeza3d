//
// Created by eduardo on 19/3/22.
//

#ifndef BRAKEDA3D_ENEMYBEHAVIORPATROL_H
#define BRAKEDA3D_ENEMYBEHAVIORPATROL_H


#include "EnemyBehavior.h"
#include "../Objects/Vector3D.h"

class EnemyBehaviorPatrol : public EnemyBehavior {
    Vertex3D origin;
    Vector3D v;
    float speed;
public:

    EnemyBehaviorPatrol(Vertex3D from, Vertex3D to, float speed);

    void onUpdate(Vertex3D &position) override;

    float getSpeed() const;

    void setSpeed(float speed);
};


#endif //BRAKEDA3D_ENEMYBEHAVIORPATROL_H
