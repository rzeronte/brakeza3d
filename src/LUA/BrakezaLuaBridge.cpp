/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#include "../../include/LUA/BrakezaLuaBridge.h"

void LUAIntegration(sol::state &lua)
{
    LUAIntegrationBasics(lua);
    LUAIntegrationComponents(lua);
    LUAIntegrationBrakeza(lua);
    LUAIntegrationMeshes(lua);
    LUAIntegrationLights(lua);
    LUAIntegrationShaders(lua);
    LUAIntegrationTables(lua);
}
