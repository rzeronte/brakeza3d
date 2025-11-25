
#include "../include/GUI/GUIManager.h"
#include "../../include/Render/Drawable.h"
#include "../include/Brakeza3D.h"

GUIManager::GUIManager(std::vector<Object3D *> &gameObjects)
:
    gameObjects(gameObjects),
    widgetConsole(new ImGuiConsoleApp(ComponentsManager::get()->getComponentScripting()->getLua())),
    widgetObjects3D(new GUIAddonObjects3D(icons, this->gameObjects)),
    widgetObject3DProperties(new GUIAddonObject3DProperties(icons, this->gameObjects, scriptEditableManager)),
    widgetProjectSettings(new GUIAddonProjectSettings(icons, scriptEditableManager)),
    widgetMenu(new GUIAddonMenu(icons)),
    widgetToolbar(new GUIAddonToolbar(icons)),
    currentScriptsFolderWidget(BrakezaSetup::get()->SCRIPTS_FOLDER),
    currentScenesFolderWidget(BrakezaSetup::get()->SCENES_FOLDER),
    currentProjectsFolderWidget(BrakezaSetup::get()->PROJECTS_FOLDER),
    currentShadersFolderWidget(BrakezaSetup::get()->CUSTOM_SHADERS_FOLDER)
{
    LoadIcons(icons);
    loadImagesFolder();
    currentScriptsFolders = Tools::getFolderFolders(currentScriptsFolderWidget);
    currentScriptsFolderFiles = Tools::getFolderFiles(currentScriptsFolderWidget, "lua");

    currentScenesFolders = Tools::getFolderFolders(currentScenesFolderWidget);
    currentScenesFolderFiles = Tools::getFolderFiles(currentScenesFolderWidget, "json");

    currentProjectsFolders = Tools::getFolderFolders(currentProjectsFolderWidget);
    currentProjectsFolderFiles = Tools::getFolderFiles(currentProjectsFolderWidget, "json");

    currentShadersFolders = Tools::getFolderFolders(currentShadersFolderWidget);
    currentShadersFolderFiles = Tools::getFolderFiles(currentShadersFolderWidget, "json");
}

void GUIManager::loadImagesFolder()
{
    auto images = Tools::getFolderFiles(BrakezaSetup::get()->IMAGES_FOLDER, "png");

    for (const auto& f: images) {
        imagesFolder.addItem(BrakezaSetup::get()->IMAGES_FOLDER + f, f);
    }
}

void GUIManager::drawSelectedObjectShaders()
{
    bool hasSelectedIndex = selectedObjectIndex >= 0 && selectedObjectIndex < gameObjects.size();

    if (!hasSelectedIndex) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No object selected");
        return;
    }

    auto mesh = dynamic_cast<Mesh3D*>(gameObjects[selectedObjectIndex]);

    if (mesh == nullptr) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No Mesh3D object");
        return;
    }

    auto customShaders = mesh->getCustomShaders();
    if (customShaders.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No shaders in selected object.");
    }

    for (unsigned int i = 0; i < customShaders.size(); i++) {
        ImGui::PushID(i);

        auto s = customShaders[i];

        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "shaderIcon"), ImVec2(14, 14))) {
            auto folder = s->getFolder();
            auto jsonFilename = s->getLabel() + ".json";
            LoadShaderDialog(folder, jsonFilename);
        }

        ImGui::SameLine();

        if (!s->isEnabled()) {
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "unlockIcon"), ImVec2(14, 14))) {
                s->setEnabled(true);
            }
        } else {
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "lockIcon"), ImVec2(14, 14))) {
                s->setEnabled(false);
            }
        }
        ImGui::SameLine();
        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "rebuildIcon"), ImVec2(14, 14))) {
            s->reload();
        }
        ImGui::SameLine();
        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "removeIcon"), ImVec2(14, 14))) {
            mesh->removeShader(i);
        }

        ImGui::SameLine();

        if (ImGui::CollapsingHeader(s->getLabel().c_str())) {
            s->drawImGuiProperties(mesh->getModelTextures()[0], mesh->getModelTextures()[0]);
        }
        ImGui::PopID();
    }
}

void GUIManager::LoadScriptDialog(const std::string& filename)
{
    delete scriptEditableManager.script;
    scriptEditableManager.selectedScriptFilename = filename;
    scriptEditableManager.script = new ScriptLUA(filename,ScriptLUA::dataTypesFileFor(filename));
    strcpy(scriptEditableManager.editableSource, scriptEditableManager.script->content.c_str());
    showEditScriptWindow = true;
}

void GUIManager::drawSelectedObjectScripts()
{
    bool hasSelectedIndex = selectedObjectIndex >= 0 && selectedObjectIndex < gameObjects.size();

    if (!hasSelectedIndex) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No object selected");
        return;
    }

    auto o = gameObjects[selectedObjectIndex];
    auto objectScripts = o->getScripts();

    if (objectScripts.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No scripts in selected object.");
    }

    for (unsigned int i = 0; i < objectScripts.size(); i++) {
        auto currentScript = objectScripts[i];
        ImGui::PushID(i);

        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "scriptIcon"), ImVec2(14, 14))) {
            LoadScriptDialog(currentScript->scriptFilename);
        }

        ImGui::SameLine();

        std::string optionText = std::to_string(i + 1) + ") " + Tools::removeSubstring(currentScript->scriptFilename, BrakezaSetup::get()->SCRIPTS_FOLDER);

        if (currentScript->isPaused()) {
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "unlockIcon"), ImVec2(14, 14))) {
                currentScript->setPaused(false);
            }
        } else {
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "lockIcon"), ImVec2(14, 14))) {
                currentScript->setPaused(true);
            }
        }
        ImGui::SameLine();
        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "removeIcon"), ImVec2(14, 14))) {
            o->removeScript(currentScript);
        }
        ImGui::SameLine();
        if (ImGui::CollapsingHeader(optionText.c_str())) {
            currentScript->drawImGuiProperties();
        }

        ImGui::PopID();
    }
}

