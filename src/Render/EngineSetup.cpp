
#include <iostream>
#include "../../headers/Render/EngineSetup.h"

EngineSetup* EngineSetup::instance = 0;

EngineSetup* EngineSetup::getInstance()
{
    if (instance == 0) {
        instance = new EngineSetup();
    }

    return instance;
}

EngineSetup::EngineSetup()
{
}

void EngineSetup::setResolution(int w, int h)
{
    EngineSetup::getInstance()->screenWidth = w;
    EngineSetup::getInstance()->screenHeight = h;
}