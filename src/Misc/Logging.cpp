#include "../../include/Misc/Logging.h"
#include "../../include/Config.h"
#include "../../include/Brakeza.h"

void Logging::Message(const char *message, ...)
{
    if (Brakeza::get()->GUI() != nullptr) {
        Brakeza::get()->GUI()->getConsole()->setLogIcon(IconGUI::ADD);
    }

    va_list args;
    va_start(args, message);
    OutputVa(message, args);
    va_end(args);
}

void Logging::OutputVa(const char *message, va_list args)
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

    //if (Config::get()->ENABLE_LOGGING_STD) {
        std::cout << buffer << std::endl;
    //}

    delete[] buffer;
}

void Logging::Output(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    OutputVa(message, args);
    va_end(args);
}

void Logging::Error(const char *error, ...)
{
    if (Brakeza::get()->GUI() != nullptr) {
        Brakeza::get()->GUI()->getConsole()->setLogIcon(IconGUI::EXIT);
    }
    
    va_list args;
    va_start(args, error);
    OutputVa(error, args);
    va_end(args);
}