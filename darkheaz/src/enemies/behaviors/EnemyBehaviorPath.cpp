
#include "EnemyBehaviorPath.h"
#include "../../../../include/Render/Logging.h"
#include "../../../../include/Render/Drawable.h"

EnemyBehaviorPath::EnemyBehaviorPath(float speed)
{
    this->speed = speed;
    this->currentPointIndex = -1;
    this->path.resize(0);
}

EnemyBehavior *EnemyBehaviorPath::clone()
{
    return new EnemyBehaviorPath(this->speed);
}

void EnemyBehaviorPath::onUpdate(Vertex3D &position)
{
    EnemyBehavior::onUpdate(position);

    Vertex3D movement = currentDirection.getComponent().getScaled(getExecutionTime() * speed);
    position = this->currentFrom + movement;

    if (movement.getModule() >= currentDirection.getComponent().getModule()) {
        nextPoint();
    }
}

void EnemyBehaviorPath::addPoint(Vertex3D point)
{
    this->path.push_back(point);
}

void EnemyBehaviorPath::nextPoint()
{
    if (currentPointIndex + 1 < (int) path.size() - 1) {
        currentPointIndex++;
        this->currentFrom = path[currentPointIndex];
        this->currentTo = path[currentPointIndex+1];
        this->currentDirection = Vector3D(currentFrom, currentTo);
    } else {
        std::reverse(path.begin(),path.end());
        start();
    }

    resetTimer();
}

void EnemyBehaviorPath::start()
{
    this->currentPointIndex = 0;
    this->currentFrom = path[currentPointIndex];
    this->currentTo = path[currentPointIndex+1];

    this->currentDirection = Vector3D(currentFrom, currentTo);
}

