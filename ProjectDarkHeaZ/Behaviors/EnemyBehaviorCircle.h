
#ifndef BRAKEDA3D_ENEMYBEHAVIORCIRCLE_H
#define BRAKEDA3D_ENEMYBEHAVIORCIRCLE_H


#include "../../include/Render/M3.h"
#include "../../include/Behaviors/Object3DBehavior.h"

class EnemyBehaviorCircle: public Object3DBehavior {
    float speed;
    Vertex3D center;
    float radius;
    Vertex3D offset;
public:
    EnemyBehaviorCircle(const Vertex3D &center, float speed, float radius);

    void onUpdate(Vertex3D &position) override;

    Object3DBehavior *clone() override;

    void setStartPosition(Vertex3D &position) override;
};


#endif //BRAKEDA3D_ENEMYBEHAVIORCIRCLE_H
