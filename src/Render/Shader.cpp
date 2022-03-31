//
// Created by eduardo on 13/2/22.
//

#include <cstdint>
#include "../../include/Render/Shader.h"
#include "../../include/EngineSetup.h"

Shader::Shader()
{
    setEnabled(false);
    setPhaseRender(EngineSetup::ShadersPhaseRender::PREUPDATE);

    h = EngineSetup::get()->screenHeight;
    w = EngineSetup::get()->screenWidth;

    videoBuffer = new uint32_t[w*h];
    executionTime = 0;
    t.start();
}

void Shader::update()
{
    current_ticks = (float) this->t.getTicks();
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;

    executionTime += deltaTime / 1000.f;
}

bool Shader::isEnabled() const {
    return enabled;
}

void Shader::setEnabled(bool enabled) {
    Shader::enabled = enabled;
}

const std::string &Shader::getLabel() const {
    return label;
}

void Shader::setLabel(const std::string &label) {
    Shader::label = label;
}

void Shader::setPhaseRender(int type) {
    Shader::phaseRender = type;
}

int Shader::getPhaseRender() const {
    return phaseRender;
}

