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
