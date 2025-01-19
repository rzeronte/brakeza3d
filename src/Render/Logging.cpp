
#include "../../include/Render/Logging.h"
#include "../../include/EngineSetup.h"
#include "../../include/Brakeza3D.h"

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

    va_list args_copy;
    va_copy(args_copy, args);

    int size = vsnprintf(nullptr, 0, message, args_copy);
    va_end(args_copy);

    char* buffer = new char[size + 1];

    vsnprintf(buffer, size + 1, message, args);

    va_end(args);

    if ( Brakeza3D::get()->getManagerGui() != nullptr) {
        Brakeza3D::get()->getManagerGui()->getConsole()->AddLog("%s", buffer);
    } else {
        std::cout << buffer << std::endl;
    }

    delete[] buffer;    va_start(args, message);
    vfprintf(stdout, message, args);
    std::cout << std::endl;
    va_end (args);
}

void Logging::Message(const char *message, ...)
{
    va_list args;
    va_start(args, message);

    va_list args_copy;
    va_copy(args_copy, args);

    int size = vsnprintf(nullptr, 0, message, args_copy);
    va_end(args_copy);

    char* buffer = new char[size + 1];

    vsnprintf(buffer, size + 1, message, args);

    va_end(args);

    if ( Brakeza3D::get()->getManagerGui() != nullptr) {
        Brakeza3D::get()->getManagerGui()->getConsole()->AddLog("%s", buffer);
    } else {
        std::cout << buffer << std::endl;
    }

    delete[] buffer;
}

void Logging::head(const char *message, ...)
{
    va_list args;
    va_start(args, message);

    va_list args_copy;
    va_copy(args_copy, args);

    int size = vsnprintf(nullptr, 0, message, args_copy);
    va_end(args_copy);

    char* buffer = new char[size + 1];

    vsnprintf(buffer, size + 1, message, args);

    va_end(args);

    if ( Brakeza3D::get()->getManagerGui() != nullptr) {
        Brakeza3D::get()->getManagerGui()->getConsole()->AddLog("#> %s", buffer);
    } else {
        std::cout << buffer << std::endl;
    }

    delete[] buffer;

}

void Logging::Log(float value)
{
    this->Log(std::to_string(value).c_str());
}

