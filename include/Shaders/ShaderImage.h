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
    cl_mem clBufferImage;
    int useOffset;
    int useColors;

    float offsetX;
    float offsetY;
public:
    explicit ShaderImage(const std::string& filename);

    ~ShaderImage() override;

    void update(float increaseOffsetX, float increaseOffsetY);

    void executeKernelOpenCL(float increaseOffsetX, float increaseOffsetY);

    void setImage(const std::string& fileName);

    void resetOffsets();

    void setUseOffset(bool value);

    void refreshBufferImage();

    [[nodiscard]] bool isUseOffset() const;

    void limitOffset();

    void setUseColors(int useColors);
};


#endif //BRAKEZA3D_SHADERIMAGE_H
