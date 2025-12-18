//
// Created by eduardo on 3/10/23.
//
#ifndef BRAKEZA3D_SCRIPTLUA_H
#define BRAKEZA3D_SCRIPTLUA_H

#include <map>
#include <string>
#include <utility>
#include "cJSON.h"
#include "../Config.h"
#include "../../sol/sol.hpp"

typedef std::variant<int, float, Vertex3D, const char*> LUADataValue;

enum class LUADataType {
    INT,
    FLOAT,
    VERTEX3D,
    STRING
};

struct LUATypeInfo {
    LUADataType type;
    std::string label;
};

static std::map<std::string, LUATypeInfo> LUADataTypesMapping = {
    {"int", {LUADataType::INT, "Integer" }},
    {"float", {LUADataType::FLOAT, "Float" }},
    {"string", {LUADataType::STRING, "String" }},
    {"Vertex3D", {LUADataType::VERTEX3D, "Vertex3D" }},
};

struct ScriptLUATypeData {
    ScriptLUATypeData(const char *name, const char *type, LUADataValue value)
    : name(name), type(type), value(value)
    {
    }

    std::string name;
    std::string type;
    LUADataValue value;

    static std::string toString(LUADataValue value) {
        return std::visit([](const auto& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, int>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, float>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, Vertex3D>) {
                return std::string("(x: " + std::to_string(arg.x) + ", y: " + std::to_string(arg.x) + ", z: " + std::to_string(arg.z));
            } else if constexpr (std::is_same_v<T, const char*>) {
                return "Es un string";
            } else {
                return "Unknown type";
            }
        }, value);
    }
};

struct ScriptMetaInfo {
    std::string name;
    std::string codeFile;
    std::string typesFile;
};

class ScriptLUA
{
    bool paused = false;

public:
    std::string content;
    std::vector<ScriptLUATypeData> dataTypes;
    std::vector<ScriptLUATypeData> dataTypesDefaultValues;

    std::string scriptFilename;
    std::string fileTypes;
    std::string name;

    explicit ScriptLUA(const std::string& name, const std::string& codeFile, const std::string &typesFile);
    ScriptLUA(const std::string& name, const std::string &codeFile, const std::string &typesFile, const cJSON *types);

    bool existDataType(const char *name, const char *type) const;
    void runEnvironment(sol::environment &environment, const std::string& func, std::optional<sol::object> arg = std::nullopt) const;
    void runGlobal(const std::string& func) const;
    void addDataType(const char *name, const char *type, cJSON *value);
    void parseTypesFromFileAttributes();
    void removeDataType(const ScriptLUATypeData& data);
    void updateFileTypes() const;
    void ReloadEnvironment(sol::environment &environment);
    void reloadScriptCode();
    void getCode(const std::string &script);
    void updateScriptCodeWith(const std::string &content) const;
    void ReloadGlobals() const;
    void setPaused(bool value);
    void drawImGuiProperties();
    void setDataTypesFromJSON(const cJSON *typesJSON);
    void addDataTypeEmpty(const char *name, const char *type);
    [[nodiscard]] bool isPaused() const { return paused; }
    [[nodiscard]] const std::vector<ScriptLUATypeData> &getDataTypes() const { return dataTypes; }
    [[nodiscard]] cJSON *getTypesJSON() const;
    [[nodiscard]] const std::string &getScriptFilename() const { return scriptFilename; }

    void Reload();

    static std::string dataTypesFileFor(std::string basicString);
    static std::string removeFilenameExtension(std::string& filename);

    [[nodiscard]] std::string getName() const { return name; }

    void setName(const std::string &name);

    friend class ScriptLuaGUI;
};


#endif //BRAKEZA3D_SCRIPTLUA_H
