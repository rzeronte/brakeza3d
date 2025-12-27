//
// Created by eduardo on 3/10/23.
//

#include <fstream>
#include "../../include/Misc/ScriptLUA.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
#include "../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/ToolsJSON.h"

ScriptLUA::ScriptLUA(const std::string& name, const std::string &codeScript, const std::string &typesFile)
:
    scriptFilename(codeScript),
    fileTypes(typesFile),
    name(name)
{
    LOG_MESSAGE("Loading LUA Script (%s, %s)", codeScript.c_str(), fileTypes.c_str());
    if (!Tools::FileExists(scriptFilename.c_str()) || !Tools::FileExists(fileTypes.c_str())) {
        LOG_ERROR("[ScriptLUA] The script cannot be loaded. Files are missing.");
        LOG_ERROR("[ScriptLUA] Trying to load '%s' and '%s'", scriptFilename.c_str(), fileTypes.c_str());
        scriptFilename.clear(); fileTypes.clear();
        return;
    }
    getCode(scriptFilename);
    ProcessFileTypes();
}

ScriptLUA::ScriptLUA(const std::string& name, const std::string &codeFile, const std::string &typesFile, const cJSON *types)
:
    scriptFilename(codeFile),
    fileTypes(typesFile),
    name(name)
{
    LOG_MESSAGE("Loading LUA Script (%s, %s)", codeFile.c_str(), fileTypes.c_str());
    if (!Tools::FileExists(codeFile.c_str()) || !Tools::FileExists(fileTypes.c_str())) {
        LOG_ERROR("[ScriptLUA] The script cannot be loaded. Files are missing.");
        LOG_ERROR("[ScriptLUA] Trying to load '%s' and '%s'", codeFile.c_str(), fileTypes.c_str());
        this->scriptFilename.clear(); fileTypes.clear();
        return;
    }
    getCode(scriptFilename);
    setDataTypesFromJSON(types);
}

void ScriptLUA::getCode(const std::string &script)
{
    content.clear();
    content = Tools::ReadFile(script);
}

void ScriptLUA::InitEnvironment(sol::environment &environment)
{
    sol::state &lua = Components::get()->Scripting()->getLua();

    try {

        // 2. Cargar variables con conversión explícita
        for (const auto& type : dataTypes) {
            switch (LUADataTypesMapping[type.type].type) {
                case LUADataType::INT: {
                    int value = std::get<int>(type.value);
                    environment[type.name] = value;
                    break;
                }
                case LUADataType::FLOAT: {
                    float value = std::get<float>(type.value);
                    environment[type.name] = value;
                    break;
                }
                case LUADataType::STRING: {
                    const char* value = std::get<const char*>(type.value);
                    environment[type.name] = std::string(value);  // ← Convertir a std::string
                    break;
                }
                case LUADataType::VERTEX3D: {
                    Vertex3D value = std::get<Vertex3D>(type.value);
                    environment[type.name] = value;
                    break;
                }
                default:
                    LOG_ERROR("[ScriptLUA] Unknown data type for '%s'", type.name.c_str());
                    break;
            }

            lua.script(content, environment);

            LOG_MESSAGE("[ScriptLUA] Set variable '%s' = '%s'", type.name.c_str(), ScriptLUATypeData::toString(type.value).c_str());
        }

        LOG_MESSAGE("[ScriptLUA] Environment initialized for '%s'", name.c_str());
    } catch (const sol::error& e) {
        LOG_ERROR("[ScriptLUA] Error initializing environment: %s", e.what());
        throw;
    }
}

void ScriptLUA::RunEnvironment(sol::environment &environment, const std::string& func, std::optional<sol::object> arg) const
{
    if (paused) return;

    if (!environment.valid()) {
        LOG_ERROR("[ScriptLUA] Environment is INVALID for script '%s'", scriptFilename.c_str());
        return;
    }

    sol::object obj = environment[func];
    if (!obj.is<sol::function>()) {
        LOG_ERROR("[ScriptLUA Error] Function %s not exists in script '%s'", func.c_str(), scriptFilename.c_str());
        return;
    }

    try {

        if (!obj.valid()) {
            LOG_ERROR("[ScriptLUA] Object '%s' is invalid in environment", func.c_str());
            return;
        }

        if (!obj.is<sol::function>()) {
            LOG_MESSAGE("[ScriptLUA Error] Function %s not exists in script '%s'", func.c_str(), scriptFilename.c_str());
            return;
        }

        sol::protected_function f = obj.as<sol::protected_function>();
        sol::protected_function_result result;

        if (arg) {
            result = f(*arg);
        } else {
            result = f();
        }

        if (!result.valid()) {
            sol::error err = result;
            LOG_ERROR("[ScriptLUA] Error in function %s: %s", func.c_str(), err.what());
            Components::get()->Scripting()->StopLUAScripts();
        }
    } catch (const std::exception& e) {
        LOG_ERROR("[ScriptLUA] Exception accessing environment: %s", e.what());
    }
}

