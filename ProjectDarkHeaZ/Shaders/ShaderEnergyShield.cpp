//
// Created by eduardo on 26/06/23.
//

#include "ShaderEnergyShield.h"
#include "../../include/Brakeza3D.h"

ShaderEnergyShield::ShaderEnergyShield(
    bool active,
    Mesh3D* parent,
    const std::string &textureFilename,
    const std::string &maskFilename
)
:
        FXEffectOpenGL(active),
        texture(Image(textureFilename)),
        mask(Image(maskFilename)),
        maxAlpha(255),
        parent(parent)
{
}

void ShaderEnergyShield::update()
{
    FXEffectBase::update();

    if (!isEnabled()) return;

    executeKernelOpenCL();
}

void ShaderEnergyShield::executeKernelOpenCL()
{
}

void ShaderEnergyShield::setMaxAlpha(unsigned int maxAlpha) {
    ShaderEnergyShield::maxAlpha = maxAlpha;
}
