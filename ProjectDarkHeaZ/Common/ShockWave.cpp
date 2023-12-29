//
// Created by eduardo on 9/03/23.
//

#include "ShockWave.h"
#include "../../include/Brakeza3D.h"

ShockWave::ShockWave(Vertex3D position, float speed, float ttl, ShockWaveParams params, bool deleteWhenEnds)
:
    params(params),
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

    auto window = ComponentsManager::get()->getComponentWindow();
    auto p = Transforms::WorldToPoint(getPosition());
    window->getShaderOGLShockWave()->render(
        p,
        t,
        speed,
        params,
        window->sceneTexture,
        window->getPostProcessingFramebuffer()
    );
}

void ShockWave::postUpdate()
{
}

float ShockWave::getSpeed() const {
    return speed;
}