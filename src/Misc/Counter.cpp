
#include "../../headers/Misc/Counter.h"
#include "../../headers/Brakeza3D.h"

Counter::Counter() {
    this->timer = Brakeza3D::get()->getTimer();
}

Counter::Counter(float step) {
    this->timer = Brakeza3D::get()->getTimer();
    this->step = step;
}

void Counter::update() {
    if (!isEnabled()) return;

    auto ticks = (float) this->timer->getTicks();
    float deltatime = ticks - this->lastTicks;
    this->lastTicks = ticks;

    acumulatedTime += (deltatime / 1000);

    if (acumulatedTime >= step) {
        finished = true;
        return;
    }

    finished = false;
}

bool Counter::isFinished() const {
    return finished;
}

void Counter::setStep(float step) {
    this->step = step;
    this->acumulatedTime = 0;
    this->finished = false;
    this->lastTicks = 0;
}

float Counter::getAcumulatedTime() const {
    return acumulatedTime;
}

float Counter::getStep() const {
    return step;
}

bool Counter::isEnabled() const {
    return enabled;
}

void Counter::setEnabled(bool e) {
    Counter::enabled = e;

    this->acumulatedTime = 0;
    this->finished = false;
    this->lastTicks = 0;

    if (enabled) {
        this->lastTicks = timer->getTicks();
    }
}

