//
// Created by eduardo on 26/06/23.
//

#include "FXEnergyShield.h"
#include "../../include/Brakeza3D.h"

FXEnergyShield::FXEnergyShield(
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

void FXEnergyShield::update()
{
    FXEffectBase::update();

    if (!isEnabled()) return;

    executeKernelOpenCL();
}

void FXEnergyShield::executeKernelOpenCL()
{
}

void FXEnergyShield::setMaxAlpha(unsigned int maxAlpha) {
    FXEnergyShield::maxAlpha = maxAlpha;
}
