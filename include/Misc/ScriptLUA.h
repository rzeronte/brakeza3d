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

    bool hasDataType(const char *name, const char *type) const;
    void RunEnvironment(sol::environment &environment, const std::string& func, std::optional<sol::object> arg = std::nullopt) const;
    void RunGlobal(const std::string& func) const;
    void AddDataType(const char *name, const char *type, cJSON *value);
    void ProcessFileTypes();
    void RemoveDataType(const ScriptLUATypeData& data);
    void UpdateFileTypes() const;
    void ReloadEnvironment(sol::environment &environment);
    void ReloadScriptCode();
    void getCode(const std::string &script);
    void InitEnvironment(sol::environment &environment);
    void Reload();
    void UpdateScriptCodeWith(const std::string &content) const;
    void ReloadGlobals() const;
    void DrawImGuiProperties();
    void AddDataTypeEmpty(const char *name, const char *type);
    [[nodiscard]] bool isPaused() const                                             { return paused; }
    [[nodiscard]] const std::vector<ScriptLUATypeData> &getDataTypes() const        { return dataTypes; }
    [[nodiscard]] const std::string &getScriptFilename() const                      { return scriptFilename; }
    [[nodiscard]] const std::string &getTypesFile() const                           { return fileTypes; }
    [[nodiscard]] std::string getName() const                                       { return name; }
    [[nodiscard]] cJSON *getTypesJSON() const;

    static std::string dataTypesFileFor(std::string basicString);
    static std::string RemoveFilenameExtension(std::string& filename);

    void setName(const std::string &name);
    void setPaused(bool value);
    void setDataTypesFromJSON(const cJSON *typesJSON);

    friend class ScriptLuaGUI;
};


#endif //BRAKEZA3D_SCRIPTLUA_H
