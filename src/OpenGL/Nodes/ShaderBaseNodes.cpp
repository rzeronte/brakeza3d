//
// Created by Eduardo on 04/01/2026.
//

#include "../../../include/OpenGL/Nodes/ShaderBaseNodes.h"

#include <cstdlib>

#include "../../../include/Misc/Logging.h"
#include "../../../include/Misc/Tools.h"

ShaderBaseNodes::~ShaderBaseNodes()
{
    delete nodeManager;
    nodeManager = nullptr;
}

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
    if (!nodeManager) return;

    ImGui::BeginChild(getLabel().c_str(), ImVec2(0, 350), true);
    nodeManager->Render();
    ImGui::EndChild();
}

void ShaderBaseNodes::SaveToFile() const
{
    if (!nodeManager) return;   // aún no se ha cargado el grafo

    cJSON *root = getTypesJSON();

    char *content = cJSON_Print(root);
    Tools::WriteToFile(getTypesFile(), content);

    free(content);
    cJSON_Delete(root);
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

    free(content);
    cJSON_Delete(root);
}

cJSON * ShaderBaseNodes::getTypesJSON() const
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "typesFile", fileTypes.c_str());
    cJSON_AddStringToObject(root, "name", label.c_str());
    cJSON_AddStringToObject(root, "type", getShaderTypeString(type).c_str());

    // Si el grafo todavía no se ha cargado (carga en curso) no hay "types" que guardar
    if (nodeManager) {
        cJSON_AddItemToObject(root, "types", nodeManager->getJSONTypes());
    }

    return root;
}
