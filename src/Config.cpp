/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */


#include "../include/Config.h"

Config *Config::instance = nullptr;

Config *Config::get()
{
    if (instance == nullptr) {
        instance = new Config();
    }

    return instance;
}
