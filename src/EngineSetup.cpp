
#include <iostream>
#include "../headers/EngineSetup.h"

EngineSetup* EngineSetup::instance = 0;

EngineSetup* EngineSetup::getInstance()
{
    if (instance == 0) {
        instance = new EngineSetup();
    }

    return instance;
}

EngineSetup::EngineSetup() {

}