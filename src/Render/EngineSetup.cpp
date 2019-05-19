
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

EngineSetup::EngineSetup() {
    this->engineTimer.start();
    this->current_ticks = 0;
    this->timerCurrent = 0;
}