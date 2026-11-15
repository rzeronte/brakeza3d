/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */

#include "../../include/Misc/Counter.h"
#include "../../include/Brakeza.h"

Counter::Counter()
:
    enabled(true),
    step(0),
    timer(Brakeza::get()->getTimer()),
    finished(false)
{
}

Counter::Counter(float step)
:
    enabled(true),
    step(step),
    timer(Brakeza::get()->getTimer()),
    finished(false)
{
}

void Counter::update()
{
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

bool Counter::isFinished() const
{
    if (step <= 0) {
        return false;
    }

    return finished;
}

void Counter::setStep(float value)
{
    this->step = value;
    this->acumulatedTime = 0;
    this->finished = false;
    this->lastTicks = 0;
}

float Counter::getAcumulatedTime() const {
    return acumulatedTime;
}

float Counter::getStep() const
{
    return step;
}

bool Counter::isEnabled() const
{
    return enabled;
}

void Counter::setEnabled(bool e)
{
    enabled = e;

    this->acumulatedTime = 0;
    this->finished = false;
    this->lastTicks = 0;

    if (enabled) {
        this->lastTicks = timer->getTicks();
    }
}

float Counter::currentPercentage()
{
    return getAcumulatedTime() * 100 / getStep();
}

