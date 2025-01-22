//
// Created by eduardo on 3/10/23.
//

#include <fstream>
#include "../../include/Objects/ScriptLUA.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Misc/ToolsJSON.h"

ScriptLUA::ScriptLUA(const std::string &script, std::string properties)
:
    paused(false),
    scriptFilename(script),
    fileTypes(std::move(properties))
{
    Logging::Message("Loading LUA Script (%s, %s)", script.c_str(), fileTypes.c_str());

    getCode(script);
    parseTypesFromFileAttributes();
}

ScriptLUA::ScriptLUA(const std::string &scriptFilename, cJSON *types)
:
    paused(false),
    scriptFilename(scriptFilename),
    fileTypes(ScriptLUA::dataTypesFileFor(scriptFilename))
{
    Logging::Message("Loading LUA Script (%s, %s)", scriptFilename.c_str(), fileTypes.c_str());

    getCode(scriptFilename);
    setDataTypesFromJSON(types);
}

void ScriptLUA::getCode(const std::string &script)
{
    size_t file_size;
    content.clear();
    content = Tools::readFile(script, file_size);
}

void ScriptLUA::runEnvironment(sol::environment &environment, const std::string& func, std::optional<sol::object> arg) const
{
    if (paused) return;
    sol::state &lua = LUAManager::get()->getLua();

    try {
        lua.script(content, environment);

        const sol::function f = environment[func];

        sol::function_result result;
        if (arg) {
            result = f(*arg);  // Llama con el argumento
        } else {
            result = f();         // Llama sin argumentos
        }

        if (!result.valid()) {
            sol::error err = result;
            Logging::Message("LUA fail func error on file %s: %s: %s", scriptFilename.c_str(), err.what(), func.c_str());
        }
    } catch (const sol::error& e) {
        Logging::Message("LUA script error on file %s: %s", scriptFilename.c_str(), e.what());
    }
}


void ScriptLUA::runGlobal(const std::string& func) const
{
    if (paused) return;
    sol::state &lua = LUAManager::get()->getLua();

    try {
        lua.script(content);

        sol::function f = lua[func];
        sol::function_result result = f();

        if (!result.valid()) {
            sol::error err = result;
            Logging::Message("LUA script error on file %s: %s", scriptFilename.c_str(), err.what());
        }
    } catch (const sol::error& e) {
        Logging::Message("LUA script error on file %s: %s", scriptFilename.c_str(), e.what());
    }
}

void ScriptLUA::addDataTypeEmpty(const char *name, const char *type)
{
    LUADataValue LUAValue;

    switch (EngineSetup::get()->LUADataTypesMapping[type]) {
        case EngineSetup::LUADataType::INT: {
            LUAValue = 0;
            break;
        }
        case EngineSetup::LUADataType::FLOAT: {
            LUAValue = 0.0f;
            break;
        }
        case EngineSetup::LUADataType::STRING: {
            LUAValue = "";
            break;
        }
        case EngineSetup::LUADataType::VERTEX3D: {
            LUAValue = Vertex3D();
            break;
        }
        default:
            break;
    }

    dataTypes.emplace_back(name, type, LUAValue);
    dataTypesDefaultValues.emplace_back(name, type, LUAValue);
}

void ScriptLUA::addDataType(const char *name, const char *type, cJSON *value)
{
    LUADataValue LUAValue;

    switch (EngineSetup::get()->LUADataTypesMapping[type]) {
        case EngineSetup::LUADataType::INT: {
            LUAValue = value->valueint;
            break;
        }
        case EngineSetup::LUADataType::FLOAT: {
            LUAValue = (float) value->valuedouble;
            break;
        }
        case EngineSetup::LUADataType::STRING: {
            LUAValue = value->valuestring;
            break;
        }
        case EngineSetup::LUADataType::VERTEX3D: {
            LUAValue = ToolsJSON::parseVertex3DJSON(value);
            break;
        }
        default:
            break;
    }

    dataTypes.emplace_back(name, type, LUAValue);
    dataTypesDefaultValues.emplace_back(name, type, LUAValue);
}

bool ScriptLUA::existDataType(const char *name, const char *type)
{
    for (const auto& t: dataTypes) {
        if (t.name == name && t.type == type) {
            return true;
        }
    }

    return false;
}

