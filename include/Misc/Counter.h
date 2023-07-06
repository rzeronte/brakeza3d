
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
