//
// Created by eduardo on 19/11/22.
//

#include "../../include/Shaders/ShaderColor.h"
#include "../../include/EngineSetup.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"

ShaderColor::ShaderColor(bool active, Color color, float progress)
:
    ShaderOpenCL(active),
    color(color),
    progress(progress)
{
}

void ShaderColor::update()
{
    Shader::update();

    if(!isEnabled()) return;

    executeKernelOpenCL();
}

void ShaderColor::executeKernelOpenCL()
{
}

void ShaderColor::setProgress(float value) {
    ShaderColor::progress = value;
}