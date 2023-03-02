//
// Created by eduardo on 13/2/22.
//

#include <cstdint>
#include <functional>
#include "../../include/Render/Shader.h"
#include "../../include/EngineSetup.h"
#include "../../include/EngineBuffers.h"

Shader::Shader()
:
    videoBuffer(new uint32_t[EngineSetup::get()->screenWidth * EngineSetup::get()->screenHeight]),
    enabled(false),
    w(EngineSetup::get()->screenHeight),
    h(EngineSetup::get()->screenWidth),
    bufferSize(EngineSetup::get()->screenWidth * EngineSetup::get()->screenHeight),
    resolution(Vector2D((float) EngineSetup::get()->screenWidth, (float) EngineSetup::get()->screenHeight)),
    phaseRender(EngineSetup::ShadersPhaseRender::PREUPDATE),
    deltaTime(0),
    last_ticks(0),
    current_ticks(0),
    executionTime(0)
{
    std::fill(videoBuffer, videoBuffer + bufferSize, 0);

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

Shader::~Shader()
{
    delete[] videoBuffer;
}

float Shader::fract(float x)
{
    return x - floor(x);
}

float Shader::mix(float a, float b, float f)
{
    return (a * (1.0 - f)) + (b * f);
}

Vertex3D Shader::mix(Vertex3D a, Vertex3D b, float f)
{
    return a.getScaled(1.0f - f) + b.getScaled(f);
}

Color Shader::mix(Color &a, Color &b, float f) {
    return a * (1.0f - f) + b * f;
}

float Shader::smoothStep(float edge0, float edge1, float x)
{
    x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3 - 2 * x);
}

float Shader::step(float limit, float value)
{
    if (value < limit ) {
        return 0.0f;
    }

    return 1.0f;
}

void Shader::flipToVideo()
{
    auto engineBuffers = EngineBuffers::getInstance();

    auto screenBuffer = engineBuffers->videoBuffer;
    auto currentBuffer = this->videoBuffer;

    for (int i = 0; i < bufferSize; i++, ++screenBuffer, ++currentBuffer) {
        *screenBuffer = *currentBuffer;
    }
}

