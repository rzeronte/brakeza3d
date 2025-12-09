
#ifndef SDL2_3D_ENGINE_LOGGING_H
#define SDL2_3D_ENGINE_LOGGING_H
#include <cstdarg>
#include <string>

class Logging {

public:
    static void OutputVa(const char *message, bool forceSTD, va_list args);
    static void Message(const char *, ...);
    static void Message(const std::string &message, ...);
    static void Error(const char *error, ...);
};

#endif //SDL2_3D_ENGINE_LOGGING_H
