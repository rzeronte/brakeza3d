
#ifndef SDL2_3D_ENGINE_LOGGING_H
#define SDL2_3D_ENGINE_LOGGING_H

#include <iostream>

class Logging {
public:

    static Logging *instance;

public:

    static Logging *getInstance();

    static void Log(const std::string& message, const std::string& type);

    void Log(std::string message);
};

#endif //SDL2_3D_ENGINE_LOGGING_H