void ScriptLUA::RunGlobal(const std::string& func) const
{
    if (paused) return;
    sol::state &lua = Components::get()->Scripting()->getLua();

    try {
        lua.script(content);

        sol::function f = lua[func];
        sol::function_result result = f();

        if (!result.valid()) {
            sol::error err = result;
            LOG_ERROR("[ScriptLUA] Error in LUA Script %s", scriptFilename.c_str());
            LOG_ERROR("[ScriptLUA] %s", scriptFilename.c_str(), err.what());
            Components::get()->Scripting()->StopLUAScripts();
        }
    } catch (const sol::error& e) {
        LOG_ERROR("[ScriptLUA] Error in LUA Script %s", scriptFilename.c_str());
        LOG_ERROR("[ScriptLUA] %s", e.what());
        Components::get()->Scripting()->StopLUAScripts();
    }
}

void ScriptLUA::AddDataTypeEmpty(const char *name, const char *type)
{
    LUADataValue LUAValue;

    switch (LUADataTypesMapping[type].type) {
        case LUADataType::INT: {
            LUAValue = 0;
            break;
        }
        case LUADataType::FLOAT: {
            LUAValue = 0.0f;
            break;
        }
        case LUADataType::STRING: {
            LUAValue = "";
            break;
        }
        case LUADataType::VERTEX3D: {
            LUAValue = Vertex3D();
            break;
        }
        default:
            break;
    }

    dataTypes.emplace_back(name, type, LUAValue);
    dataTypesDefaultValues.emplace_back(name, type, LUAValue);
}

void ScriptLUA::AddDataType(const char *name, const char *type, cJSON *value)
{
    LUADataValue LUAValue;

    switch (LUADataTypesMapping[type].type) {
        case LUADataType::INT: {
            LUAValue = value->valueint;
            break;
        }
        case LUADataType::FLOAT: {
            LUAValue = static_cast<float>(value->valuedouble);
            break;
        }
        case LUADataType::STRING: {
            LUAValue = value->valuestring;
            break;
        }
        case LUADataType::VERTEX3D: {
            LUAValue = ToolsJSON::getVertex3DByJSON(value);
            break;
        }
        default:
            break;
    }

    dataTypes.emplace_back(name, type, LUAValue);
    dataTypesDefaultValues.emplace_back(name, type, LUAValue);
}

bool ScriptLUA::hasDataType(const char *name, const char *type) const
{
    for (const auto& t: dataTypes) {
        if (t.name == name && t.type == type) {
            return true;
        }
    }

    return false;
}

void ScriptLUA::ReloadGlobals() const
{
    LOG_MESSAGE("[ScriptLUA] Reloading LUA Global Environment (%s)", this->fileTypes.c_str());

    sol::state &lua = Components::get()->Scripting()->getLua();
    for (const auto& type : dataTypes) {
        LOG_MESSAGE("[ScriptLUA] Setting GLOBAL variable => Script: '%s', Name: '%s', Type: '%s', Value: '%s'", this->getName().c_str(), type.name.c_str(), type.type.c_str(), ScriptLUATypeData::toString(type.value).c_str());
        lua[type.name] = type.value;
    }
}

void ScriptLUA::ReloadEnvironment(sol::environment &environment)
{
    LOG_MESSAGE("[ScriptLUA] Reloading LUA Environment (%s)", this->fileTypes.c_str());

    ProcessFileTypes();
    sol::state &lua = Components::get()->Scripting()->getLua();

    try {
        lua.script(content, environment);
    } catch (const sol::error& e) {
        LOG_ERROR("[ScriptLUA] Error loading script: %s", e.what());
        return;
    }

    for (const auto& type : dataTypes) {
        switch (LUADataTypesMapping[type.type].type) {
            case LUADataType::INT:
                environment[type.name] = std::get<int>(type.value);
                break;
            case LUADataType::FLOAT:
                environment[type.name] = std::get<float>(type.value);
                break;
            case LUADataType::STRING:
                environment[type.name] = std::string(std::get<const char*>(type.value));
                break;
            case LUADataType::VERTEX3D:
                environment[type.name] = std::get<Vertex3D>(type.value);
                break;
        }
    }
}

void ScriptLUA::ProcessFileTypes()
{
    if (!Tools::FileExists(fileTypes.c_str())) {
        LOG_ERROR("[ScriptLUA] The script cannot be loaded. Files are missing.");
        LOG_ERROR("[ScriptLUA] Trying to load '%s'", fileTypes.c_str());
        return;
    }

    auto contentFile = Tools::ReadFile(fileTypes);
    LOG_MESSAGE("[ScriptLUA] Parsing attributes from: '%s'", fileTypes.c_str());

    setName(cJSON_GetObjectItemCaseSensitive(cJSON_Parse(contentFile), "name")->valuestring);
    setDataTypesFromJSON(cJSON_GetObjectItemCaseSensitive(cJSON_Parse(contentFile), "types"));
}

