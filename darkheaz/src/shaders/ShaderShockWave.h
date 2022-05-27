//
// Created by eduardo on 27/3/22.
//

#ifndef BRAKEDA3D_SHADERSHOCKWAVE_H
#define BRAKEDA3D_SHADERSHOCKWAVE_H


#include <CL/cl.h>
#include "../../../include/Render/Shader.h"
#include "../../../include/Misc/Color.h"
#include "../../../include/Objects/Vertex3D.h"
#include "../../../include/Objects/Point2D.h"
#include "../../../include/Misc/Counter.h"

class ShaderShockWave: public Shader {
    float startSize;
    float currentSize;
    float waveSpeed;
    Counter ttlWave;

    cl_command_queue clCommandQueue;
    cl_device_id clDeviceId;
    cl_int clRet;

    cl_program program;
    cl_kernel kernel;

    cl_mem opencl_buffer_video;
    cl_mem opencl_buffer_videoShader;
public:
    ShaderShockWave(
        cl_device_id device_id,
        cl_context context,
        cl_command_queue command_queue,
        float size,
        float waveSpeed,
        float ttl
    );

    void onUpdate(Vertex3D position);
    void initOpenCLProgram(cl_device_id &device_id, cl_context context);

    void demoOpenCL(Vertex3D position);
};


#endif //BRAKEDA3D_SHADERSHOCKWAVE_H
