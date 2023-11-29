//
// Created by eduardo on 14/11/22.
//

#ifndef BRAKEZA3D_SHADERIMAGE_H
#define BRAKEZA3D_SHADERIMAGE_H


#include <CL/cl.h>
#include "../Misc/Image.h"
#include "../Render/ShaderOpenCL.h"

class ShaderImage: public ShaderOpenCL {
    Image image;
    int useOffset;

    float offsetX;
    float offsetY;
public:
    explicit ShaderImage(const std::string& filename);

    void update(float increaseOffsetX, float increaseOffsetY);

    void executeKernelOpenCL(float increaseOffsetX, float increaseOffsetY);

    void resetOffsets();

    void setUseOffset(bool value);

    [[nodiscard]] bool isUseOffset() const;

    void limitOffset();

    Image &getImage();
};


#endif //BRAKEZA3D_SHADERIMAGE_H
