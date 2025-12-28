#include "../../include/Components/Component.h"

#include "../../include/Misc/Logging.h"

Component::Component()
:
    SETUP(Config::get()
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
    LOG_MESSAGE("[%s] Starting...", label.c_str());
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
    LOG_MESSAGE("Component set to %d", (int) value);
    enabled = value;
}
