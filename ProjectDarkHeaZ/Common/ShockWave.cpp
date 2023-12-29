//
// Created by eduardo on 9/03/23.
//

#include "ShockWave.h"
#include "../../include/Brakeza3D.h"

ShockWave::ShockWave(Vertex3D position, float speed, float ttl, bool deleteWhenEnds)
:
    ttl(ttl),
    speed(speed),
    deleteWhenEnds(deleteWhenEnds)
{
    setPosition(position);
}

void ShockWave::onUpdate()
{
    auto t = getTimer().getTotalTime();

    if (t > ttl) {
        if (deleteWhenEnds) {
            setRemoved(true);
        }
    }

    auto p = Transforms::WorldToPoint(getPosition());
    ComponentsManager::get()->getComponentWindow()->getShaderOGLShockWave()->setPoint(p);
    ComponentsManager::get()->getComponentWindow()->getShaderOGLShockWave()->setTimeLive(t);

    ComponentsManager::get()->getComponentWindow()->getShaderOGLShockWave()->render(
        ComponentsManager::get()->getComponentWindow()->sceneTexture,
        ComponentsManager::get()->getComponentWindow()->getPostProcessingFramebuffer()
    );
}

void ShockWave::postUpdate()
{
}

float ShockWave::getSpeed() const {
    return speed;
}