//
// Created by eduardo on 3/10/23.
//

#include <fstream>
#include "../../include/Objects/ScriptLUA.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"

ScriptLUA::ScriptLUA(const std::string &script, std::string properties)
:
    scriptFilename(script),
    fileTypes(std::move(properties)),
    content(nullptr),
    paused(false)
{
    Logging::Message("Loading LUA Script (%s, %s)", script.c_str(), fileTypes.c_str());

    getCode(script);
    parseTypesFromFileAttributes();
}

ScriptLUA::ScriptLUA(const std::string &script, cJSON *types)
:
    scriptFilename(script),
    fileTypes(""),
    content(nullptr),
    paused(false)
{
    Logging::Message("Loading LUA Script (%s, %s)", script.c_str(), fileTypes.c_str());

    getCode(script);
    setDataTypesFromJSON(types);
}

void ScriptLUA::getCode(const std::string &script)
{
    size_t file_size;
    content = readFile(EngineSetup::get()->SCRIPTS_FOLDER + script, file_size);
}

char *ScriptLUA::readFile(const std::string &name, size_t &source_size)
{
    FILE *fp = fopen(name.c_str(), "r");

    if (!fp) {
        Logging::Message("File %s can't be loaded!", name.c_str());
        return nullptr;
    }

    fseek(fp, 0, SEEK_END);
    source_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *file_str = (char *)malloc(source_size + 1);
    if (!file_str) {
        fclose(fp);
        return nullptr;
    }

    fread(file_str, 1, source_size, fp);
    file_str[source_size] = '\0';
    fclose(fp);

    return file_str;
}

