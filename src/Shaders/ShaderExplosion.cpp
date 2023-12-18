//
// Created by eduardo on 22/05/23.
//

#include "../../include/Shaders/ShaderExplosion.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Brakeza3D.h"



ShaderExplosion::ShaderExplosion(bool active, Color from, Color to, Point2D origin, float emissionTime, OCParticlesContext oclContext)
:
    ShaderOpenCL(active),
    origin(origin),
    particlesContext(oclContext),
    intensity(1.0f),
    emissionTime(emissionTime)
{

}

void ShaderExplosion::update()
{
    Shader::update();
    executeKernelOpenCL();
}

void ShaderExplosion::executeKernelOpenCL()
{
}


void ShaderExplosion::resetContext()
{

}

void ShaderExplosion::setIntensity(float intensity) {
    ShaderExplosion::intensity = intensity;
}
