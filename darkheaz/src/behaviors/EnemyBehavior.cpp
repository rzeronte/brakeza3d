//
// Created by eduardo on 19/3/22.
//

#include "../enemies/EnemyBehavior.h"

EnemyBehavior::EnemyBehavior()
{
    timer.start();
}

void EnemyBehavior::onUpdate(Vertex3D &position)
{
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

EnemyBehavior::~EnemyBehavior() {

}

