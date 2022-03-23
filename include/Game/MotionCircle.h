//
// Created by eduardo on 19/3/22.
//

#ifndef BRAKEDA3D_MOTIONCIRCLE_H
#define BRAKEDA3D_MOTIONCIRCLE_H


#include "Motion.h"
#include "../Render/M3.h"

class MotionCircle: public Motion {
    float speed;
    Vertex3D center;
    float radius;
public:
    MotionCircle(const Vertex3D &center, float speed, float radius);

    void onUpdate(Vertex3D &position) override;

};


#endif //BRAKEDA3D_MOTIONCIRCLE_H
