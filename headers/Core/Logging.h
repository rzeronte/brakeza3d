
#ifndef SDL2_3D_ENGINE_LOGGING_H
#define SDL2_3D_ENGINE_LOGGING_H


#include "../GUI/GUI_Log.h"

class Logging {
public:

    static Logging* instance;

    GUI_Log *gui_log;

    Logging();
    ~Logging();

public:

    static Logging* getInstance();

    void setGUILog(GUI_Log *gui_log) {
        Logging::gui_log = gui_log;
    }

    void Log(std::string message, std::string type)
    {
        if (gui_log != NULL) {
            std::string signature = "[%.1f][%s] ";
            gui_log->log.AddLog( (signature + message + "\n").c_str(), ImGui::GetTime(), type.c_str());
        } else {
            printf("Logging error: GUI Log not found\r\n");
        }
    }
};

#endif //SDL2_3D_ENGINE_LOGGING_H
