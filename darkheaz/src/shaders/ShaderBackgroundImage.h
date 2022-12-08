//
// Created by eduardo on 14/11/22.
//

#ifndef BRAKEZA3D_SHADERBACKGROUNDIMAGE_H
#define BRAKEZA3D_SHADERBACKGROUNDIMAGE_H


#include <CL/cl.h>
#include "../../../include/Misc/Image.h"
#include "../../../include/Render/ShaderOpenCL.h"

class ShaderImage: public ShaderOpenCL {
    Image *image;
    cl_mem opencl_buffer_pixels_image;
public:
    ShaderImage();

    void update() override;

    ~ShaderImage() override;

    void executeKernelOpenCL();

    void setImage(Image *image);

    float offsetX;
    float offsetY;

    void resetOffsets();
};


#endif //BRAKEZA3D_SHADERBACKGROUNDIMAGE_H
