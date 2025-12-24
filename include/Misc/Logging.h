
#ifndef SDL2_3D_ENGINE_LOGGING_H
#define SDL2_3D_ENGINE_LOGGING_H

#include <string>

#ifdef ENABLE_LOGGING_BUILD
    #define LOG_MESSAGE(fmt, ...) Logging::Message(fmt, ##__VA_ARGS__)
    #define LOG_ERROR(fmt, ...) Logging::Error(fmt, ##__VA_ARGS__)
#else
    #define LOG_MESSAGE(fmt, ...) ((void)0)
    #define LOG_ERROR(fmt, ...) ((void)0)
#endif

class Logging {

public:
    static void OutputVa(const char *message, bool forceSTD, va_list args);
    static void Message(const char *, ...);
    static void Message(const std::string &message, ...);
    static void Error(const char *error, ...);
};

#endif //SDL2_3D_ENGINE_LOGGING_H
