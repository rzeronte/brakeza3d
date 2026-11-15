/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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
    enabled = value;
}