void ScriptLUA::setDataTypesFromJSON(const cJSON *typesJSON)
{
    cJSON *currentType;
    cJSON_ArrayForEach(currentType, typesJSON) {
        auto name = cJSON_GetObjectItemCaseSensitive(currentType, "name")->valuestring;
        auto type = cJSON_GetObjectItemCaseSensitive(currentType, "type")->valuestring;
        auto value = cJSON_GetObjectItemCaseSensitive(currentType, "value");

        if (!hasDataType(name, type)){
            AddDataType(name, type, value);
            LOG_MESSAGE("[ScriptLUA] Loading script variable: '%s': %s => %s", getName().c_str(), name, type);
        } else {
            LOG_MESSAGE("[ScriptLUA] Keeping script variable: '%s': %s => %s", getName().c_str(), name, type);
        }
    }
}

void ScriptLUA::Reload()
{
    getCode(scriptFilename);
    ProcessFileTypes();
}

std::string ScriptLUA::dataTypesFileFor(std::string basicString)
{
    return RemoveFilenameExtension(basicString) + ".json";
}

std::string ScriptLUA::RemoveFilenameExtension(std::string& filename)
{
    size_t dotPosition = filename.find_last_of('.');

    if (dotPosition != std::string::npos) {
        return filename.substr(0, dotPosition);
    }

    return filename;
}

void ScriptLUA::setName(const std::string &value)
{
    name = value;
}

void ScriptLUA::RemoveDataType(const ScriptLUATypeData& data)
{
    for (auto it = dataTypes.begin(); it != dataTypes.end(); ++it) {
        if (it->name == data.name) {
            dataTypes.erase(it);
            return;
        }
    }
}

void ScriptLUA::UpdateFileTypes() const
{
    LOG_MESSAGE("Updating types file (%s)", this->fileTypes.c_str());
    char *output_string = cJSON_Print(getTypesJSON());

    Tools::WriteToFile(this->fileTypes, output_string);

    delete output_string;
}

void ScriptLUA::UpdateScriptCodeWith(const std::string& content) const
{
    LOG_MESSAGE("Writing content in file (%s)", scriptFilename.c_str());

    std::ofstream file(this->scriptFilename, std::ios::trunc);

    if (!file.is_open()) {
        std::cerr << "ScriptLUA: Error opening file " << scriptFilename << std::endl;
        exit(-1);
    }

    file << content;
    file.close();

    if (file.fail()) {
        LOG_MESSAGE("ScriptLUA: Error writing to file %s", fileTypes.c_str());
        exit(-1);
    }

    LOG_MESSAGE("Done!");
}

void ScriptLUA::ReloadScriptCode()
{
    getCode(scriptFilename);
}

void ScriptLUA::setPaused(bool value)
{
    LOG_MESSAGE("Script %s has been paused to %d", scriptFilename.c_str(), value);
    paused = value;
}

void ScriptLUA::DrawImGuiProperties()
{
    ScriptLuaGUI::DrawPropertiesGUI(this);
}

cJSON *ScriptLUA::getTypesJSON() const
{
    cJSON *scriptJSON = cJSON_CreateObject();

    cJSON_AddStringToObject(scriptJSON, "codeFile", scriptFilename.c_str());
    cJSON_AddStringToObject(scriptJSON, "typesFile", fileTypes.c_str());
    cJSON_AddStringToObject(scriptJSON, "name", getName().c_str());

    cJSON *typesArray = cJSON_CreateArray();
    for (auto dataType : getDataTypes()) {
        cJSON *typeJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(typeJSON, "name", dataType.name.c_str());
        cJSON_AddStringToObject(typeJSON, "type", dataType.type.c_str());

        std::string name = dataType.name + "("+ dataType.type +")";
        switch (LUADataTypesMapping[dataType.type].type) {
            case LUADataType::INT: {
                int valueInt = std::get<int>(dataType.value);
                cJSON_AddNumberToObject(typeJSON, "value", valueInt);
                break;
            }
            case LUADataType::STRING: {
                std::string valueString = std::get<const char *>(dataType.value);
                cJSON_AddStringToObject(typeJSON, "value", valueString.c_str());
                break;
            }
            case LUADataType::FLOAT: {
                float valueFloat = std::get<float>(dataType.value);
                cJSON_AddNumberToObject(typeJSON, "value", valueFloat);
                break;
            }
            case LUADataType::VERTEX3D: {
                auto valueVertex = std::get<Vertex3D>(dataType.value);
                cJSON_AddItemToObject(typeJSON, "value", ToolsJSON::Vertex3DToJSON(valueVertex));

                break;
            }
            default:
                std::cerr << "Unknown data typeJSON." << std::endl;
        }
        cJSON_AddItemToArray(typesArray, typeJSON);
    }

    cJSON_AddItemToObject(scriptJSON, "types", typesArray);

    return scriptJSON;
}
