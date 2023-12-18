//
// Created by eduardo on 10/05/23.
//

#include "../../include/Shaders/ShaderBilinear.h"
#include "../../include/EngineSetup.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Brakeza3D.h"

ShaderBilinear::ShaderBilinear(bool active)
:
    ShaderOpenCL(active)
{
    setLabel("ShaderBilinear");
}

void ShaderBilinear::preUpdate()
{
}

void ShaderBilinear::postUpdate()
{
    if (!this->enabled) return;
    update();
    executeKernelOpenCL();
}

void ShaderBilinear::update()
{
    Shader::update();
}

void ShaderBilinear::executeKernelOpenCL()
{
}

cJSON *ShaderBilinear::getJSON()
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "type", this->getLabel().c_str());

    return root;
}
