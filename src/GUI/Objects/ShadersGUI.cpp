//
// Created by Eduardo on 26/11/2025.
//

#include <string>
#include <vector>
#include "../../../include/Brakeza.h"
#include "../../../include/GUI/Objects/ShadersGUI.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/GUI/Editable/EditableOpenShaderFile.h"
#include "../../../include/GUI/Editable/EditableOpenNode.h"
#include "../../../include/GUI/AddOns/CustomTreeNode.h"
#include "../../../include/Components/Components.h"
#include "../../../include/OpenGL/Code/ShaderCustomOGLCodeTypes.h"
#include "../../../include/OpenGL/Code/ShaderOGLCustomCodeMesh3D.h"
#include "../../../include/OpenGL/Code/ShaderOGLCustomCodePostprocessing.h"
#include "../../../include/OpenGL/Nodes/ShaderBaseNodes.h"
#include "../../../include/OpenGL/Nodes/ShaderNodesMesh3D.h"
#include "../../../include/OpenGL/Nodes/ShaderNodesPostProcessing.h"
#include "../../../include/Render/Drawable.h"

void ShadersGUI::DrawShaderConfig(EditableOpenShaderFile &file)
{
    DrawShaderConfigHeader(file);
    DrawShaderConfigEditName(file);
    DrawShaderConfigVarsCreator(file);
    DrawShaderConfigVarsTable(file);
    DrawShaderConfigEmptyStateWarning(file);
    ImGui::Separator();
    DrawShaderConfigActionButtons(file);
}

void ShadersGUI::DrawShaderConfigHeader(EditableOpenShaderFile &file)
{
    auto vsFile = file.getShader()->getVertexFilename();
    auto fsFile = file.getShader()->getFragmentFilename();
    auto type = file.getShader()->getType();
    auto typeName = ShaderBaseCustom::getShaderTypeString(type);

    ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_CODE_VS), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::Text(std::string("VS: " + Tools::removeSubstring(vsFile, Config::get()->ASSETS_FOLDER)).c_str());

    ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_CODE_FS), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::Text(std::string("FS: " + Tools::removeSubstring(fsFile, Config::get()->ASSETS_FOLDER)).c_str());

    ImGui::Separator();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_TYPE_MESH3D), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::Text(std::string("Shader type: " + typeName).c_str());
}

