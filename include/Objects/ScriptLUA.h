//
// Created by eduardo on 3/10/23.
//

#ifndef BRAKEZA3D_SCRIPTLUA_H
#define BRAKEZA3D_SCRIPTLUA_H


#include <cstdio>
#include <string>
#include <utility>
#include "../EngineSetup.h"
#include "../../sol/sol.hpp"

#define MAX_SOURCE_SIZE (0x100000)

struct ScriptLUATypeData {
    ScriptLUATypeData(const char *name, const char *value) : name(name), value(value) {
    }

    std::string name;
    std::string value;
};

class ScriptLUA {
public:
    explicit ScriptLUA(const std::string& script, std::string properties);

    char *content;
    std::vector<ScriptLUATypeData> dataTypes;
public:
    std::string scriptFilename;
    std::string fileTypes;

    char *readFile(const std::string &name, size_t &source_size);

    void run(sol::environment&);

    void addDataType(const char *name, const char* value);

    void parseTypes();

    static std::string dataTypesFileFor(std::string basicString);
    static std::string removeFilenameExtension(std::string& filename);

    void removeDataType(ScriptLUATypeData data);

    void updateFileTypes();

    [[nodiscard]] bool updateFileTypesWith(const std::string &content) const;

    void reloadEnvironment(sol::environment &environment);

    void reloadScriptCode();

    void getCode(const std::string &script);

    bool updateScriptCodeWith(const std::string &content) const;

    void reloadGlobals();
};


#endif //BRAKEZA3D_SCRIPTLUA_H
