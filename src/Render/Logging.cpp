
#include "../../headers/Render/Logging.h"

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

void Logging::Log(std::string message, std::string type)
{
    std::cout << '[' << type << ']' << ' ' << message << std::endl;
}

void Logging::Log(std::string message)
{
    this->Log(message, "DEBUG");
}