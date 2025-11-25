
#include "../../include/Misc/Logging.h"
#include "../../include/BrakezaSetup.h"
#include "../../include/Brakeza.h"


void Logging::Message(const char *message, ...)
{
    if (!BrakezaSetup::get()->ENABLE_LOGGING) return;

    va_list args;
    va_start(args, message);

    va_list args_copy;
    va_copy(args_copy, args);

    int size = vsnprintf(nullptr, 0, message, args_copy);
    va_end(args_copy);

    char* buffer = new char[size + 1];

    vsnprintf(buffer, size + 1, message, args);

    va_end(args);

    if ( Brakeza::get()->getManagerGui() != nullptr) {
        Brakeza::get()->getManagerGui()->getConsole()->AddLog("%s", buffer);
    }

    std::cout << buffer << std::endl;

    delete[] buffer;
}