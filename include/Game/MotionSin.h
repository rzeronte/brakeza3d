//
// Created by eduardo on 20/3/22.
//

#ifndef BRAKEDA3D_MOTIONSIN_H
#define BRAKEDA3D_MOTIONSIN_H


#include "Motion.h"

class MotionSin : public Motion {
    float speed;
public:
    MotionSin(float speed);

    void onUpdate(Vertex3D &position) override;

    float getSpeed() const;

    void setSpeed(float speed);
};


#endif //BRAKEDA3D_MOTIONSIN_H
