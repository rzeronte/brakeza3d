//
// Created by eduardo on 19/3/22.
//

#ifndef BRAKEDA3D_ENEMYBEHAVIORPATROL_H
#define BRAKEDA3D_ENEMYBEHAVIORPATROL_H

#include "../../include/Objects/Vector3D.h"
#include "../../include/Behaviors/Object3DBehavior.h"

#define BEHAVIOR_PATROL_CLOSEST_TOLERANCE 0.5f
#define BEHAVIOR_PATROL_FAR_TOLERANCE 1.0f

class EnemyBehaviorPatrol : public Object3DBehavior {
    Vertex3D origin;
    Vector3D direction;
    float speed;
    int returnedCounter;

    bool returned = false;
    bool hasMovedAway = false;
public:

    EnemyBehaviorPatrol(Vertex3D from, Vertex3D to, float speed);

    virtual ~EnemyBehaviorPatrol();

    void onUpdate(Vertex3D &position) override;

    void setSpeed(float speed);

    Object3DBehavior *clone() override;

    [[nodiscard]] float getSpeed() const;

    [[nodiscard]] bool isReturned() const;

    int getReturnedCounter() const;

    void setReturnedCounter(int returnedCounter);
};


#endif //BRAKEDA3D_ENEMYBEHAVIORPATROL_H
