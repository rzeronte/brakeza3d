//
// Created by eduardo on 15/04/22.
//

#include "../../include/Game/EnemyBehaviorRandom.h"
#include "../../include/Misc/Tools.h"
#include "../../include/ComponentsManager.h"

EnemyBehaviorRandom::EnemyBehaviorRandom(float speed)
{
    this->speed = speed;
    this->direction = Vertex3D(Tools::random(-10, 10), Tools::random(-10, 10), 0);
}

void EnemyBehaviorRandom::onUpdate(Vertex3D &position) {
    EnemyBehavior::onUpdate(position);

    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    auto player = ComponentsManager::get()->getComponentGame()->getPlayer();
    if (!camera->frustum->isVertexInside(position)) {
        Vector3D direction(position, player->getPosition());
        this->direction = direction.getComponent().getNormalize().getScaled(speed);
    }

    position = position + this->direction;
}

float EnemyBehaviorRandom::getSpeed() const {
    return speed;
}

