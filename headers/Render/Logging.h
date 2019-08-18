
#ifndef SDL2_3D_ENGINE_LOGGING_H
#define SDL2_3D_ENGINE_LOGGING_H


#include "../../headers/GUI/GUI_Log.h"

class Logging {
public:

    static Logging* instance;

    GUI_Log *gui_log;

    Logging();
    ~Logging();

public:

    static Logging* getInstance();

    void setGUILog(GUI_Log *gui_log);
    void Log(std::string message, std::string type);
    void Log(std::string message);
};

#endif //SDL2_3D_ENGINE_LOGGING_H
