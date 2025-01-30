
#ifndef SDL2_3D_ENGINE_LOGGING_H
#define SDL2_3D_ENGINE_LOGGING_H

#include <iostream>

class Logging {
public:

    static Logging *instance;

public:

    static Logging *getInstance();

    static void Log(const char *, ...);

    static void Message(const char *, ...);

    static void Error(const std::string& error, ...);

    void Log(float value);

};

#endif //SDL2_3D_ENGINE_LOGGING_H
