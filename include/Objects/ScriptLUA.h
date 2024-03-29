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
#include "../Misc/cJSON.h"

#define MAX_SOURCE_SIZE (0x100000)

enum class LUADataType {
    INT,
    FLOAT,
    VERTEX3D,
    STRING
};

typedef std::variant<int, float, Vertex3D, const char*> LUADataValue;

struct ScriptLUATypeData {
    ScriptLUATypeData(const char *name, const char *type, LUADataValue value)
    :name(name), type(type), value(value)
    {}
    std::string name;
    std::string type;
    LUADataValue value;
};

class ScriptLUA {
private:
    bool paused;
    std::map<std::string, LUADataType> LUADataTypesMapping = {
            {"int", LUADataType::INT},
            {"float", LUADataType::FLOAT},
            {"Vertex3D", LUADataType::VERTEX3D}
    };
public:
    explicit ScriptLUA(const std::string& script, std::string properties);

    ScriptLUA(const std::string &scriptFilename, cJSON *types);

    std::string content;
    std::vector<ScriptLUATypeData> dataTypes;
    std::vector<ScriptLUATypeData> dataTypesDefaultValues;

public:
    std::string scriptFilename;
    std::string fileTypes;

    void runEnvironment(sol::environment&, const std::string& func) const;

    void runGlobal(const std::string& func) const;

    void addDataType(const char *name, const char *type, cJSON *value);

    void parseTypesFromFileAttributes();

    static std::string dataTypesFileFor(std::string basicString);

    static std::string removeFilenameExtension(std::string& filename);

    void removeDataType(const ScriptLUATypeData& data);

    void updateFileTypes();

    void reloadEnvironment(sol::environment &environment);

    void reloadScriptCode();

    void getCode(const std::string &script);

    [[nodiscard]] bool updateScriptCodeWith(const std::string &content) const;

    void reloadGlobals();

    [[nodiscard]] bool isPaused() const;

    void setPaused(bool paused);

    void drawImGuiProperties();

    const std::vector<ScriptLUATypeData> &getDataTypes() const;

    cJSON *getTypesJSON();

    const std::string &getScriptFilename() const;

    void setDataTypesFromJSON(cJSON *typesJSON);

    void addDataTypeEmpty(const char *name, const char *type);

    bool existDataType(const char *name, const char *type);
};


#endif //BRAKEZA3D_SCRIPTLUA_H
