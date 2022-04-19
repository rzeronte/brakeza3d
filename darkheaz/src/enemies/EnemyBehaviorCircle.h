
#ifndef BRAKEDA3D_ENEMYBEHAVIORCIRCLE_H
#define BRAKEDA3D_ENEMYBEHAVIORCIRCLE_H


#include "EnemyBehavior.h"
#include "../../../include/Render/M3.h"

class EnemyBehaviorCircle: public EnemyBehavior {
    float speed;
    Vertex3D center;
    float radius;
    Vertex3D offset;
public:
    EnemyBehaviorCircle(const Vertex3D &center, float speed, float radius);

    void onUpdate(Vertex3D &position) override;

    EnemyBehavior *clone() override;
};


#endif //BRAKEDA3D_ENEMYBEHAVIORCIRCLE_H
