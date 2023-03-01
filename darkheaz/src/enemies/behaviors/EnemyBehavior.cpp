//
// Created by eduardo on 19/3/22.
//

#include "../EnemyBehavior.h"
#include "../include/Render/Logging.h"

EnemyBehavior::EnemyBehavior(): enabled(true)
{
    timer.start();
}

void EnemyBehavior::onUpdate(Vertex3D &position)
{
    if (!isEnabled()) return;

    current_ticks = (float) timer.getTicks();
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;
    executionTime += deltaTime / 1000.f;
}

float EnemyBehavior::getExecutionTime() const
{
    return executionTime;
}

void EnemyBehavior::resetTimer()
{
    timer.stop();
    timer.start();
}

bool EnemyBehavior::isEnabled() const {
    return enabled;
}

void EnemyBehavior::setEnabled(bool value) {
    EnemyBehavior::enabled = value;
    if (!value) {
        timer.pause();
    } else {
        timer.unpause();
    }
}

