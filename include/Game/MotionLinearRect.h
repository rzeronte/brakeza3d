//
// Created by eduardo on 19/3/22.
//

#ifndef BRAKEDA3D_MOTIONLINEARRECT_H
#define BRAKEDA3D_MOTIONLINEARRECT_H


#include "Motion.h"
#include "../Objects/Vector3D.h"

#define MOTIONLINEARRECT_DEFAULT_SPEED  1000

class MotionLinearRect : public Motion {
    Vector3D direction;
    float speed;

public:

    MotionLinearRect(const Vector3D &direction);

    void onUpdate(Vertex3D &position) override;

    void setDirection(const Vector3D &direction);

    float getSpeed() const;

    void setSpeed(float speed);
};


#endif //BRAKEDA3D_MOTIONLINEARRECT_H
