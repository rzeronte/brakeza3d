//
// Created by eduardo on 26/06/23.
//

#ifndef BRAKEZA3D_SHADERENERGYSHIELD_H
#define BRAKEZA3D_SHADERENERGYSHIELD_H


#include "../../../include/Misc/Image.h"
#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Objects/Mesh3D.h"

class ShaderEnergyShield : public ShaderOpenCL{
    Image image;
    Image mask;
    cl_mem clBufferMask;
    unsigned int maxAlpha;
    Mesh3D *parent;
public:
    ShaderEnergyShield(bool active, Mesh3D* paren, const std::string &maskFilename);

    void executeKernelOpenCL();

    void update() override;

    void setMaxAlpha(unsigned int maxAlpha);
};


#endif //BRAKEZA3D_SHADERENERGYSHIELD_H
