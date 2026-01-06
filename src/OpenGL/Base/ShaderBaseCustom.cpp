//
// Created by Eduardo on 03/01/2026.
//

#include "../../../include/OpenGL/Base/ShaderBaseCustom.h"

#include "../../../include/Components/Components.h"
#include "../../../include/Misc/Logging.h"
#include "../../../include/Misc/Tools.h"

ShaderBaseCustom::ShaderBaseCustom(const std::string &label, ShaderCustomType type, const std::string &vertexFilename, const std::string &fragmentFilename, const std::string &fileTypes, bool enableFeedback)
:
    ShaderBaseOpenGL(vertexFilename, fragmentFilename, enableFeedback),
    fileTypes(fileTypes),
    label(label),
    type(type)
{
}

ShaderBaseCustom::ShaderBaseCustom(const std::string &label, ShaderCustomType type, const std::string &vertexFilename, const std::string &fileTypes, bool enableFeedback)
    :
    ShaderBaseOpenGL(vertexFilename, enableFeedback),
    fileTypes(fileTypes),
    label(label),
    type(type)
{
}

ShaderBaseCustom::ShaderBaseCustom(const std::string &label, const std::string &fileTypes, ShaderCustomType type)
:
    fileTypes(fileTypes),
    label(label),
    type(type)
{
}

void ShaderBaseCustom::setLabel(const std::string &value)
{
    label = value;
}

void ShaderBaseCustom::setEnabled(bool value)
{
    LOG_MESSAGE("[ShaderOGLCustom] Setting shader '%s' enabled to %d", label.c_str(), value);
    enabled = value;
}

cJSON * ShaderBaseCustom::getTypesJSON() const {
}

ShaderCustomType ShaderBaseCustom::ExtractTypeFromShaderName(const std::string& folder, const std::string &name)
{
    std::string jsonFile = name + ".json";

    auto contentFile = Tools::ReadFile(folder + jsonFile);
    LOG_MESSAGE("[ShaderBaseCustomOGLCode] Extracting type from: '%s'", name.c_str());

    auto oJSON = cJSON_Parse(contentFile);
    std::string nameType = cJSON_GetObjectItemCaseSensitive(oJSON, "type")->valuestring;

    return getShaderTypeFromString(nameType);
}

ShaderCustomType ShaderBaseCustom::ExtractShaderTypeFromTypesFile(const std::string &fullPath)
{
    LOG_MESSAGE("[ShaderBaseCustom] Extracting type from: '%s'", fullPath.c_str());
    auto contentFile = Tools::ReadFile(fullPath);

    auto oJSON = cJSON_Parse(contentFile);
    std::string nameType = cJSON_GetObjectItemCaseSensitive(oJSON, "type")->valuestring;

    return getShaderTypeFromString(nameType);
}

ShaderCustomType ShaderBaseCustom::getShaderTypeFromString(const std::string& shaderName)
{
    auto render = Components::get()->Render();
    auto types = render->getShaderTypesMapping();

    auto it = types.find(shaderName);
    if (it != types.end()) {
        return it->second;
    }

    return SHADER_NONE;
}

std::string ShaderBaseCustom::getShaderTypeString(ShaderCustomType type)
{
    auto render = Components::get()->Render();
    auto types = render->getShaderTypesMapping();

    for (const auto& a: types) {
        if (a.second == type) {
            return a.first;
        }
    }

    return "";
}