void GUIManager::DrawEditShaderWindow()
{
    if (!showEditShaderWindow) return;

    setNextWindowSize(600, 600);
    ImGui::SetNextWindowBgAlpha(0.9f);

    if (ImGui::Begin("Shader edition", &showEditShaderWindow, ImGuiWindowFlags_NoDocking)) {
        drawShaderVariables();

        ImGui::SeparatorText("GLSL code");

        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;

        if (ImGui::CollapsingHeader("Vertex shader code")) {
            ImGui::InputTextMultiline("##sourceVS", shaderEditableManager.editableSourceVS, IM_ARRAYSIZE(shaderEditableManager.editableSourceVS), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10), flags);
            if (ImGui::Button(std::string("Save vertex shader").c_str())) {
                shaderEditableManager.shader->sourceVS = shaderEditableManager.editableSourceVS;
                Tools::writeToFile(shaderEditableManager.shader->getVertexFilename(), shaderEditableManager.shader->sourceVS.c_str());
            }
        }

        if (ImGui::CollapsingHeader("Fragment shader code")) {
            ImGui::InputTextMultiline("##sourceFS", shaderEditableManager.editableSourceFS, IM_ARRAYSIZE(shaderEditableManager.editableSourceFS), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10), flags);
            if (ImGui::Button(std::string("Save fragment shader").c_str())) {
                shaderEditableManager.shader->sourceFS = shaderEditableManager.editableSourceFS;
                Tools::writeToFile(shaderEditableManager.shader->getFragmentFilename(), shaderEditableManager.shader->sourceFS.c_str());
            }
        }
    }
    ImGui::End();
}

void GUIManager::DrawEditScriptWindow()
{
    if (!showEditScriptWindow) return;

    setNextWindowSize(600, 600);
    ImGui::SetNextWindowBgAlpha(0.9f);

    if (ImGui::Begin("Script edition", &showEditScriptWindow, ImGuiWindowFlags_NoDocking)) {
        drawScriptVariables();

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Script code")) {
            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            ImGui::InputTextMultiline("##source", scriptEditableManager.editableSource, IM_ARRAYSIZE(scriptEditableManager.editableSource), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 25), flags);
            if (ImGui::Button(std::string("Save").c_str())) {
                scriptEditableManager.script->updateScriptCodeWith(scriptEditableManager.editableSource);
                scriptEditableManager.script->reloadScriptCode();
            }
        }
    }
    ImGui::End();
}

void GUIManager::DrawLightsDepthMapsViewerWindow()
{
    if (!showLightsDepthMapsViewerWindow) return;

    setNextWindowSize(350, 400);
    ImGui::SetNextWindowBgAlpha(0.9f);

    auto title = std::string("Lights Depth Maps Viewer: ");
    if (ImGui::Begin(title.c_str(), &showLightsDepthMapsViewerWindow, ImGuiWindowFlags_NoDocking)) {

        auto render = ComponentsManager::get()->getComponentRender();
        auto shaderShadowPassDebugLight = render->getShaderOGLShadowPassDebugLight();
        auto lights = render->getShaderOGLRenderForward()->getShadowMappingSpotLights();

        const int columns = 1;

        // Añadir padding a las celdas
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 10.0f));

        if (ImGui::BeginTable("DepthMapsTable", columns, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame)) {
            float imageSize = 200.0f;

            // Añadir DirectionalLight primero
            ImGui::TableNextColumn();

            auto combinedText = std::string("DirectionalLight");
            float availWidth = ImGui::GetContentRegionAvail().x;
            float textWidth = ImGui::CalcTextSize(combinedText.c_str()).x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - textWidth) * 0.5f);
            ImGui::Text("%s", combinedText.c_str());

            // Centrar imagen
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - imageSize) * 0.5f);
            ImGui::Image(reinterpret_cast<ImTextureID>(shaderShadowPassDebugLight->getSceneTexture()), ImVec2(imageSize, imageSize));

            // Luego añadir los SpotLights
            int i = 0;
            for (const auto l: lights) {
                ImGui::TableNextColumn();

                // Centrar texto combinado
                auto combinedText = l->getLabel() + " / Layer: " + std::to_string(i);
                float availWidth = ImGui::GetContentRegionAvail().x;
                float textWidth = ImGui::CalcTextSize(combinedText.c_str()).x;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - textWidth) * 0.5f);
                ImGui::Text("%s", combinedText.c_str());

                // Centrar imagen
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - imageSize) * 0.5f);
                ImGui::Image(reinterpret_cast<ImTextureID>(shaderShadowPassDebugLight->getInternalTexture(i)), ImVec2(imageSize, imageSize));

                i++;
            }

            ImGui::EndTable();
        }

        ImGui::PopStyleVar();
    }

    ImGui::End();
}

