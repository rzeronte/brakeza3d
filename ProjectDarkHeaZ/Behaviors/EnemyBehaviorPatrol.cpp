
#include "EnemyBehaviorPatrol.h"
#include "../../include/Render/Logging.h"
#include "../../include/Misc/Tools.h"

EnemyBehaviorPatrol::EnemyBehaviorPatrol(Vertex3D from, Vertex3D to, float speed)
:
    returnedCounter(0)
{
    this->origin = from;
    this->speed = speed;
    this->direction = Vector3D(from, to);
}

void EnemyBehaviorPatrol::onUpdate(Vertex3D &position)
{
    Object3DBehavior::onUpdate(position);

    if (!isEnabled()) return;

    calcPosition(position);
}

void EnemyBehaviorPatrol::calcPosition(Vertex3D &position)
{
    auto v = direction.getComponent().getScaled(
        abs(Tools::interpolate((float) sin((getExecutionTime() * speed) + (M_PI / 2)), -1, 1))
    );

    float length = v.getModule();

    returned = false;

    if (!hasMovedAway && length > BEHAVIOR_PATROL_FAR_TOLERANCE) {
        hasMovedAway = true;
    }

    if (hasMovedAway && length < BEHAVIOR_PATROL_CLOSEST_TOLERANCE) {
        hasMovedAway = false;
        returnedCounter++;
        returned = true;
    }

    position = origin + v;
}

float EnemyBehaviorPatrol::getSpeed() const
{
    return speed;
}

void EnemyBehaviorPatrol::setSpeed(float speed)
{
    EnemyBehaviorPatrol::speed = speed;
}

Object3DBehavior *EnemyBehaviorPatrol::clone()
{
    return new EnemyBehaviorPatrol(
        this->direction.vertex1,
        this->direction.vertex2,
        this->speed
    );
}

bool EnemyBehaviorPatrol::isReturned() const {
    return returned;
}

int EnemyBehaviorPatrol::getReturnedCounter() const {
    return returnedCounter;
}

void EnemyBehaviorPatrol::setReturnedCounter(int returnedCounter) {
    EnemyBehaviorPatrol::returnedCounter = returnedCounter;
}

void EnemyBehaviorPatrol::setStartPosition(Vertex3D &position) {
    Logging::Message("iep: %f", getExecutionTime());
    calcPosition(position);
}

EnemyBehaviorPatrol::~EnemyBehaviorPatrol() = default;
