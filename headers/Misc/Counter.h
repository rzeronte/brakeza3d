
#ifndef BRAKEZA3D_COUNTER_H
#define BRAKEZA3D_COUNTER_H


#include "../Render/Timer.h"

class Counter {
private:
    float step;

    Timer *timer;
    float lastTicks = 0;
    float acumulatedTime = 0;

    bool finished;
public:

    Counter();
    Counter(float step);

    void setTimer(Timer *timer);
    void setStep(float step);

    float getStep() const;

    void resetAcumulated();

    void update();
    bool isFinished() const;

    float getAcumulatedTime() const;

};


#endif //BRAKEZA3D_COUNTER_H
