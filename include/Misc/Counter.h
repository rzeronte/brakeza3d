
#ifndef BRAKEZA3D_COUNTER_H
#define BRAKEZA3D_COUNTER_H


#include "Timer.h"

class Counter {
private:
    bool enabled = true;
    float step{};

    Timer *timer;
    float lastTicks = 0;
    float acumulatedTime = 0;

    bool finished = false;
public:

    Counter();

    Counter(float step);

    void setStep(float step);

    float getStep() const;

    void update();

    bool isFinished() const;

    float getAcumulatedTime() const;

    bool isEnabled() const;

    void setEnabled(bool enabled);

};


#endif //BRAKEZA3D_COUNTER_H
