#include "../../include/Components/Component.h"

Component::Component()
:
    id(-1),
    enabled(false),
    SETUP(BrakezaSetup::get()
)
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

bool Component::isEnabled() const
{
    return enabled;
}

void Component::setEnabled(bool value)
{
    enabled = value;
}

Component::~Component()
{
}