void GUIManager::DrawEditBonesMappingWindow()
{
    if (selectedObjectIndex < 0) return;
    if (!showBoneMappingsEditorWindow) return;

    auto a = dynamic_cast<Mesh3DAnimation*>(gameObjects[selectedObjectIndex]);

    if (a == nullptr) return;

    auto bc = a->getBoneMappingColliders();
    int numMappings = bc->size();

    setNextWindowSize(700, 700);
    ImGui::SetNextWindowBgAlpha(0.9f);
    auto dialogTitle = std::string("Bones Mapping Editor: " + a->getLabel());
    if (ImGui::Begin(dialogTitle.c_str(), &showBoneMappingsEditorWindow, ImGuiWindowFlags_NoDocking)) {
        ImGui::SeparatorText("Create new bones mapping:");

        static char name[256];
        strncpy(name, currentVariableToAddName.c_str(), sizeof(name));
        if (ImGui::InputText("Mapping name", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
            currentVariableToAddName = name;
        }
        ImGui::SameLine();
        if (ImGui::Button(std::string("Add mapping").c_str())) {
            if (!currentVariableToAddName.empty()) {
                a->createBonesMappingColliders(currentVariableToAddName);
                currentVariableToAddName.clear();
            }
        }
        if (numMappings <= 0) return;

        ImGui::SeparatorText("Bone mappings created");

        if (numMappings > 0) {
            const char* items[numMappings];
            for (unsigned int i = 0; i < bc->size(); i++) {
                items[i] = bc->at(i).nameMapping.c_str();
            }

            auto comboTitle = "BoneMappings##" + a->getLabel();
            ImGui::Combo("Bone Mappings", &a->BoneColliderIndexPointer(), items, IM_ARRAYSIZE(items));

            if (a->BoneColliderIndexPointer() >= 0) {
                if (ImGui::Button(std::string("Delete mapping").c_str())) {
                    a->removeBonesColliderMapping(bc->at(a->BoneColliderIndexPointer()).nameMapping);
                    return;
                }
            }
        }

        if (a->BoneColliderIndexPointer() != -1) {
            ImGui::SeparatorText(std::string("List bones for mapping: " + bc->at(a->BoneColliderIndexPointer()).nameMapping).c_str());

            auto nameMapping = bc->at(a->BoneColliderIndexPointer()).nameMapping;
            if (ImGui::BeginTable("BoneMappingTable", 4, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Index");
                ImGui::TableSetupColumn("Bone Name");
                ImGui::TableSetupColumn("Bone ID");
                ImGui::TableSetupColumn("Enabled");
                ImGui::TableHeadersRow();

                for (int i = 0; i < bc->at(a->BoneColliderIndexPointer()).boneColliderInfo.size(); i++) {
                    auto& b = bc->at(a->BoneColliderIndexPointer()).boneColliderInfo[i];

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%d", i + 1);

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", b.name.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%d", b.boneId);

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::ImageButton((std::string((b.enabled ? "lock##" : "unlock##")) + std::to_string(i)).c_str(),
                                           TexturePackage::getOGLTextureID(icons, b.enabled ? "lockIcon" : "unlockIcon"),
                                           ImVec2(14, 14))) {
                        a->SetMappingBoneColliderInfo(nameMapping, b.boneId, !b.enabled, b.shape);
                    }
                }
                ImGui::EndTable();
            }
        }
    }

    ImGui::End();
}

void GUIManager::drawScriptsLuaFolderFiles(const std::string& folder)
{
    static char name[256];
    strncpy(name, currentVariableToAddName.c_str(), sizeof(name));
    if (ImGui::InputText("Script name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        currentVariableToAddName = name;
    }
    if (ImGui::Button(std::string("Create script").c_str())) {
        if (!currentVariableToAddName.empty()) {
            ComponentScripting::createScriptLUAFile(currentScriptsFolderWidget + currentVariableToAddName);
            currentScriptsFolderFiles = Tools::getFolderFiles(currentScriptsFolderWidget, "lua");
        }
    }

    ImGui::Separator();

    drawBrowserFolders(folder, BrakezaSetup::get()->SCRIPTS_FOLDER, currentScriptsFolderWidget, currentScriptsFolders, currentScriptsFolderFiles, "lua");

    ImGui::Separator();

    auto files= currentScriptsFolderFiles;
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("ScriptsFolderTable", 2, flags)) {
        for (int i = 0; i < files.size(); i++) {
            ImGui::PushID(i);
            const auto& file = files[i];
            auto fullPath = folder + file;
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Image(TexturePackage::getOGLTextureID(icons, "scriptIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            std::string optionText = std::to_string(i + 1) + ") " + file;
            if (ImGui::Selectable(optionText.c_str())) {
                LoadScriptDialog(fullPath);
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("SCRIPT_ITEM",fullPath.c_str(),fullPath.size() + 1);
                ImGui::Text("%s", fullPath.c_str());
                ImGui::EndDragDropSource();
            }
            ImGui::TableSetColumnIndex(1);
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "removeIcon"), ImVec2(14, 14))) {
                ImGui::OpenPopup("Delete Script?");
            }
            ShowDeletePopup("Delete Script?", [folder, file, this] () {
                ComponentScripting::removeScriptLUAFile(folder + file);
                updateFolderFiles();
            });
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void GUIManager::updateFolderFiles()
{
    currentScriptsFolderFiles = Tools::getFolderFiles(currentScriptsFolderWidget, "lua");
    currentScenesFolderFiles = Tools::getFolderFiles(currentScenesFolderWidget, "json");
    currentProjectsFolderFiles = Tools::getFolderFiles(currentProjectsFolderWidget, "json");
    currentShadersFolderFiles = Tools::getFolderFiles(currentShadersFolderWidget, "json");
}

void GUIManager::drawShaderVariables()
{
    auto type = shaderEditableManager.shader->getType();
    auto typeName = shaderEditableManager.shader->getShaderTypeString(type);

    ImGui::SeparatorText((shaderEditableManager.shader->getLabel() + ": " + typeName).c_str());

    static char name[256];
    strncpy(name, currentVariableToAddName.c_str(), sizeof(name));

    if (ImGui::InputText("Variable name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        currentVariableToAddName = name;
    }

    std::vector<std::string> items;
    for (auto t: GLSLTypeMapping) {
        items.push_back(t.first);
    }

    if (shaderEditableManager.shader->getType() == ShaderCustomTypes::SHADER_POSTPROCESSING) {
        items.erase(std::remove_if(items.begin(), items.end(), [](const std::string& item) {
            auto typeEnum = GLSLTypeMapping[item];
            return typeEnum == ShaderOpenGLCustomDataType::DIFFUSE || typeEnum == ShaderOpenGLCustomDataType::SPECULAR;
        }), items.end());
    }

    std::vector<const char*> itemsCStr;
    for (auto& item : items) itemsCStr.push_back(item.c_str());

    static int selectedItem = 0;
    ImGui::Combo("Type##1", &selectedItem, itemsCStr.data(), itemsCStr.size());
    ImGui::SameLine();
    if (ImGui::Button(std::string("Create variable").c_str())) {
        if (!currentVariableToAddName.empty()) {
            shaderEditableManager.shader->addDataTypeEmpty(currentVariableToAddName.c_str(), itemsCStr[selectedItem]);
        }
    }

    ImGui::SeparatorText("Variables");

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    auto types = shaderEditableManager.shader->getDataTypes();
    if (ImGui::BeginTable("ScriptProperties", 3, flags)) {
        for (int i = 0; i < types.size(); i++) {
            ImGui::PushID(i);
            ImGui::TableNextRow();
            auto type = &types[i];

            ImGui::TableSetColumnIndex(0);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%d) %s", i + 1, type->name.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%s", type->type.c_str());

            ImGui::TableSetColumnIndex(2);
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "removeIcon"), ImVec2(14, 14))) {
                shaderEditableManager.shader->removeDataType(*type);
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    if (types.empty()) {
        ImGui::Text("No types defined");
        ImGui::Spacing();
    }

    if (ImGui::Button(std::string("Save shader types").c_str())) {
        shaderEditableManager.shader->updateFileTypes();
    }
}

