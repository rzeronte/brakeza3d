#include "ShaderCRT.h"
#include "../../include/EngineSetup.h"
#include "../../include/Brakeza3D.h"

ShaderCRT::ShaderCRT(bool active, const std::string& imageFilename, const std::string& maskFilename)
:
    ShaderOpenCL(active),
    image(Image(imageFilename)),
    mask(Image(maskFilename)),
    maxAlpha(255)
{
}

void ShaderCRT::update()
{
    Shader::update();

    if (!isEnabled()) return;

    executeKernelOpenCL();
}

void ShaderCRT::executeKernelOpenCL()
{

}

void ShaderCRT::setMaxAlpha(unsigned int maxAlpha) {
    ShaderCRT::maxAlpha = maxAlpha;
}

void ShaderCRT::setImage(const std::string& filename)
{
    image.setImage(filename);
}

void ShaderCRT::setMask(const std::string& filename)
{
    mask.setImage(filename);
}
