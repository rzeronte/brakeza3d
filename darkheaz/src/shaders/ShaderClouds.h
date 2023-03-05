//
// Created by eduardo on 10/05/22.
//

#ifndef BRAKEDA3D_ShaderClouds_H
#define BRAKEDA3D_ShaderClouds_H


#include <CL/cl.h>
#include "../../../include/Misc/Image.h"
#include "../../../include/Objects/Point2D.h"
#include "../../../include/Objects/Vector2D.h"
#include "../../../include/Render/ShaderOpenCL.h"

class ShaderClouds: public ShaderOpenCL {
private:
    Image clouds;
    cl_mem opencl_buffer_pixels_image;
    Color color;
public:
    explicit ShaderClouds(bool active, Color c);

    void update() override;

    void executeKernelOpenCL();

    void setColor(const Color &v);
};


#endif //BRAKEDA3D_ShaderClouds_H
