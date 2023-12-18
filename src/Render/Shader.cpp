#include <functional>
#include "../../include/Render/Shader.h"
#include "../../include/EngineSetup.h"

Shader::Shader(bool enabled)
:
    enabled(enabled),
    w(EngineSetup::get()->screenHeight),
    h(EngineSetup::get()->screenWidth),
    resolution(Vector2D((float) EngineSetup::get()->screenWidth, (float) EngineSetup::get()->screenHeight)),
    deltaTime(0),
    last_ticks(0),
    current_ticks(0),
    executionTime(0)
{
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

float Shader::step(float limit, float value)
{
    if (value < limit ) {
        return 0.0f;
    }

    return 1.0f;
}
