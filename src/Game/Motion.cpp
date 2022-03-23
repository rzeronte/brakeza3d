//
// Created by eduardo on 19/3/22.
//

#include "../../include/Game/Motion.h"

Motion::Motion() {
    timer.start();
}

void Motion::onUpdate(Vertex3D &position) {
    current_ticks = (float) timer.getTicks();
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;
    executionTime += deltaTime / 1000.f;
}

float Motion::getExecutionTime() const {
    return executionTime;
}