void GUIManager::drawScriptVariables()
{
    ImGui::SeparatorText(std::string("File: " + scriptEditableManager.script->scriptFilename).c_str());

    static char name[256];

    strncpy(name, currentVariableToAddName.c_str(), sizeof(name));

    if (ImGui::InputText("Variable name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        currentVariableToAddName = name;
    }

    const char* items[] = { "int", "float", "string", "Vertex3D" };
    static int selectedItem = 0;
    ImGui::Combo("Type", &selectedItem, items, IM_ARRAYSIZE(items));
    ImGui::SameLine();
    if (ImGui::Button(std::string("Create variable").c_str())) {
        if (!currentVariableToAddName.empty()) {
            scriptEditableManager.script->addDataTypeEmpty(currentVariableToAddName.c_str(), items[selectedItem]);
        }
    }

    ImGui::SeparatorText("Variables");

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("ScriptProperties", 3, flags)) {
        for (int i = 0; i < scriptEditableManager.script->dataTypes.size(); i++) {
            ImGui::TableNextRow();
            auto type = &scriptEditableManager.script->dataTypes[i];

            ImGui::TableSetColumnIndex(0);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%d) %s", i + 1, type->name.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("%s", type->type.c_str());

            ImGui::TableSetColumnIndex(2);
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "removeIcon"), ImVec2(14, 14))) {
                scriptEditableManager.script->removeDataType(*type);
                scriptEditableManager.script->updateFileTypes();
            }
        }
        ImGui::EndTable();
    }

    if (scriptEditableManager.script->dataTypes.empty()) {
        ImGui::Text("No types defined");
        ImGui::Spacing();
    } else {
        if (ImGui::Button(std::string("Save script types").c_str())) {
            scriptEditableManager.script->updateFileTypes();
        }
    }
}

