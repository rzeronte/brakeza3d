//
// Created by eduardo on 3/10/23.
//
#ifndef BRAKEZA3D_SCRIPTLUA_H
#define BRAKEZA3D_SCRIPTLUA_H

#include <string>
#include <utility>
#include "../EngineSetup.h"
#include "../../sol/sol.hpp"
#include "../Misc/cJSON.h"

typedef std::variant<int, float, Vertex3D, const char*> LUADataValue;

struct ScriptLUATypeData {
    ScriptLUATypeData(const char *name, const char *type, LUADataValue value)
    : name(name), type(type), value(value)
    {}
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

class ScriptLUA {
private:
    bool paused;

public:
    explicit ScriptLUA(const std::string& script, std::string properties);

    ScriptLUA(const std::string &scriptFilename, cJSON *types);

    std::string content;
    std::vector<ScriptLUATypeData> dataTypes;
    std::vector<ScriptLUATypeData> dataTypesDefaultValues;

    std::string scriptFilename;
    std::string fileTypes;

    void runEnvironment(sol::environment &environment, const std::string& func, std::optional<sol::object> arg = std::nullopt) const;

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

    [[nodiscard]] const std::vector<ScriptLUATypeData> &getDataTypes() const;

    cJSON *getTypesJSON() const;

    [[nodiscard]] const std::string &getScriptFilename() const;

    void setDataTypesFromJSON(cJSON *typesJSON);

    void addDataTypeEmpty(const char *name, const char *type);

    bool existDataType(const char *name, const char *type);

    static ScriptLUA* create(const std::string& scriptFile);
};


#endif //BRAKEZA3D_SCRIPTLUA_H
