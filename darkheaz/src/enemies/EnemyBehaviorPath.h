
#ifndef BRAKEDA3D_ENEMYBEHAVIORPATH_H
#define BRAKEDA3D_ENEMYBEHAVIORPATH_H


#include <vector>
#include "EnemyBehavior.h"
#include "../../../include/Objects/Vector3D.h"

class EnemyBehaviorPath: public EnemyBehavior {
    std::vector<Vertex3D> path;
    Vertex3D currentFrom;
    Vertex3D currentTo;
    Vector3D currentDirection;
    float speed;

    int currentPointIndex;
public:
    EnemyBehaviorPath(float speed);

    void onUpdate(Vertex3D &position) override;

    void addPoint(Vertex3D point);

    void start();
    void nextPoint();

    EnemyBehavior *clone() override;
};


#endif //BRAKEDA3D_ENEMYBEHAVIORPATH_H
