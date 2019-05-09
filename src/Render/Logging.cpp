
#include "../../headers/Render/Logging.h"

Logging* Logging::instance = 0;

Logging* Logging::getInstance()
{
    if (instance == 0) {
        instance = new Logging();
    }

    return instance;
}

Logging::Logging() {

}

void Logging::setGUILog(GUI_Log *gui_log) {
    Logging::gui_log = gui_log;
}


void Logging::Log(std::string message, std::string type)
{
    if (gui_log != NULL) {
        std::string signature = "[%.1f][%s] ";
        gui_log->log.AddLog( (signature + message + "\n").c_str(), ImGui::GetTime(), type.c_str());
    } else {
        printf("Logging error: GUI Log not found\r\n");
    }
}

void Logging::Log(std::string message)
{
    this->Log(message, "DEBUG");
}