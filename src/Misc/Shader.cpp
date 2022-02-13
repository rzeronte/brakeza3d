//
// Created by eduardo on 13/2/22.
//

#include <cstdint>
#include "../../include/Shaders/Shader.h"
#include "../../include/EngineSetup.h"

Shader::Shader()
{
    h = EngineSetup::get()->screenHeight;
    w = EngineSetup::get()->screenWidth;

    videoBuffer = new uint32_t[w*h];
    executionTime = 0;
    t.start();
}

void Shader::onUpdate()
{
    current_ticks = (float) this->t.getTicks();
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;

    executionTime += deltaTime / 1000.f;
}
