
#include "../include/EngineSetup.h"

EngineSetup *EngineSetup::instance = nullptr;

EngineSetup *EngineSetup::get() {
    if (instance == nullptr) {
        instance = new EngineSetup();
    }

    return instance;
}

EngineSetup::EngineSetup() {
}