void GUIManager::drawProjectsFiles(const std::string& folder)
{
    static char name[256];
    strncpy(name, currentVariableToCreateCustomShader.c_str(), sizeof(name));
    if (ImGui::InputText("Project name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        currentVariableToCreateCustomShader = name;
    }
    if (ImGui::Button(std::string("Create Project").c_str())) {
        if (!currentVariableToCreateCustomShader.empty()) {
            ProjectLoader::createProject(folder + currentVariableToCreateCustomShader);
            currentProjectsFolderFiles = Tools::getFolderFiles(currentProjectsFolderWidget, "json");
        }
    }

    ImGui::Separator();

    std::vector<std::string> files = currentProjectsFolderFiles;
    std::sort(files.begin(), files.end() );
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("ProjectsFolderTable", 2, flags)) {
        for (int i = 0; i < files.size(); i++) {
            ImGui::TableNextRow();
            const auto& file = files[i];
            auto title = std::to_string(i + 1) + ") " + file;
            ImGui::PushID(i);

            if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                ImGui::TableSetColumnIndex(0);
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 4, ImGui::GetCursorPosY() + 5.0f));
                ImGui::Image(TexturePackage::getOGLTextureID(icons, "projectIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY()));
                ImGui::Text("%s", title.c_str());

                ImGui::TableSetColumnIndex(1);
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "openIcon"), ImVec2(14, 14))) {
                    ComponentsManager::get()->getComponentRender()->getProjectLoader().loadProject(folder + file);
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "saveIcon"), ImVec2(14, 14))) {
                    ProjectLoader::saveProject(folder + file);
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "removeIcon"), ImVec2(14, 14))) {
                    ImGui::OpenPopup("Delete Project?");
                }
                ShowDeletePopup("Delete Project?", [folder, file, this] () {
                    ProjectLoader::removeProject(folder + file);
                    updateFolderFiles();
                });

            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void GUIManager::drawScenesFolder(const std::string& folder)
{
    static char name[256];
    strncpy(name, currentVariableToCreateCustomShader.c_str(), sizeof(name));
    if (ImGui::InputText("Scene name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        currentVariableToCreateCustomShader = name;
    }
    if (ImGui::Button(std::string("Create Scene").c_str())) {
        if (!currentVariableToCreateCustomShader.empty()) {
            SceneLoader::CreateScene(folder + currentVariableToCreateCustomShader);
            currentScenesFolderFiles = Tools::getFolderFiles(currentScenesFolderWidget, "json");
        }
    }

    ImGui::Separator();
    drawBrowserFolders(folder, BrakezaSetup::get()->SCENES_FOLDER, currentScenesFolderWidget, currentScenesFolders, currentScenesFolderFiles, "json");
    ImGui::Separator();

    auto files = currentScenesFolderFiles;
    std::sort(files.begin(), files.end() );
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("ScenesFolderTable", 2, flags)) {
        for (int i = 0; i < files.size(); i++) {
            ImGui::TableNextRow();
            const auto& file = files[i];
            auto fullPath = folder + file;
            ImGui::PushID(i);

            auto title = std::to_string(i + 1) + ") " + file;
            if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                ImGui::TableSetColumnIndex(0);
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 4, ImGui::GetCursorPosY() + 5.0f));
                ImGui::Image(TexturePackage::getOGLTextureID(icons, "sceneIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY()));
                ImGui::Text("%s", title.c_str());

                ImGui::TableSetColumnIndex(1);
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "openIcon"), ImVec2(14, 14))) {
                    SceneLoader::ClearScene();
                    ComponentsManager::get()->getComponentRender()->getSceneLoader().LoadScene(folder + file);
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "saveIcon"), ImVec2(14, 14))) {
                    SceneLoader::SaveScene(folder + file);
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "removeIcon"), ImVec2(14, 14))) {
                    ImGui::OpenPopup("Delete Scene?");
                }
                ShowDeletePopup("Delete Scene?", [folder, file, this] () {
                    SceneLoader::RemoveScene(folder + file);
                    updateFolderFiles();
                });
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void GUIManager::drawBrowserFolders(
    const std::string& folder,
    const std::string& baseFolder,
    std::string& destiny,
    std::vector<std::string> &folders,
    std::vector<std::string> &files,
    const std::string& extension
)
{
    ImGui::Text("Current: %s", folder.c_str());

    ImGui::Separator();

    if (folder != baseFolder) {
        ImGui::Image(TexturePackage::getOGLTextureID(icons, "folderIcon"), ImVec2(16, 16));
        ImGui::SameLine();
        if (ImGui::Button("..")) {
            destiny = Tools::GoBackFromFolder(destiny);
            folders = Tools::getFolderFolders(destiny);
            files = Tools::getFolderFiles(destiny, extension);
        }
    }

    ImGui::Image(TexturePackage::getOGLTextureID(icons, "folderIcon"), ImVec2(16, 16));
    ImGui::SameLine();
    ImGui::Text(".");

    for (const auto & i : folders) {
        auto fullPathFolder = folder + i;
        ImGui::Image(TexturePackage::getOGLTextureID(icons, "folderIcon"), ImVec2(16, 16));
        ImGui::SameLine();
        if (ImGui::Button(i.c_str())) {
            destiny = fullPathFolder + "/";
            folders = Tools::getFolderFolders(destiny);
            files = Tools::getFolderFiles(destiny, extension);
        }
    }
}

void GUIManager::LoadShaderDialog(const std::string &folder, std::string &file)
{
    auto shader = ComponentsManager::get()->getComponentRender()->getLoadedShader(folder, file);
    delete shaderEditableManager.shader;
    shaderEditableManager.shader = shader;
    shaderEditableManager.loaded = true;
    strcpy(shaderEditableManager.editableSourceVS, shader->sourceVS.c_str());
    strcpy(shaderEditableManager.editableSourceFS, shader->sourceFS.c_str());
    showEditShaderWindow = true;
}

