
#include "../enemies/behaviors/EnemyBehaviorPatrol.h"
#include "../../../include/Render/Logging.h"
#include "../../../include/Misc/Tools.h"

EnemyBehaviorPatrol::EnemyBehaviorPatrol(Vertex3D from, Vertex3D to, float speed)
{
    this->origin = from;
    this->speed = speed;
    this->direction = Vector3D(from, to);
}

void EnemyBehaviorPatrol::onUpdate(Vertex3D &position)
{
    EnemyBehavior::onUpdate(position);

    position = origin + direction.getComponent().getScaled(
        abs(Tools::interpolate(sin(getExecutionTime() * speed) , -1, 1))
    );
}

float EnemyBehaviorPatrol::getSpeed() const
{
    return speed;
}

void EnemyBehaviorPatrol::setSpeed(float speed)
{
    EnemyBehaviorPatrol::speed = speed;
}

EnemyBehavior *EnemyBehaviorPatrol::clone()
{
    return new EnemyBehaviorPatrol(
        this->direction.vertex1,
        this->direction.vertex2,
        this->speed
    );
}
