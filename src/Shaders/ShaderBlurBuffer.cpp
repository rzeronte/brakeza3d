//
// Created by eduardo on 24/08/23.
//

#include "ShaderParticlesBlurBuffer.h"
#include "../../include/EngineSetup.h"
#include "../../include/EngineBuffers.h"
#include "../../include/ComponentsManager.h"

ShaderParticlesBlurBuffer::ShaderParticlesBlurBuffer(bool active, int blurSize)
:
    ShaderOpenCL(active, "blurBuffer.cl"),
    blurSize(blurSize)
{
    setLabel("ShaderParticlesBlurBuffer");
}

void ShaderParticlesBlurBuffer::preUpdate()
{
}

void ShaderParticlesBlurBuffer::postUpdate()
{
    if (!this->enabled) return;
    update();
    executeKernelOpenCL();
}

void ShaderParticlesBlurBuffer::update()
{
    Shader::update();
}

void ShaderParticlesBlurBuffer::executeKernelOpenCL()
{
}

cJSON *ShaderParticlesBlurBuffer::getJSON()
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "type", this->getLabel().c_str());

    return root;
}
