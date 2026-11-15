/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */

#ifndef BRAKEZA3D_COUNTER_H
#define BRAKEZA3D_COUNTER_H


#include "Timer.h"

class Counter {
private:
    bool enabled;
    float step;

    Timer *timer;
    float lastTicks = 0;
    float acumulatedTime = 0;

    bool finished;
public:

    Counter();

    Counter(float step);

    void setStep(float step);

    [[nodiscard]] float getStep() const;

    void update();

    [[nodiscard]] bool isFinished() const;

    [[nodiscard]] float getAcumulatedTime() const;

    [[nodiscard]] bool isEnabled() const;

    void setEnabled(bool enabled);

    float currentPercentage();

};


#endif //BRAKEZA3D_COUNTER_H
