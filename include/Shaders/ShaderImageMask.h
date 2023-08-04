//
// Created by eduardo on 14/06/23.
//

#ifndef BRAKEZA3D_SHADERIMAGEMASK_H
#define BRAKEZA3D_SHADERIMAGEMASK_H


#include "../Render/ShaderOpenCL.h"
#include "../Misc/Image.h"

class ShaderImageMask : public ShaderOpenCL {
    Image image;
    Image mask;
    cl_mem clBufferImage;
    cl_mem clBufferMask;
    unsigned int maxAlpha;

public:
    ShaderImageMask(bool active, const std::string& imageFilename, const std::string& maskFilename);

    void executeKernelOpenCL();

    void update() override;

    void setMaxAlpha(unsigned int maxAlpha);
};


#endif //BRAKEZA3D_SHADERIMAGEMASK_H
