//
// Created by Eduardo on 04/01/2026.
//

#include "../../OpenGL/Nodes/ShaderBaseNodes.h"

#include "../../Misc/Logging.h"
#include "../../Misc/Tools.h"

void ShaderBaseNodes::LoadUniforms()
{
}

void ShaderBaseNodes::Destroy()
{
}

void ShaderBaseNodes::Reload()
{
}

void ShaderBaseNodes::Render(GLuint fbo, GLuint texture)
{
}

void ShaderBaseNodes::DrawImGuiProperties(const Image *diffuse, Image *specular)
{
}

void ShaderBaseNodes::SaveToFile() const
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "typesFile", getTypesFile().c_str());
    cJSON_AddStringToObject(root, "name", getLabel().c_str());
    auto typeString =getShaderTypeString(type);
    cJSON_AddStringToObject(root, "type", typeString.c_str());

    auto dataNodes = nodeManager->getJSONTypes();
    cJSON_AddItemToObject(root, "types", dataNodes);

    char *content = cJSON_Print(root);

    Tools::WriteToFile(getTypesFile(), content);
}

void ShaderBaseNodes::WriteEmptyCustomShaderToDisk(const std::string &name, const std::string &folder, ShaderCustomType type, NodeEditorManager *nodeManager)
{
    LOG_MESSAGE("[ShaderBaseNodes] Creating new custom shader '%s' of type '%d' in '%s'", name.c_str(), type, name.c_str());
    std::string typesFile = folder + std::string(name + ".json");

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "typesFile", typesFile.c_str());
    cJSON_AddStringToObject(root, "name", name.c_str());
    cJSON_AddStringToObject(root, "type", getShaderTypeString(type).c_str());

    auto dataNodes = nodeManager->getJSONTypes();
    cJSON_AddItemToObject(root, "types", dataNodes);

    char *content = cJSON_Print(root);

    // json
    Tools::WriteToFile(typesFile, content);
}
