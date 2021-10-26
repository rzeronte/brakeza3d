
#include <utility>

#include "../../include/Render/Logging.h"
#include "../../include/EngineSetup.h"

Logging *Logging::instance = nullptr;

Logging *Logging::getInstance() {
    if (instance == nullptr) {
        instance = new Logging();
    }

    return instance;
}

void Logging::Log(const std::string& message, const std::string& type) {
    if (!EngineSetup::getInstance()->LOGGING) return;

    if (EngineSetup::getInstance()->LOGGING_TO_FILE) {
        FILE *f = fopen("brakeza.log", "a");
        if (f == nullptr) {
            std::cout << "Error opening log file!" << std::endl;
            exit(1);
        }
        fprintf(f, "%s\n", message.c_str());
        fclose(f);
    }

    std::cout << '[' << type << ']' << ' ' << message << std::endl;
}

void Logging::Log(const std::string& message) {
    this->Log(message, "DEBUG");
}