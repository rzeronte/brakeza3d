//
// Created by eduardo on 10/05/22.
//

#ifndef BRAKEDA3D_SHADERBACKGROUNDGAME_H
#define BRAKEDA3D_SHADERBACKGROUNDGAME_H


#include <CL/cl.h>
#include "../../../include/Misc/Image.h"
#include "../../../include/Objects/Point2D.h"
#include "../../../include/Objects/Vector2D.h"
#include "../../../include/Render/ShaderOpenCL.h"

class ShaderBackgroundGame: public ShaderOpenCL {
    Image *channel1;
    uint32_t *palette;

    cl_mem opencl_buffer_palette;

public:
    ShaderBackgroundGame(
        cl_device_id deviceId,
        cl_context context,
        cl_command_queue commandQueue,
        const std::string& kernelFile
    );

    void update() override;

    ~ShaderBackgroundGame() override;

    float plot(Vector2D st, float thickness);

    float plot(Vector2D st, float pct, float thickness);

    void demo01();

    void demo02();

    void demo03();

    void demo04();

    void makeColorPalette();

    void executeKernelOpenCL();
};


#endif //BRAKEDA3D_SHADERBACKGROUNDGAME_H
