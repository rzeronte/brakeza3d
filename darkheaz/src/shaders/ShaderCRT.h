//
// Created by eduardo on 20/06/23.
//

#ifndef BRAKEZA3D_SHADERCRT_H
#define BRAKEZA3D_SHADERCRT_H


#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Misc/Image.h"

class ShaderCRT : public ShaderOpenCL  {
    Image image;
    Image mask;
    cl_mem clBufferImage;
    cl_mem clBufferMask;
    unsigned int maxAlpha;

public:
    ShaderCRT(bool active, const std::string& imageFilename, const std::string& maskFilename);

    void update() override;

    void setMaxAlpha(unsigned int maxAlpha);

private:
    void executeKernelOpenCL();

public:
    void setImage(const std::string& filename);

    void setMask(const std::string& filename);
};


#endif //BRAKEZA3D_SHADERCRT_H