void GUIManager::drawCustomShadersFolder(std::string folder)
{
    static char name[256];
    strncpy(name, currentVariableToCreateCustomShader.c_str(), sizeof(name));
    if (ImGui::InputText("Shader name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        currentVariableToCreateCustomShader = name;
    }

    std::vector<const char*> items;
    for (const auto& pair : ComponentsManager::get()->getComponentRender()->getShaderTypesMapping()) {
        items.push_back(pair.first.c_str());
    }

    static int item_current_idx = 0;

    if (ImGui::Combo("Type", &item_current_idx, items.data(), items.size())) {
        std::cout << "Seleccionado: " << items[item_current_idx] << std::endl;
    }

    if (ImGui::Button(std::string("Create custom shader").c_str())) {
        if (!currentVariableToCreateCustomShader.empty()) {
            auto type = ShaderOGLCustom::getShaderTypeFromString(items[item_current_idx]);
            ShaderOGLCustom::createEmptyCustomShader(currentVariableToCreateCustomShader, folder, type);
            updateFolderFiles();
        }
    }

    ImGui::Separator();

    drawBrowserFolders(folder, BrakezaSetup::get()->CUSTOM_SHADERS_FOLDER, currentShadersFolderWidget, currentShadersFolders, currentShadersFolderFiles, "json");

    ImGui::Separator();

    auto files= currentShadersFolderFiles;
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("ScriptsFolderTable", 3, flags)) {
        for (int i = 0; i < files.size(); i++) {
            ImGui::PushID(i);
            auto file = files[i];
            auto fullPath = folder + file;
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
            ImGui::Image(TexturePackage::getOGLTextureID(icons, "shaderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            std::string optionText = std::to_string(i + 1) + ") " + file;
            if (ImGui::Selectable(optionText.c_str())) {
                LoadShaderDialog(folder, file);
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                static std::string folderCopy, fileCopy;
                folderCopy = folder;
                fileCopy = file;
                BrakezaSetup::DragDropCustomShaderData data = {folderCopy.c_str(), fileCopy.c_str()};
                ImGui::SetDragDropPayload("CUSTOMSHADER_ITEM", &data,sizeof(data));
                ImGui::Text("%s", fullPath.c_str());
                ImGui::EndDragDropSource();
            }
            ImGui::TableSetColumnIndex(1);
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "sceneIcon"), ImVec2(14, 14))) {
                ComponentsManager::get()->getComponentRender()->loadShaderIntoScene(folder, file);
            }
            ImGui::TableSetColumnIndex(2);
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "removeIcon"), ImVec2(14, 14))) {
                ImGui::OpenPopup("Delete shaders?");
            }
            ShowDeletePopup("Delete shaders?", [folder, file, this] () {
                ShaderOGLCustom::removeCustomShaderFiles(folder,  Tools::getFilenameWithoutExtension(file));
                updateFolderFiles();
            });

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void GUIManager::DrawWidgets()
{
    if (ImGui::Begin("Object shaders")) {
        drawSelectedObjectShaders();
    }
    ImGui::End();

    if (ImGui::Begin("Object Scripts")) {
        drawSelectedObjectScripts();
    }
    ImGui::End();

    if (ImGui::Begin("Scripts")) {
        drawScriptsLuaFolderFiles(currentScriptsFolderWidget);
    }
    ImGui::End();

    if (ImGui::Begin("Object variables")) {
        DrawObjectVariables();
    }
    ImGui::End();

    if (ImGui::Begin("Global variables")) {
        drawGlobalVariables();
    }
    ImGui::End();

    widgetProjectSettings->draw();

    if (ImGui::Begin("Keyboard/Mouse")) {
        drawKeyboardMouseSettings();
    }
    ImGui::End();

    if (ImGui::Begin("Shaders")) {
        drawCustomShadersFolder(currentShadersFolderWidget);
    }
    ImGui::End();

    if (ImGui::Begin("Projects")) {
        drawProjectsFiles(currentProjectsFolderWidget);
    }
    ImGui::End();

    if (ImGui::Begin("Scenes")) {
        drawScenesFolder(currentScenesFolderWidget);
    }
    ImGui::End();

    if (ImGui::Begin("Images")) {
        drawImages();
    }
    ImGui::End();
}

void GUIManager::DrawObjectSelectedGuizmoOperation() {
    auto render = ComponentsManager::get()->getComponentRender();

    if (render->getSelectedObject() != nullptr) {
        auto o = render->getSelectedObject();

        auto camera = ComponentsManager::get()->getComponentCamera();
        Drawable::drawObject3DGizmo(
            o,
            o->getModelMatrix(),
            camera->getGLMMat4ViewMatrix(),
            camera->getGLMMat4ProjectionMatrix()
        );
    }
}

void GUIManager::updateImGuiDocking() {
    //bool show_demo_window = true;
    //ImGui::ShowDemoWindow(&show_demo_window);
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each other.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &opt_fullscreen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
}

void GUIManager::DrawGUIPlugins(bool &finish)
{
    bool p_open = true;
    widgetConsole->Draw("Logging/Console", &p_open);
    widgetObjects3D->Draw(selectedObjectIndex);
    widgetObject3DProperties->Draw(selectedObjectIndex);
    widgetMenu->Draw(finish, showAboutWindow, showLightsDepthMapsViewerWindow);
    widgetToolbar->Draw();
}

void GUIManager::draw(float timedelta, bool &finish)
{
    updateImGuiDocking();

    DrawWidgets();
    DrawGUIPlugins(finish);
    DrawObjectSelectedGuizmoOperation();
    DrawEditShaderWindow();
    DrawEditScriptWindow();
    DrawEditBonesMappingWindow();
    DrawLightsDepthMapsViewerWindow();
    DrawSplash();

    RenderFPS();

    ImGui::End();
}

void GUIManager::RenderFPS()
{
    if (BrakezaSetup::get()->DRAW_FPS_IMGUI) {
        auto fps = ComponentsManager::get()->getComponentRender()->getFps();
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        char fpsText[32];
        snprintf(fpsText, sizeof(fpsText), "%d", fps);
        ImVec2 textSize = ImGui::CalcTextSize(fpsText);
        ImVec2 textPos = ImVec2((screenSize.x - textSize.x) * 0.5f, (screenSize.y - textSize.y) * 0.5f);

        ImGui::SetNextWindowBgAlpha(0.5f); // transparent
        ImGui::SetNextWindowPos(textPos, ImGuiCond_Always);
        ImGui::Begin("FPS Overlay", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNav);
        ImGui::TextUnformatted(fpsText);
        ImGui::End();
    }
}

ImGuiConsoleApp *GUIManager::getConsole()
{
    return widgetConsole;
}

TexturePackage *GUIManager::getImGuiTextures()
{
    return &icons;
}

bool GUIManager::isShowLightsDepthMapsViewerWindow() const
{
    return showLightsDepthMapsViewerWindow;
}

