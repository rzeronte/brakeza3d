
#include "../../headers/Render/Logging.h"
#include "../../headers/EngineSetup.h"

Logging *Logging::instance = 0;

Logging *Logging::getInstance() {
    if (instance == 0) {
        instance = new Logging();
    }

    return instance;
}

Logging::Logging() {
}

void Logging::Log(std::string message, std::string type) {
    if (!EngineSetup::getInstance()->LOGGING) return;

    if (EngineSetup::getInstance()->LOGGING_TO_FILE) {
        FILE *f = fopen("brakeza.log", "a");
        if (f == NULL) {
            std::cout << "Error opening log file!" << std::endl;
            exit(1);
        }
        fprintf(f, "%s\n", message.c_str());
        fclose(f);
    };

    std::cout << '[' << type << ']' << ' ' << message << std::endl;
}

void Logging::Log(std::string message) {
    this->Log(message, "DEBUG");
}