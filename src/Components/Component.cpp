#include "../../include/Components/Component.h"

Component::Component()
:
    id(-1),
    enabled(false),
    BUFFERS(EngineBuffers::get()),
    SETUP(EngineSetup::get()
)
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

Component::~Component() {

}
