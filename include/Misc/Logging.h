
#ifndef SDL2_3D_ENGINE_LOGGING_H
#define SDL2_3D_ENGINE_LOGGING_H

class Logging {

public:
    static void Output(const char *, ...);
    static void Message(const char *, ...);
    static void Error(const char *error, ...);
};

#endif //SDL2_3D_ENGINE_LOGGING_H