void GUIManager::setSelectedObjectIndex(int selectedObjectIndex)
{
    GUIManager::selectedObjectIndex = selectedObjectIndex;
}

void GUIManager::setSelectedObject(Object3D *s)
{
    int i = 0;
    for (auto o: gameObjects) {
        if (s == o) {
            setSelectedObjectIndex(i);
        }
        i++;
    }
}

void GUIManager::ShowDeletePopup(const char* title, const std::function<void()>& onConfirm)
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure to delete !?!?");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            onConfirm();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void GUIManager::DrawObjectVariables() const
{
    bool hasSelectedIndex = selectedObjectIndex >= 0 && selectedObjectIndex < gameObjects.size();

    if (!hasSelectedIndex) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No object selected");
    }

    if (hasSelectedIndex) {
        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
        if (ImGui::BeginTable("ObjectVariablesTable", 3, flags)) {
            auto o = gameObjects[selectedObjectIndex];
            auto scripts = o->getScripts();
            auto &luaEnvironment = o->getLuaEnvironment();
            auto &lua = ComponentsManager::get()->getComponentScripting()->getLua();

            int numVarFound = 0;
            for (auto &pair : luaEnvironment) {
                std::string key = pair.first.as<std::string>(); // Nombre de la variable
                sol::type valueType = pair.second.get_type();   // Tipo de la variable

                auto type = std::string(sol::type_name(lua, valueType));

                if (type == "number" || type == "string" || type == "boolean") {
                    numVarFound++;
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", std::string(key).c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", std::string(sol::type_name(lua, valueType)).c_str());

                    ImGui::TableSetColumnIndex(2);
                    if (type == "number") {
                        ImGui::Text("%f", static_cast<float>(luaEnvironment[key]));
                    } else if (type == "string") {
                        ImGui::Text("%s", std::string(luaEnvironment[key]).c_str());
                    } else if (type == "boolean") {
                        ImGui::Text("%d", static_cast<bool>(luaEnvironment[key]));
                    }
                }
            }
            ImGui::EndTable();
            if (numVarFound <= 0) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "The selected object has defined variables");
            }
        }
    }
}

void GUIManager::drawGlobalVariables()
{
    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("GlobalVariablesTable", 3, flags)) {
        auto &lua = ComponentsManager::get()->getComponentScripting()->getLua();

        sol::table globalTable = lua["_G"];  // Acceder a la tabla global

        int numVarFound = 0;
        for (auto &pair : globalTable) {
            std::string key = pair.first.as<std::string>(); // Nombre de la variable
            sol::type valueType = pair.second.get_type();   // Tipo de la variable
            //std::cout << "Variable: " << key << " | Tipo: " << sol::type_name(lua, valueType) << std::endl;

            auto type = std::string(sol::type_name(lua, valueType));

            if (type == "number" || type == "string" || type == "boolean") {
                numVarFound++;
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", std::string(key).c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", std::string(sol::type_name(lua, valueType)).c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", std::string(lua[key]).c_str());
            }
        }

        if (numVarFound <= 0) {
            ImGui::Text("%s", "There are not variables yet");
        }

        ImGui::EndTable();
    }
}

void GUIManager::drawKeyboardMouseSettings()
{
    auto input = ComponentsManager::get()->getComponentInput();

    ImGui::Text(("Mouse motion: " + std::to_string(input->isMouseMotion())).c_str());
    ImGui::Text(("Mouse motion RelX: " + std::to_string(input->getMouseMotionXRel())).c_str());
    ImGui::Text(("Mouse motion RelY: " + std::to_string(input->getMouseMotionYRel())).c_str());
    ImGui::Separator();

    ImGui::Text(("Click Left: " + std::to_string(input->isClickLeft())).c_str());
    ImGui::Text(("Click Right: " + std::to_string(input->isClickRight())).c_str());
    ImGui::Text(("Click Left pressed: " + std::to_string(input->isLeftMouseButtonPressed())).c_str());
    ImGui::Text(("Click Right pressed: " + std::to_string(input->isRightMouseButtonPressed())).c_str());
    ImGui::Text(("Mouse Up: " + std::to_string(input->isMouseButtonUp())).c_str());
    ImGui::Text(("Mouse Down: " + std::to_string(input->isMouseButtonDown())).c_str());
    ImGui::Text(("Mouse Drag: " + std::to_string(input->isDrag())).c_str());

    ImGui::Separator();
    ImGui::Text(("Key event Down: " + std::to_string(input->isKeyEventDown())).c_str());
    ImGui::Text(("Key event Up: " + std::to_string(input->isKeyEventUp())).c_str());
    ImGui::Separator();
    ImGui::Text(("Enabled: " + std::to_string(input->isEnabled())).c_str());
    ImGui::Separator();
    ImGui::Text(("Game Controller Enabled: " + std::to_string(input->isGameControllerEnabled())).c_str());
    ImGui::Text(("Controller Button A: " + std::to_string(input->getControllerButtonA())).c_str());
    ImGui::Text(("Controller Button B: " + std::to_string(input->getControllerButtonB())).c_str());
    ImGui::Text(("Controller Button X: " + std::to_string(input->getControllerButtonX())).c_str());
    ImGui::Text(("Controller Button Y: " + std::to_string(input->getControllerButtonY())).c_str());
    ImGui::Text(("Controller AxisLeft X: " + std::to_string(input->getControllerAxisLeftX())).c_str());
    ImGui::Text(("Controller AxisLeft Y: " + std::to_string(input->getControllerAxisLeftY())).c_str());
    ImGui::Text(("Controller AxisRight X: " + std::to_string(input->getControllerAxisRightX())).c_str());
    ImGui::Text(("Controller AxisRight Y: " + std::to_string(input->getControllerAxisRightY())).c_str());
    ImGui::Text(("Controller TriggerLeft: " + std::to_string(input->getControllerAxisTriggerLeft())).c_str());
    ImGui::Text(("Controller TriggerRight: " + std::to_string(input->getControllerAxisTriggerRight())).c_str());
    ImGui::Text(("Controller Pad Up: " + std::to_string(input->getControllerPadUp())).c_str());
    ImGui::Text(("Controller Pad Down: " + std::to_string(input->getControllerPadDown())).c_str());
    ImGui::Text(("Controller Pad Left: " + std::to_string(input->getControllerPadLeft())).c_str());
    ImGui::Text(("Controller Pad Right: " + std::to_string(input->getControllerPadRight())).c_str());
    ImGui::Text(("Controller Shoulder Left: " + std::to_string(input->getControllerShoulderLeft())).c_str());
    ImGui::Text(("Controller Shoulder Right: " + std::to_string(input->getControllerShoulderRight())).c_str());
    ImGui::Text(("Controller Button Back: " + std::to_string(input->getControllerButtonBack())).c_str());
    ImGui::Text(("Controller Button Guide: " + std::to_string(input->getControllerButtonGuide())).c_str());
    ImGui::Text(("Controller Button Start: " + std::to_string(input->getControllerButtonStart())).c_str());
}

