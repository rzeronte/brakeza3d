
#include "../include/LUAManager.h"
#include "../include/ComponentsManager.h"
#include "LUAIntegration.h"

LUAManager *LUAManager::instance = nullptr;

LUAManager *LUAManager::get()
{
    if (instance == nullptr) {
        instance = new LUAManager();
    }

    return instance;
}

LUAManager::LUAManager()
{
    sharedLuaContext = new SharedLUAContext();
}

sol::state &LUAManager::getLua()
{
    return lua;
}

void LUAManager::initLUATypes()
{
    lua.open_libraries(sol::lib::base, sol::lib::math);

    LUAIntegration(lua);

    lua["brakeza"] = Brakeza3D::get();
    lua["componentsManager"] = ComponentsManager::get();
    lua["context"] = sharedLuaContext;
    lua.set_function("print", &Logging::Message);
}


