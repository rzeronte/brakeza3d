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
        active(true),
        intensity(1.0f),
        start(start),
        rotation(rotation),
        end(anEnd)
    {
    }

    bool active;
    float intensity;
    Vertex3D start;
    M3 rotation;
    Vertex3D end;

    Vertex3D startWorld;
    Vertex3D endWorld;
};

class TentacleIK: public Object3D {
    std::vector<TentacleSegment *> joints;
    Vertex3D target;
    Vertex3D followPosition;
    Vertex3D velocity;
    float maxLength;
    Counter cadence;

public:
    TentacleIK(Vertex3D position, Object3D* parent, Object3D *target, float maxLength);

    void onUpdate() override;

    void postUpdate() override;

    void addJoint(TentacleSegment *joint);

    void updateRotations();

private:

    void transformJoints();

    void applySinusoidalMovement(float amplitude, float frequency, float time);

public:
    void setTarget(Object3D *target);

    void draw();

    void IKForwardSolver();

    void transformJoint(int i);

    void setRootPosition(Vertex3D position);

    void moveFinalToTargetPosition();

    void moveFinalToRandomNear();

    void drawTargetPosition();

    void fadeOut();

    void fadeIn();

    void hide();

    void show();
};

#endif //BRAKEZA3D_TENTACLEIK_H