void ScriptLUA::reloadGlobals()
{
    Logging::Message("Reloading LUA Global Environment (%s)", this->fileTypes.c_str());

    sol::state &lua = LUAManager::get()->getLua();
    for (const auto& type : dataTypes) {
        std::cout << "Setting GLOBAL variable for script '(" << scriptFilename.c_str() << ", " << type.name.c_str() << ", " << type.type.c_str() << ")"<< std::endl;
        Logging::Message("Setting GLOBAL variable for script '%s' (Name: '%s', Type: '%s', Value: '%s')", scriptFilename.c_str(), type.name.c_str(), type.type.c_str(), ScriptLUATypeData::toString(type.value).c_str());
        lua[type.name] = type.value;
    }
}

void ScriptLUA::reloadEnvironment(sol::environment &environment)
{
    Logging::Message("Reloading LUA Environment (%s)", this->fileTypes.c_str());

    parseTypesFromFileAttributes();

    //dataTypes = dataTypesDefaultValues;

    for (const auto& type : dataTypes) {
        Logging::Message("reloadEnvironment ('%s' => '%s' => '%s')", type.name.c_str(), type.type.c_str(), ScriptLUATypeData::toString(type.value).c_str());
        environment[type.name] = type.value;
    }
}

void ScriptLUA::parseTypesFromFileAttributes()
{
    size_t file_size;
    auto contentFile = Tools::readFile(fileTypes, file_size);
    Logging::Message("Parsing attributes from: '%s'", fileTypes.c_str());

    setDataTypesFromJSON(cJSON_GetObjectItemCaseSensitive(cJSON_Parse(contentFile), "types"));
}

void ScriptLUA::setDataTypesFromJSON(cJSON *typesJSON)
{
    cJSON *currentType;
    cJSON_ArrayForEach(currentType, typesJSON) {
        auto name = cJSON_GetObjectItemCaseSensitive(currentType, "name")->valuestring;
        auto type = cJSON_GetObjectItemCaseSensitive(currentType, "type")->valuestring;
        auto value = cJSON_GetObjectItemCaseSensitive(currentType, "value");

        if (!existDataType(name, type)){
            addDataType(name, type, value);
            Logging::Message("Loading script variable (%s, %s)", name, type);
        } else {
            Logging::Message("Keeping script variable (%s, %s)", name, type);
        }
    }
}

std::string ScriptLUA::dataTypesFileFor(std::string basicString)
{
    return ScriptLUA::removeFilenameExtension(basicString) + ".json";
}

std::string ScriptLUA::removeFilenameExtension(std::string& filename)
{
    size_t dotPosition = filename.find_last_of('.');

    if (dotPosition != std::string::npos) {
        return filename.substr(0, dotPosition);
    }

    return filename;
}

void ScriptLUA::removeDataType(const ScriptLUATypeData& data)
{
    for (auto it = dataTypes.begin(); it != dataTypes.end(); ++it) {
        if (it->name == data.name) {
            dataTypes.erase(it);
            return;
        }
    }
}

void ScriptLUA::updateFileTypes()
{
    Logging::Message("Updating types file (%s)", this->fileTypes.c_str());
    char *output_string = cJSON_Print(getTypesJSON());

    Tools::writeToFile(this->fileTypes, output_string);

    delete output_string;
}

bool ScriptLUA::updateScriptCodeWith(const std::string& content) const
{
    Logging::Message("Writing content in file (%s)", scriptFilename.c_str());

    std::ofstream file(this->scriptFilename, std::ios::trunc);

    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo." << std::endl;
        return false;
    }

    file << content;
    file.close();

    if (file.fail()) {
        Logging::Message("Error writing to file %s", fileTypes.c_str());
        return false;
    }
    Logging::Message("Done!");

    return true;
}

void ScriptLUA::reloadScriptCode()
{
    getCode(scriptFilename);
}

bool ScriptLUA::isPaused() const {
    return paused;
}

void ScriptLUA::setPaused(bool paused) {
    Logging::Message("Script %s has been paused to %d", scriptFilename.c_str(), paused);
    ScriptLUA::paused = paused;
}

