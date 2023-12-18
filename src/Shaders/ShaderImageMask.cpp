//
// Created by eduardo on 14/06/23.
//

#include "../../include/Shaders/ShaderImageMask.h"
#include "../../include/EngineSetup.h"
#include "../../include/Brakeza3D.h"

ShaderImageMask::ShaderImageMask(
    bool active,
    const std::string& imageFilename,
    const std::string& maskFilename
) :
    ShaderOpenCL(active),
    image(Image(imageFilename)),
    mask(Image(maskFilename)),
    maxAlpha(255)
{
}

void ShaderImageMask::update()
{
    Shader::update();

    if (!isEnabled()) return;

    executeKernelOpenCL();
}

void ShaderImageMask::executeKernelOpenCL()
{

}

void ShaderImageMask::setMaxAlpha(unsigned int maxAlpha) {
    ShaderImageMask::maxAlpha = maxAlpha;
}