void ShadersGUI::DrawShaderConfigEditName(EditableOpenShaderFile &file)
{
    ImGui::Separator();

    auto label = file.getShader()->getLabel();
    static char name[256];
    strncpy(name, label.c_str(), sizeof(name));

    if (ImGui::InputText("Name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        file.getShader()->setLabel(name);
    }
}

void ShadersGUI::DrawShaderConfigVarsCreator(EditableOpenShaderFile &file)
{
    // Listas precalculadas (solo se hacen una vez)
    static std::vector<std::string> allGlslTypes;
    static std::vector<std::string> allLabels;
    static std::vector<const char*> allLabelsCStr;

    static std::vector<std::string> postprocessingGlslTypes;
    static std::vector<std::string> postprocessingLabels;
    static std::vector<const char*> postprocessingLabelsCStr;

    static bool initialized = false;
    if (!initialized) {
        // PRIMERO: Llenar todos los strings
        for (const auto& [glslType, info] : GLSLTypeMapping) {
            allGlslTypes.push_back(glslType);
            allLabels.push_back(info.label);

            if (info.type != ShaderOpenGLCustomDataType::DIFFUSE &&
                info.type != ShaderOpenGLCustomDataType::SPECULAR) {
                postprocessingGlslTypes.push_back(glslType);
                postprocessingLabels.push_back(info.label);
            }
        }

        // DESPUÉS: Llenar los punteros (ahora los vectores ya no se realojarán)
        for (const auto& label : allLabels) {
            allLabelsCStr.push_back(label.c_str());
        }
        for (const auto& label : postprocessingLabels) {
            postprocessingLabelsCStr.push_back(label.c_str());
        }

        initialized = true;
    }

    // Elegir la lista según el tipo de shader
    bool isPostprocessing = (file.getShader()->getType() == SHADER_POSTPROCESSING);
    const auto& glslTypes = isPostprocessing ? postprocessingGlslTypes : allGlslTypes;
    const auto& labelsCStr = isPostprocessing ? postprocessingLabelsCStr : allLabelsCStr;

    ImGui::SeparatorText("Create new uniform");

    static char localVarName[256] = "";
    auto fieldId = std::string("Uniform name##") + "_" + file.getShader()->getLabel();
    ImGui::InputText(fieldId.c_str(), localVarName, IM_ARRAYSIZE(localVarName), ImGuiInputTextFlags_AutoSelectAll);

    static int selectedItem = 0;

    auto comboId = std::string("Type##") + "_" + file.getShader()->getLabel();
    ImGui::Combo(comboId.c_str(), &selectedItem, labelsCStr.data(), (int) labelsCStr.size());
    GUI::ImageButtonNormal(IconGUI::SHADER_CREATE_VARIABLE, "Create uniform", [&] {
        if (localVarName[0] != '\0' && selectedItem < glslTypes.size()) {
            file.getShader()->AddDataTypeEmpty(localVarName, glslTypes[selectedItem].c_str());
            localVarName[0] = '\0';
        }
    });
}

std::vector<std::string> ShadersGUI::GetDataTypeItems(EditableOpenShaderFile &file)
{
    std::vector<std::string> items;

    for (const auto& t : GLSLTypeMapping) {
        if (file.getShader()->getType() == SHADER_POSTPROCESSING) {
            auto typeEnum = t.second.type;
            if (typeEnum == ShaderOpenGLCustomDataType::DIFFUSE ||
                typeEnum == ShaderOpenGLCustomDataType::SPECULAR) {
                continue;
                }
        }

        items.push_back(t.second.label);
    }

    return items;
}

void ShadersGUI::DrawShaderConfigVarsTable(EditableOpenShaderFile &file)
{
    ImGui::SeparatorText("Shader uniforms");

    if (file.getShader()->dataTypes.empty()) {
        Drawable::WarningMessage("No variables defined");
        return;
    }

    // Calcular el ancho del botón
    float buttonWidth = GUIType::Sizes::ICONS_BROWSERS.x + ImGui::GetStyle().FramePadding.x * 2;

    // Calcular el ancho máximo de los labels (solo los que tienen controles)
    float maxLabelWidth = 0.0f;
    for (int i = 0; i < file.getShader()->dataTypes.size(); i++) {
        auto type = &file.getShader()->dataTypes[i];

        // Verificar si el tipo es interno
        auto dataType = GLSLTypeMapping[type->type].type;
        bool isInternal = dataType == ShaderOpenGLCustomDataType::DELTA_TIME ||
                          dataType == ShaderOpenGLCustomDataType::EXECUTION_TIME ||
                          dataType == ShaderOpenGLCustomDataType::DEPTH ||
                          dataType == ShaderOpenGLCustomDataType::SCENE ||
                          dataType == ShaderOpenGLCustomDataType::TEXTURE2D ||
                          dataType == ShaderOpenGLCustomDataType::DIFFUSE ||
                          dataType == ShaderOpenGLCustomDataType::SPECULAR;

        // Solo calcular ancho si NO es interno
        if (!isInternal) {
            std::string label = std::to_string(i + 1) + ") " + type->name + " (" + GLSLTypeMapping[type->type].label + ")";
            float labelWidth = ImGui::CalcTextSize(label.c_str()).x;
            maxLabelWidth = ImMax(maxLabelWidth, labelWidth);
        }
    }

    // Ancho total de la parte izquierda: botón + spacing + texto + margen
    float leftSideWidth = buttonWidth + ImGui::GetStyle().ItemSpacing.x + maxLabelWidth + 30.0f;

    for (int i = 0; i < file.getShader()->dataTypes.size(); i++) {
        auto type = &file.getShader()->dataTypes[i];

        ImGui::PushID(i);

        ImGui::AlignTextToFramePadding();

        // Botón eliminar
        GUI::DrawButton("Remove shader variable", IconGUI::SHADER_REMOVE_VARIABLE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            file.getShader()->removeDataType(*type);
        });

        ImGui::SameLine();

        // Número y nombre
        ImGui::Text("%d) %s", i + 1, type->name.c_str());
        ImGui::SameLine();

        // Tipo
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "(%s)", GLSLTypeMapping[type->type].label.c_str());

        // Verificar si el tipo es interno (sin control editable)
        auto dataType = GLSLTypeMapping[type->type].type;
        bool isInternal = dataType == ShaderOpenGLCustomDataType::DELTA_TIME ||
                          dataType == ShaderOpenGLCustomDataType::EXECUTION_TIME ||
                          dataType == ShaderOpenGLCustomDataType::DEPTH ||
                          dataType == ShaderOpenGLCustomDataType::SCENE ||
                          dataType == ShaderOpenGLCustomDataType::TEXTURE2D ||
                          dataType == ShaderOpenGLCustomDataType::DIFFUSE ||
                          dataType == ShaderOpenGLCustomDataType::SPECULAR;

        if (!isInternal) {
            // Posicionar el control en la posición calculada
            ImGui::SameLine(leftSideWidth);

            // Control de valor ocupa el resto del espacio disponible
            ImGui::SetNextItemWidth(-FLT_MIN);
            ShaderBaseCustomOGLCode::DrawTypeImGuiControl(*type, true);
        }

        ImGui::PopID();
    }
}
void ShadersGUI::DrawShaderConfigEmptyStateWarning(EditableOpenShaderFile &file)
{
    auto types = file.getShader()->getDataTypes();

    if (types.empty()) {
        Drawable::WarningMessage("No variables defined");
    }
}

