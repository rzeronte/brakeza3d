
#include "EnemyBehaviorRandom.h"
#include "../../include/Misc/Tools.h"
#include "../../include/ComponentsManager.h"

EnemyBehaviorRandom::EnemyBehaviorRandom(float speed): speed(speed)
{
    this->direction = Vertex3D(
        (float) Tools::random(-25 ,25),
        (float) Tools::random(-25, 25), 0
    )
    .getNormalize()
    .getScaled(speed)
    ;
}

void EnemyBehaviorRandom::onUpdate(Vertex3D &position)
{
    Object3DBehavior::onUpdate(position);

    if (!isEnabled()) return;

    auto player = ComponentsManager::get()->getComponentGame()->getPlayer();
    if (!Frustum::isVertexInside(position)) {
        Vector3D direction(position, player->getPosition());
        this->direction = direction.getComponent().getNormalize().getScaled(speed);
    }

    position = position + this->direction;
}

float EnemyBehaviorRandom::getSpeed() const {
    return speed;
}

Object3DBehavior *EnemyBehaviorRandom::clone() {
    return new EnemyBehaviorRandom(this->speed);
}

void EnemyBehaviorRandom::setStartPosition(Vertex3D &position) {

}