void GUIManager::drawImages()
{
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame;

    auto imageFiles = imagesFolder.getItems();
    int columns = 8; // Máximo de 6 imágenes por fila

    if (ImGui::BeginTable("ImagesTable", columns, flags)) {
        int count = 0;
        for (auto image : imageFiles) {
            if (count % columns == 0) {
                ImGui::TableNextRow();
            }
            ImGui::TableNextColumn();

            float columnWidth = ImGui::GetColumnWidth();
            float cursorX = ImGui::GetCursorPosX() + (columnWidth - 96) * 0.5f;
            ImGui::SetCursorPosX(cursorX);

            ImGui::BeginGroup();

            ImGui::PushID(image->label.c_str());
            ImGui::ImageButton(reinterpret_cast<ImTextureID>(image->texture->getOGLTextureID()), ImVec2(96, 96));

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                ImGui::SetDragDropPayload("IMAGE_ITEM", image->label.c_str(), image->label.size() + 1);
                ImGui::Text("%s", image->label.c_str());
                ImGui::EndDragDropSource();
            }
            ImGui::PopID();

            ImVec2 textSize = ImGui::CalcTextSize(image->label.c_str());
            ImGui::SetCursorPosX(cursorX + (96 - textSize.x) * 0.5f);
            ImGui::Text("%s", image->label.c_str());

            char sizeText[32];
            snprintf(sizeText, sizeof(sizeText), "%d x %d", image->texture->width(), image->texture->height());
            ImVec2 sizeTextSize = ImGui::CalcTextSize(sizeText);
            ImGui::SetCursorPosX(cursorX + (96 - sizeTextSize.x) * 0.5f);
            ImGui::Text("%s", sizeText);

            ImGui::EndGroup();
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            count++;
        }

        int remaining = columns - (count % columns);
        if (remaining != columns) {
            for (int i = 0; i < remaining; i++) {
                ImGui::TableNextColumn();
            }
        }

        ImGui::EndTable();
    }
}

void GUIManager::setNextWindowSize(int w, int h)
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 window_size = ImVec2(w, h);

    ImVec2 center = viewport->GetCenter();
    ImVec2 pos = ImVec2(center.x - window_size.x * 0.5f, center.y - window_size.y * 0.5f);

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
}

void GUIManager::openBoneInfoDialog()
{
    showBoneMappingsEditorWindow = true;
}

void GUIManager::openLightsDepthMapsViewerDialog()
{
    showLightsDepthMapsViewerWindow = true;
}

void GUIManager::DrawSplash()
{
    if (BrakezaSetup::get()->ENABLE_SPLASH) {
        ImGui::OpenPopup("brakeza_splash");
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        BrakezaSetup::get()->ENABLE_SPLASH = false;
    }

    if (ImGui::BeginPopup("brakeza_splash")) {
        ImGui::SeparatorText("Welcome to Brakeza3D!");
        ImGui::Image(TexturePackage::getOGLTextureID(icons, "splash"), ImVec2(640, 350));
        ImGui::SeparatorText(
            std::string("Brakeza3D (" + BrakezaSetup::get()->ENGINE_VERSION + ") | https://brakeza.com | By Eduardo Rodríguez Álvarez").c_str()
        );

        if (Brakeza3D::get()->getEngineTotalTime() > BrakezaSetup::get()->SPLASH_COUNTDOWN_TIME) {
            ImGui::CloseCurrentPopup();
            ComponentsManager::get()->getComponentInput()->setEnabled(true);
        }

        ImGui::EndPopup();
    }
}

void GUIManager::LoadIcons(TexturePackage &icon)
{
    const auto interfaceFolder = BrakezaSetup::get()->ICONS_FOLDER + "interface/";

    for (const auto& iconMapping : IncosByObject::ICON_FILES) {
        icon.addItem(interfaceFolder + iconMapping.filename, iconMapping.id);
    }

    for (const auto& iconMapping : IconsByGUI::ICON_FILES) {
        icon.addItem(interfaceFolder + iconMapping.filename, iconMapping.id);
    }

    icon.addItem(BrakezaSetup::get()->IMAGES_FOLDER + "splash.png","splash");
}
