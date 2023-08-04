//
// Created by eduardo on 19/3/22.
//

#include "../../include/Behaviors/Object3DBehavior.h"
#include "../../include/Render/Logging.h"

Object3DBehavior::Object3DBehavior(): enabled(true)
{
    timer.start();
}

void Object3DBehavior::onUpdate(Vertex3D &position)
{
    if (!isEnabled()) return;

    current_ticks = (float) timer.getTicks();
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;
    executionTime += deltaTime / 1000.f;
}

float Object3DBehavior::getExecutionTime() const
{
    return executionTime;
}

void Object3DBehavior::resetTimer()
{
    timer.stop();
    timer.start();
}

bool Object3DBehavior::isEnabled() const {
    return enabled;
}

void Object3DBehavior::setEnabled(bool value) {
    Object3DBehavior::enabled = value;
    if (!value) {
        timer.pause();
    } else {
        timer.unpause();
    }
}

