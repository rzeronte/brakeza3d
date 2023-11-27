#include "../../include/Components/Component.h"

Component::Component(bool core)
:
    id(-1),
    enabled(false),
    BUFFERS(EngineBuffers::get()),
    SETUP(EngineSetup::get()),
    core(core)
{
}

void Component::setId(int newId) {
    Component::id = newId;
}

const std::string &Component::getLabel() const {
    return label;
}

void Component::setLabel(const std::string &newLabel) {
    Component::label = newLabel;
}

bool Component::isEnabled() const {
    return enabled;
}

void Component::setEnabled(bool enabled) {
    Component::enabled = enabled;
}

bool Component::isCore() const {
    return core;
}
