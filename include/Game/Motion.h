//
// Created by eduardo on 19/3/22.
//

#ifndef BRAKEDA3D_MOTION_H
#define BRAKEDA3D_MOTION_H


#include "../Misc/Timer.h"
#include "../Objects/Vertex3D.h"

class Motion {
    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;
    Timer timer;
public:
    Motion();

    virtual void onUpdate(Vertex3D &position);

    float getExecutionTime() const;
};



#endif //BRAKEDA3D_MOTION_H
