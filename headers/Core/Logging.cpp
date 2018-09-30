
#include "Logging.h"

Logging* Logging::instance = 0;

Logging* Logging::getInstance()
{
    if (instance == 0) {
        instance = new Logging();
    }

    return instance;
}

Logging::Logging() {

}