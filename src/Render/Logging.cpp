
#include <utility>

#include "../../include/Render/Logging.h"
#include "../../include/EngineSetup.h"

Logging *Logging::instance = nullptr;

Logging *Logging::getInstance()
{
    if (instance == nullptr) {
        instance = new Logging();
    }

    return instance;
}

void Logging::Log(const char *message, ...)
{
    if (!EngineSetup::get()->LOGGING) return;

    va_list args;
    va_start(args, message);
    vfprintf(stdout, message, args);

    std::cout << std::endl;

    va_end (args);
}

void Logging::Message(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    vfprintf(stdout, message, args);

    std::cout << std::endl;

    va_end (args);
}

void Logging::Log(float value)
{
    this->Log(std::to_string(value).c_str());
}

