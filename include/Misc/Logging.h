
#ifndef SDL2_3D_ENGINE_LOGGING_H
#define SDL2_3D_ENGINE_LOGGING_H

#include <string>

#ifdef ENABLE_LOGGING_BUILD
    #define LOG_MESSAGE(fmt, ...) Logging::Message(fmt, ##__VA_ARGS__)
    #define LOG_ERROR(fmt, ...) Logging::Error(fmt, ##__VA_ARGS__)
    #define LOG_WARNING(fmt, ...) Logging::Warning(fmt, ##__VA_ARGS__)
    #define LOG_SUCCESS(fmt, ...) Logging::Success(fmt, ##__VA_ARGS__)
    // Solo si Config::ENABLE_LOGGING_VERBOSE (menú Logging > "Verbose (per-object)").
    #define LOG_VERBOSE(fmt, ...) do { if (Logging::IsVerbose()) Logging::Message(fmt, ##__VA_ARGS__); } while (0)
#else
    #define LOG_MESSAGE(fmt, ...) ((void)0)
    #define LOG_ERROR(fmt, ...) ((void)0)
    #define LOG_WARNING(fmt, ...) ((void)0)
    #define LOG_SUCCESS(fmt, ...) ((void)0)
    #define LOG_VERBOSE(fmt, ...) ((void)0)
#endif

class Logging {

public:
    static void OutputVa(const char *message, bool forceSTD, va_list args);
    static void Message(const char *, ...);
    static void Message(const std::string &message, ...);
    static void Error(const char *error, ...);
    static void Warning(const char *warning, ...);
    static void Success(const char *message, ...);
    static bool IsVerbose();
};

#endif //SDL2_3D_ENGINE_LOGGING_H
