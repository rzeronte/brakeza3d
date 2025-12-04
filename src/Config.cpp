

#include "../include/Config.h"

Config *Config::instance = nullptr;

Config *Config::get()
{
    if (instance == nullptr) {
        instance = new Config();
    }

    return instance;
}