void ScriptLUA::runEnvironment(sol::environment &environment, const std::string& func) const
{
    if (paused) return;
    sol::state &lua = LUAManager::get()->getLua();

    try {
        lua.script(content, environment);
        if (!environment[func.c_str()]) {
            return;
        }
        sol::function f = environment[func];
        sol::function_result result = f();
        if (!result.valid()) {
            sol::error err = result;
            Logging::Message("LUA script error on file %s: %s", scriptFilename.c_str(), err.what());
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
        if (!lua[func]) {
            return;
        }
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

void ScriptLUA::addDataType(const char *name, const char *type, cJSON *value)
{
    LUADataValue LUAValue;

    switch (LUADataTypesMapping[type]) {
        case LUADataType::INT: {
            LUAValue = value->valueint;
            break;
        }
        case LUADataType::FLOAT: {
            LUAValue = (float) value->valuedouble;
            break;
        }
        case LUADataType::VERTEX3D: {
            LUAValue = SceneLoader::parseVertex3DJSON(value);
            break;
        }
        default:
            std::cerr << "Unknown data type." << std::endl;
    }

    dataTypes.emplace_back(name, type, LUAValue);
    dataTypesDefaultValues.emplace_back(name, type, LUAValue);
}

void ScriptLUA::reloadGlobals()
{
    Logging::Message("Reloading LUA Global Environment (%s)", this->fileTypes.c_str());

    sol::state &lua = LUAManager::get()->getLua();
    for (const auto& type : dataTypes) {
        std::cout << "Setting GLOBAL variable for script '(" << scriptFilename.c_str() << ", " << type.name.c_str() << std::endl;
        Logging::Message("Setting GLOBAL variable for script '%s' ('%s' => '%s')", scriptFilename.c_str(), type.name.c_str());
        lua[type.name] = type.value;
    }
}

void ScriptLUA::reloadEnvironment(sol::environment &environment)
{
    Logging::Message("Reloading LUA Environment (%s)", this->fileTypes.c_str());

    dataTypes = dataTypesDefaultValues;

    for (const auto& type : dataTypes) {
        Logging::Message("reloadEnvironment ('%s' => '%s')", type.name.c_str(), type.type.c_str());
        environment[type.name] = type.value;
    }
}

void ScriptLUA::parseTypesFromFileAttributes()
{
    size_t file_size;
    auto contentFile = readFile(EngineSetup::get()->SCRIPTS_FOLDER + fileTypes, file_size);

    setDataTypesFromJSON(cJSON_GetObjectItemCaseSensitive(cJSON_Parse(contentFile), "types"));
}

void ScriptLUA::setDataTypesFromJSON(cJSON *typesJSON)
{
    dataTypes.clear();

    cJSON *currentType;
    cJSON_ArrayForEach(currentType, typesJSON) {
        std::string name = cJSON_GetObjectItemCaseSensitive(currentType, "name")->valuestring;
        std::string type = cJSON_GetObjectItemCaseSensitive(currentType, "type")->valuestring;

        Logging::Message("Loading variable script (%s, %s)", name.c_str(), type.c_str());

        addDataType(
            name.c_str(),
            type.c_str(),
            cJSON_GetObjectItemCaseSensitive(currentType, "value")
        );
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

void ScriptLUA::removeDataType(ScriptLUATypeData data)
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

    cJSON *root = cJSON_CreateObject();
    cJSON *typesArray = cJSON_CreateArray();

    for (const auto &type : dataTypes) {
        cJSON *typeObject = cJSON_CreateObject();
        cJSON_AddStringToObject(typeObject, "name", type.name.c_str());
        //cJSON_AddStringToObject(typeObject, "value", type.value.c_str());
        cJSON_AddItemToArray(typesArray, typeObject);
    }

    cJSON_AddItemToObject(root, "types", typesArray);

    char *jsonString = cJSON_Print(root);

    std::string jsonResult(jsonString);
    updateFileTypesWith(jsonString);

    cJSON_Delete(root);
}

void ScriptLUA::updateFileTypesWith(const std::string& content) const
{
    std::ofstream file(EngineSetup::get()->SCRIPTS_FOLDER + this->fileTypes, std::ios::trunc);

    if (!file.is_open()) {
        Logging::Message("File %s can't be loaded!", fileTypes.c_str());
        return;
    }

    file << content;
    file.close();

    if (file.fail()) {
        Logging::Message("Error writing to file %s", fileTypes.c_str());
        return;
    }
}

bool ScriptLUA::updateScriptCodeWith(const std::string& content) const
{
    Logging::Message("Writing content in file (%s)", scriptFilename.c_str());

    std::ofstream file(EngineSetup::get()->SCRIPTS_FOLDER + this->scriptFilename, std::ios::trunc);

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
    if (ImGui::TreeNode("Script Settings")) {
        for (auto&  type: dataTypes) {
            switch (LUADataTypesMapping[type.type]) {
                case LUADataType::INT: {
                    const float rangeMin = -500000;
                    const float rangeMax = 500000;

                    int valueInt = std::get<int>(type.value);
                    if (ImGui::DragScalar(type.name.c_str(), ImGuiDataType_S32, &valueInt, 1.0 , &rangeMin, &rangeMax, "%d", 1.0f)) {
                        type.value = valueInt;
                    }
                    break;
                }
                case LUADataType::FLOAT: {
                    const float rangeMin = -500000;
                    const float rangeMax = 500000;
                    const float rangeSensibility = 0.1;

                    float valueFloat = std::get<float>(type.value);
                    if (ImGui::DragScalar(type.name.c_str(), ImGuiDataType_Float, &valueFloat, rangeSensibility , &rangeMin, &rangeMax, "%f", 1.0f)) {
                        type.value = valueFloat;
                    }
                    break;
                }
                case LUADataType::VERTEX3D: {
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

const std::vector<ScriptLUATypeData> &ScriptLUA::getDataTypes() const {
    return dataTypes;
}

cJSON *ScriptLUA::getJSON()
{
    cJSON *scriptJSON = cJSON_CreateObject();
    cJSON_AddStringToObject(scriptJSON, "name", getScriptFilename().c_str());


    cJSON *typesArray = cJSON_CreateArray();
    for (auto dataType : getDataTypes()) {
        cJSON *typeJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(typeJSON, "name", dataType.name.c_str());
        cJSON_AddStringToObject(typeJSON, "type", dataType.type.c_str());

        std::string name = dataType.name + "("+ dataType.type +")";
        switch (LUADataTypesMapping[dataType.type]) {
            case LUADataType::INT: {
                int valueInt = std::get<int>(dataType.value);
                cJSON_AddNumberToObject(typeJSON, "value", valueInt);
                break;
            }
            case LUADataType::FLOAT: {
                int valueFloat = std::get<float>(dataType.value);
                cJSON_AddNumberToObject(typeJSON, "value", valueFloat);
                break;
            }
            case LUADataType::VERTEX3D: {
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
