//
// Created by eduardo on 10/05/22.
//

#ifndef BRAKEDA3D_SHADERBACKGROUNDGAME_H
#define BRAKEDA3D_SHADERBACKGROUNDGAME_H


#include <CL/cl.h>
#include "../../../include/Render/Shader.h"
#include "../../../include/Misc/Image.h"
#include "../../../include/Objects/Point2D.h"
#include "../../../include/Objects/Vector2D.h"

class ShaderBackgroundGame: public Shader {
    Image *channel1;
    uint32_t *palette;

    cl_command_queue clCommandQueue;
    cl_device_id clDeviceId;
    cl_int clRet;

    cl_program program;
    cl_kernel kernel;

    cl_mem opencl_buffer_video;
    cl_mem opencl_buffer_palette;

public:
    ShaderBackgroundGame(cl_device_id device_id, cl_context context, cl_command_queue command_queue);

    void update() override;

    ~ShaderBackgroundGame() override;

    float plot(Vector2D st, float thickness);

    float plot(Vector2D st, float pct, float thickness);

    void demo01();

    void demo02();

    void demo03();

    void demo04();

    void makeColorPalette();

    void demoOpenCL();

    void initOpenCLProgram(cl_device_id &device_id, cl_context context);
};


#endif //BRAKEDA3D_SHADERBACKGROUNDGAME_H
