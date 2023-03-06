//
// Created by eduardo on 27/3/22.
//

#ifndef BRAKEDA3D_SHADERSHOCKWAVE_H
#define BRAKEDA3D_SHADERSHOCKWAVE_H


#include <CL/cl.h>
#include "../../../include/Misc/Color.h"
#include "../../../include/Objects/Vertex3D.h"
#include "../../../include/Objects/Point2D.h"
#include "../../../include/Misc/Counter.h"
#include "../../../include/Render/ShaderOpenCL.h"

class ShaderShockWave: public ShaderOpenCL {
    float startSize;
    float currentSize;
    float waveSpeed;
    Counter ttlWave;
    Vertex3D position;

public:
    ShaderShockWave(bool active, Vertex3D position, float size, float speed, float ttl);

    void onUpdate();

    void reset();

    void executeKernelOpenCL();

    void setPosition(const Vertex3D &position);
};


#endif //BRAKEDA3D_SHADERSHOCKWAVE_H
