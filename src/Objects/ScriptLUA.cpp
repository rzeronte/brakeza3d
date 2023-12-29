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
    content(nullptr)
{
    Logging::Message("Loading LUA Script (%s, %s)", script.c_str(), fileTypes.c_str());

    getCode(script);
    parseTypes();
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

void ScriptLUA::addDataType(const char *name, const char *value)
{
    dataTypes.emplace_back(name, value);
}

void ScriptLUA::reloadGlobals()
{
    Logging::Message("Reloading LUA Global Environment (%s)", this->fileTypes.c_str());

    parseTypes();

    sol::state &lua = LUAManager::get()->getLua();
    for (auto type : dataTypes) {
        Logging::Message("Setting GLOBAL variable for script '%s' ('%s' => '%s')", scriptFilename.c_str(), type.name.c_str(), type.value.c_str());
        lua[type.name] = type.value;
    }
}

void ScriptLUA::reloadEnvironment(sol::environment &environment)
{
    Logging::Message("Reloading LUA Environment (%s)", this->fileTypes.c_str());
    parseTypes();

    for (const auto& type : dataTypes) {
        Logging::Message("reloadEnvironment ('%s' => '%s')", type.name.c_str(), type.value.c_str());
        environment[type.name] = type.value.c_str();
    }
}

void ScriptLUA::parseTypes()
{
    dataTypes.clear();

    size_t file_size;
    auto contentFile = readFile(EngineSetup::get()->SCRIPTS_FOLDER + fileTypes, file_size);

    cJSON *currentType;
    cJSON_ArrayForEach(currentType, cJSON_GetObjectItemCaseSensitive(cJSON_Parse(contentFile), "types")) {
        std::string name = cJSON_GetObjectItemCaseSensitive(currentType, "name")->valuestring;
        std::string value = cJSON_GetObjectItemCaseSensitive(currentType, "value")->valuestring;
        Logging::Message("Loading variable script (%s, %s)", name.c_str(), value.c_str());
        addDataType(name.c_str(), value.c_str());
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
        cJSON_AddStringToObject(typeObject, "value", type.value.c_str());
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

