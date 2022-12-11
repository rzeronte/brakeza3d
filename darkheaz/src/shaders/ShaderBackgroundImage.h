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
    int useOffset;

    float offsetX;
    float offsetY;
public:
    ShaderImage();

    void update() override;

    ~ShaderImage() override;

    void executeKernelOpenCL();

    void setImage(Image *image);

    void resetOffsets();

    bool isUseOffset() const;

    void setUseOffset(bool useOffset);

};


#endif //BRAKEZA3D_SHADERBACKGROUNDIMAGE_H