void ShadersGUI::DrawShaderConfigActionButtons(EditableOpenShaderFile &file)
{
    GUI::ImageButtonNormal(IconGUI::SHADER_SAVE, "Save uniforms config", [&] {
        file.getShader()->UpdateFileTypes();
    });
}

void ShadersGUI::DrawWinObjectShaders()
{
    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::OBJECT_SHADERS);
    if (!windowStatus->isOpen) return;

    auto o = Components::get()->Render()->getSelectedObject();
    if (o == nullptr) {
        Drawable::WarningMessage("No object selected");
        return;
    }

    auto mesh = dynamic_cast<Mesh3D*>(o);
    if (mesh == nullptr) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No Mesh3D object");
        return;
    }

    auto customShaders = mesh->getCustomShaders();
    if (customShaders.empty()) {
        Drawable::WarningMessage("No shaders in selected object");
        return;
    }

    for (unsigned int i = 0; i < customShaders.size(); i++) {
        auto s = customShaders[i];

        // Configurar CustomTreeNode
        CustomImGui::CustomTreeNodeConfig config(s->getLabel().c_str());

        config.leftIcon = FileSystemGUI::Icon(IconGUI::SHADER_FILE);
        config.iconSize = GUIType::Sizes::ICONS_OBJECTS_ALLOWED;

        // 🎯 Añadir bullets
        config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
        config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
        config.bulletSize = ImVec2(18, 18); // Opcional, ajusta el tamaño

        config.itemPadding = 1.0f;
        config.indentSpacing = 20.0f;
        config.defaultOpen = false;

        bool isNodeShader = s->getType() == SHADER_NODE_OBJECT || s->getType() == SHADER_NODE_POSTPROCESSING;

        // Lock/Unlock
        CustomImGui::TreeActionItem lockItem(
            !s->isEnabled() ? FileSystemGUI::Icon(IconGUI::SHADER_LOCK) : FileSystemGUI::Icon(IconGUI::SHADER_UNLOCK),
            !s->isEnabled() ? "Unlock shader object" : "Lock shader object",
            [s]() { s->setEnabled(!s->isEnabled()); }
        );
        lockItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(lockItem);

        // Reload
        CustomImGui::TreeActionItem reloadItem(
            FileSystemGUI::Icon(IconGUI::SHADER_RELOAD),
            "Reload shader in object",
            [s]() { s->Reload(); }
        );
        reloadItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(reloadItem);

        // Edit
        CustomImGui::TreeActionItem editItem(
            FileSystemGUI::Icon(isNodeShader ? IconGUI::WIN_SHADER_NODES : IconGUI::SHADER_EDIT),
            "Edit shader",
            [s]() { LoadDialogShader(s->getTypesFile()); }
        );
        editItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(editItem);

        // Remove
        CustomImGui::TreeActionItem removeItem(
            FileSystemGUI::Icon(IconGUI::SHADER_REMOVE),
            "Remove shader in object",
            [mesh, i]() { mesh->RemoveShader(i); }
        );
        removeItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(removeItem);

        // Dibujar el nodo
        bool isOpen = CustomImGui::CustomTreeNode(config);

        if (isOpen) {
            s->DrawImGuiProperties(mesh->getModelTextures()[0], mesh->getModelTextures()[0]);
            CustomImGui::CustomTreePop(config.indentSpacing);
        }
    }
}

