
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

    void setTimer(Timer *timer);
    void setStep(float step);

    void resetAcumulated();

    void update();
    bool isFinished() const;

};


#endif //BRAKEZA3D_COUNTER_H
