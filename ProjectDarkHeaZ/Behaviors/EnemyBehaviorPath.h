
#ifndef BRAKEDA3D_ENEMYBEHAVIORPATH_H
#define BRAKEDA3D_ENEMYBEHAVIORPATH_H


#include <vector>
#include "../../include/Objects/Vector3D.h"
#include "../../include/Behaviors/Object3DBehavior.h"

class EnemyBehaviorPath: public Object3DBehavior {
    std::vector<Vertex3D> path;
    Vertex3D currentFrom;
    Vertex3D currentTo;
    Vector3D currentDirection;
    float speed;

    int returnedCounter;
    bool returned = false;

    int currentPointIndex;
public:
    explicit EnemyBehaviorPath(float speed);

    void onUpdate(Vertex3D &position) override;

    void addPoint(Vertex3D point);

    void start();
    void nextPoint();

    Object3DBehavior *clone() override;

    [[nodiscard]] int getReturnedCounter() const;

    void setReturnedCounter(int returnedCounter);
};


#endif //BRAKEDA3D_ENEMYBEHAVIORPATH_H
