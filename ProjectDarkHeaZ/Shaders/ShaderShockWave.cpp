
#include "ShaderShockWave.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Transforms.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ShaderShockWave::ShaderShockWave(bool active)
:
    ShaderOpenCL(active)
{
}

void ShaderShockWave::update()
{
    Shader::update();

    if (!isEnabled()) return;

    this->executeKernelOpenCL();
}

void ShaderShockWave::executeKernelOpenCL()
{
}

void ShaderShockWave::addShockWave(ShockWave *wave)
{
    Point2D focalPoint = Transforms::WorldToPoint(wave->getPosition());

    this->waves.push_back(OCShockWave{
        wave->getTtlWave().getAcumulatedTime(),
        wave->getSpeed(),
        focalPoint.x,
        focalPoint.y,
        wave->getCurrentSize()
    });
}
