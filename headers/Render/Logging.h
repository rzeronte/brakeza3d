
#ifndef SDL2_3D_ENGINE_LOGGING_H
#define SDL2_3D_ENGINE_LOGGING_H

#include <iostream>

class Logging {
public:

    static Logging* instance;

    Logging();
    ~Logging();

public:

    static Logging* getInstance();

    void Log(std::string message, std::string type);
    void Log(std::string message);
};

#endif //SDL2_3D_ENGINE_LOGGING_H
