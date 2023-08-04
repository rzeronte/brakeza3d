//
// Created by eduardo on 27/3/22.
//

#ifndef BRAKEDA3D_SHADERSHOCKWAVE_H
#define BRAKEDA3D_SHADERSHOCKWAVE_H


#include <CL/cl.h>
#include "../../include/Misc/Color.h"
#include "../../include/Objects/Vertex3D.h"
#include "../../include/Objects/Point2D.h"
#include "../../include/Misc/Counter.h"
#include "../../include/Render/ShaderOpenCL.h"
#include "../../include/Shaders/ShockWave.h"

#define MAX_SHOCK_WAVES 5

struct OCShockWave {
    float iTime;
    float speed;
    int focalPointX;
    int focalPointY;
    float currentSize;
};

class ShaderShockWave: public ShaderOpenCL {
    std::vector<OCShockWave> waves;
    cl_mem opencl_buffer_waves;
public:
    explicit ShaderShockWave(bool active);

    ~ShaderShockWave() override;

    void update() override;

    void executeKernelOpenCL();

    void addShockWave(ShockWave *wave);
};


#endif //BRAKEDA3D_SHADERSHOCKWAVE_H
