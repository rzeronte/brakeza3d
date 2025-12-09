#include "../../include/Misc/Logging.h"
#include "../../include/Config.h"
#include "../../include/Brakeza.h"

void Logging::Message(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    OutputVa(message, false, args);
    va_end(args);
}

void Logging::Message(const std::string &message, ...)
{
    va_list args;
    va_start(args, message);
    OutputVa(message.c_str(), false, args);
    va_end(args);
}

void Logging::OutputVa(const char *message, bool forceSTD, va_list args)
{
    if (!Config::get()->ENABLE_LOGGING) return;

    va_list args_copy;
    va_copy(args_copy, args);

    int size = vsnprintf(nullptr, 0, message, args_copy);
    va_end(args_copy);

    char* buffer = new char[size + 1];
    vsnprintf(buffer, size + 1, message, args);

    if (Brakeza::get()->GUI() != nullptr) {
        Brakeza::get()->GUI()->getConsole()->AddLog("%s", buffer);
    }

    if (Config::get()->ENABLE_LOGGING_STD || forceSTD) {
        std::cout << buffer << std::endl;
    }

    delete[] buffer;
}

void Logging::Error(const char *error, ...)
{
    std::string prefixed = std::string("[Error] " + std::string(error));
    va_list args;
    va_start(args, error);
    OutputVa(prefixed.c_str(), true, args);
    va_end(args);
}