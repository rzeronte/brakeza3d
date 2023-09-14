//
// Created by Eduardo on 09/09/2023.
//

#ifndef BRAKEZA3D_TENTACLEIK_H
#define BRAKEZA3D_TENTACLEIK_H


#include <vector>
#include "Object3D.h"
#include "../Misc/Counter.h"

class TentacleSegment {
public:
    TentacleSegment(const Vertex3D &start, const Vertex3D &anEnd, const M3 &rotation)
    :
        start(start),
        end(anEnd),
        rotation(rotation)
    {
    }

    Vertex3D start;
    M3 rotation;
    Vertex3D end;

    Vertex3D startWorld;
    Vertex3D endWorld;
};

class TentacleIK: public Object3D {
    std::vector<TentacleSegment *> joints;
    Object3D *target;
    Vertex3D velocity;
public:
    TentacleIK(Vertex3D position, Object3D *target);

    void onUpdate() override;

    void postUpdate() override;

    void drawOnUpdateSecondPass() override;

    void onDrawHostBuffer() override;

    void addJoint(TentacleSegment *joint);

    void updateRotations();

private:

    void transformJoints();
    void applySinusoidalMovement(float amplitude, float frequency, float time);

public:
    void setTarget(Object3D *target);

    void draw();

    void IKForwardSolver();

    void attractRootToTarget();

    void transformJoint(int i);
    void transformJointSpiral(int i);

    void updateInertiaRoot();
};

#endif //BRAKEZA3D_TENTACLEIK_H
