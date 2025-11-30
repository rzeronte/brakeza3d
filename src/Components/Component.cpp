#include "../../include/Components/Component.h"

#include "../../include/Misc/Logging.h"
#include "../../include/Render/Profiler.h"

Component::Component()
:
    SETUP(BrakezaSetup::get()
)
{
}

Component::~Component()
{
}

void Component::setId(int newId)
{
    id = newId;
}

const std::string &Component::getLabel() const
{
    return label;
}

void Component::setLabel(const std::string &newLabel)
{
    label = newLabel;
}

void Component::onStart()
{
    Logging::Message("[%s] Starting...", label.c_str());
}

void Component::preUpdate()
{
}

void Component::onUpdate()
{
}

void Component::postUpdate()
{
}

bool Component::isEnabled() const
{
    return enabled;
}

void Component::setEnabled(bool value)
{
    enabled = value;
}