void ScriptLUA::drawImGuiProperties()
{
    if ((int) dataTypes.size() <= 0) {
        ImGui::Text("No variables found");
        return;
    }

    if (ImGui::TreeNode("Script Variables")) {
        for (auto&  type: dataTypes) {
            switch (EngineSetup::get()->LUADataTypesMapping[type.type]) {
                case EngineSetup::LUADataType::INT: {
                    const float rangeMin = -500000;
                    const float rangeMax = 500000;

                    int valueInt = std::get<int>(type.value);
                    if (ImGui::DragScalar(type.name.c_str(), ImGuiDataType_S32, &valueInt, 1.0 , &rangeMin, &rangeMax, "%d", 1.0f)) {
                        type.value = valueInt;
                    }
                    break;
                }
                case EngineSetup::LUADataType::STRING: {
                    std::string valueString = std::get<const char*>(type.value);
                    static char name[256];
                    strncpy(name, valueString.c_str(), sizeof(name));
                    ImGui::InputText(type.name.c_str(), name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AlwaysOverwrite);
                    if (ImGui::IsItemEdited()) {
                        type.value = name;
                    }

                    break;
                }
                case EngineSetup::LUADataType::FLOAT: {
                    const float rangeMin = -500000;
                    const float rangeMax = 500000;
                    const float rangeSensibility = 0.1;

                    float valueFloat = std::get<float>(type.value);
                    if (ImGui::DragScalar(type.name.c_str(), ImGuiDataType_Float, &valueFloat, rangeSensibility , &rangeMin, &rangeMax, "%f", 1.0f)) {
                        type.value = valueFloat;
                    }
                    break;
                }
                case EngineSetup::LUADataType::VERTEX3D: {
                    if (ImGui::TreeNode(type.name.c_str())) {
                        const float range_min = -50000;
                        const float range_max = 50000;
                        const float range_sensibility = 0.1;
                        Vertex3D valueVertex = std::get<Vertex3D>(type.value);

                        if (ImGui::DragScalar("X", ImGuiDataType_Float, &valueVertex.x, range_sensibility ,&range_min, &range_max, "%f", 1.0f)) {
                            type.value = valueVertex;
                        }
                        if (ImGui::DragScalar("Y", ImGuiDataType_Float, &valueVertex.y, range_sensibility ,&range_min, &range_max, "%f", 1.0f)) {
                            type.value = valueVertex;
                        }
                        if (ImGui::DragScalar("Z", ImGuiDataType_Float, &valueVertex.z, range_sensibility ,&range_min, &range_max, "%f", 1.0f)) {
                            type.value = valueVertex;
                        }

                        ImGui::TreePop();
                    }

                    break;
                }
                default:
                    std::cerr << "Unknown data type." << std::endl;
            }
        }

        ImGui::TreePop();
    }
}

const std::vector<ScriptLUATypeData> &ScriptLUA::getDataTypes() const
{
    return dataTypes;
}

cJSON *ScriptLUA::getTypesJSON() const
{
    cJSON *scriptJSON = cJSON_CreateObject();
    cJSON_AddStringToObject(scriptJSON, "name", getScriptFilename().c_str());

    cJSON *typesArray = cJSON_CreateArray();
    for (auto dataType : getDataTypes()) {
        cJSON *typeJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(typeJSON, "name", dataType.name.c_str());
        cJSON_AddStringToObject(typeJSON, "type", dataType.type.c_str());

        std::string name = dataType.name + "("+ dataType.type +")";
        switch (EngineSetup::get()->LUADataTypesMapping[dataType.type]) {
            case EngineSetup::LUADataType::INT: {
                int valueInt = std::get<int>(dataType.value);
                cJSON_AddNumberToObject(typeJSON, "value", valueInt);
                break;
            }
            case EngineSetup::LUADataType::STRING: {
                std::string valueString = std::get<const char *>(dataType.value);
                cJSON_AddStringToObject(typeJSON, "value", valueString.c_str());
                break;
            }
            case EngineSetup::LUADataType::FLOAT: {
                float valueFloat = std::get<float>(dataType.value);
                cJSON_AddNumberToObject(typeJSON, "value", valueFloat);
                break;
            }
            case EngineSetup::LUADataType::VERTEX3D: {
                cJSON *vertexJSON = cJSON_CreateObject();
                auto valueVertex = std::get<Vertex3D>(dataType.value);
                cJSON_AddNumberToObject(vertexJSON, "x", valueVertex.x);
                cJSON_AddNumberToObject(vertexJSON, "y", valueVertex.y);
                cJSON_AddNumberToObject(vertexJSON, "z", valueVertex.z);
                cJSON_AddItemToObject(typeJSON, "value", vertexJSON);
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

const std::string &ScriptLUA::getScriptFilename() const {
    return scriptFilename;
}

ScriptLUA *ScriptLUA::create(const std::string& scriptFile)
{
    auto typesFile = dataTypesFileFor(scriptFile);

    std::cout << typesFile.c_str() << std::endl;

    if (!Tools::fileExists(typesFile.c_str())) {
        Logging::Message("Failed loading TypesFile: %s", typesFile.c_str());

        return nullptr;
    }

    return new ScriptLUA(scriptFile,typesFile);
}