void ShadersGUI::LoadDialogShader(const std::string &file)
{
    auto metaInfo = ExtractShaderCustomCodeMetainfo(file);
    auto type = ShaderBaseCustom::ExtractShaderTypeFromTypesFile(file);

    if (!Brakeza::get()->GUI()->isEditableFileAlreadyOpen(metaInfo.name)) {
        auto shader = ComponentRender::CreateCustomShaderFromDisk(metaInfo, nullptr);
        shader->PrepareSync();
        switch (type) {
            case SHADER_POSTPROCESSING: {
                Brakeza::get()->GUI()->OpenEditableFile(
                    new EditableOpenShaderFile(shader->getLabel(), metaInfo.vsFile, dynamic_cast<ShaderOGLCustomCodePostprocessing*>(shader))
                );
                break;
            }
            case SHADER_OBJECT: {
                Brakeza::get()->GUI()->OpenEditableFile(
                    new EditableOpenShaderFile(shader->getLabel(), metaInfo.vsFile, dynamic_cast<ShaderOGLCustomCodeMesh3D*>(shader))
                );
                break;
            }
            case SHADER_NODE_OBJECT: {
                Brakeza::get()->GUI()->OpenEditableFile(
                    new EditableOpenNode(shader->getLabel(), file, dynamic_cast<ShaderNodesMesh3D*>(shader))
                );
                break;
            }
            case SHADER_NODE_POSTPROCESSING: {
                Brakeza::get()->GUI()->OpenEditableFile(
                    new EditableOpenNode(shader->getLabel(),  file, dynamic_cast<ShaderNodesPostProcessing*>(shader))
                );
                break;
            }
            default:
                LOG_ERROR("[ShadersGUI] Unknow type shader...");
        }
    }
    auto codeEditorWindow = Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::CODE_EDITOR);
    if (!codeEditorWindow->isOpen) codeEditorWindow->isOpen = true;
    Brakeza::get()->GUI()->setIndexCodeEditorTab(metaInfo.name);
}

ShaderBaseCustomMetaInfo ShadersGUI::ExtractShaderCustomCodeMetainfo(const std::string &pathFile)
{
    auto json = cJSON_Parse(Tools::ReadFile(pathFile));

    auto getStr = [&](const char* key, const char* fallback = "") {
        auto item = cJSON_GetObjectItemCaseSensitive(json, key);
        return item && item->valuestring ? item->valuestring : fallback;
    };

    auto result = ShaderBaseCustomMetaInfo{
        getStr("name"),
        getStr("type"),
        getStr("vsFile"),
        getStr("fsFile"),
        getStr("typesFile")
    };

    cJSON_Delete(json); // importante: liberar memoria
    return result;
}

ShaderBaseCustom* ShadersGUI::CreateShaderBaseCustom(ShaderCustomType type, std::string folder, std::string file)
{
    auto name = Tools::getFilenameWithoutExtension(file);
    std::string typesFile = folder + std::string(name + ".json");

    switch(type) {
        case SHADER_POSTPROCESSING:
        case SHADER_OBJECT: {
            ShaderBaseCustomOGLCode::WriteEmptyCustomShaderToDisk(name, folder, type);
            auto metaInfo = ExtractShaderCustomCodeMetainfo(typesFile);
            auto shader = ComponentRender::CreateCustomShaderFromDisk(metaInfo, nullptr);
            return shader;
        }
        case SHADER_NODE_OBJECT: {
            auto nm = new ShaderNodeEditorManager(SHADER_NODE_OBJECT);
            ShaderBaseNodes::WriteEmptyCustomShaderToDisk(name, folder, type, nm);
            auto metaInfo = ExtractShaderCustomCodeMetainfo(typesFile);
            auto shader = ComponentRender::CreateCustomShaderFromDisk(metaInfo, nullptr);
            return shader;
        }
        case SHADER_NODE_POSTPROCESSING: {
            auto nm = new ShaderNodeEditorManager(SHADER_NODE_POSTPROCESSING);
            ShaderBaseNodes::WriteEmptyCustomShaderToDisk(name, folder, type, nm);
            auto metaInfo = ExtractShaderCustomCodeMetainfo(typesFile);
            auto shader = ComponentRender::CreateCustomShaderFromDisk(metaInfo, nullptr);
            return shader;
        }
        case SHADER_NONE:
        default:
            return nullptr;
    }
}