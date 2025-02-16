
#include "../include/GUI/GUIManager.h"

GUIManager::GUIManager(std::vector<Object3D *> &gameObjects)
:
    gameObjects(gameObjects),
    widgetConsole(new ImGuiConsoleApp(ComponentsManager::get()->getComponentScripting()->getLua())),
    widgetObjects3D(new GUIWidgetObjects3D(icons, this->gameObjects)),
    widgetObject3DProperties(new GUIWidgetObject3DProperties(icons, this->gameObjects, scriptEditableManager)),
    widgetProjectSettings(new GUIWidgetProjectSettings(icons, scriptEditableManager)),
    widgetMenu(new GUIWidgetMenu(icons)),
    widgetToolbar(new GUIWidgetToolbar(icons)),
    currentScriptsFolderWidget(EngineSetup::get()->SCRIPTS_FOLDER),
    currentScenesFolderWidget(EngineSetup::get()->SCENES_FOLDER),
    currentProjectsFolderWidget(EngineSetup::get()->PROJECTS_FOLDER),
    currentShadersFolderWidget(EngineSetup::get()->CUSTOM_SHADERS_FOLDER)
{
    LoadUIIcons();
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

void GUIManager::LoadUIIcons()
{
    auto iconsFolder = EngineSetup::get()->ICONS_FOLDER + "interface/";
    icons.addItem(iconsFolder + "translate.png", "translateIcon");
    icons.addItem(iconsFolder + "rotate.png", "rotateIcon");
    icons.addItem(iconsFolder + "scale.png", "scaleIcon");
    icons.addItem(iconsFolder + "stop.png", "stopIcon");
    icons.addItem(iconsFolder + "play.png", "playIcon");
    icons.addItem(iconsFolder + "reload.png", "reloadIcon");
    icons.addItem(iconsFolder + "rebuild.png", "rebuildIcon");
    icons.addItem(iconsFolder + "object.png", "objectIcon");
    icons.addItem(iconsFolder + "light.png", "lightIcon");
    icons.addItem(iconsFolder + "script.png", "scriptIcon");
    icons.addItem(iconsFolder + "swarm.png", "swarmIcon");
    icons.addItem(iconsFolder + "mesh.png", "meshIcon");
    icons.addItem(iconsFolder + "folder.png", "folderIcon");
    icons.addItem(iconsFolder + "player.png", "playerIcon");
    icons.addItem(iconsFolder + "BillboardAnimation8DirectionsIcon.png", "BillboardAnimation8DirectionsIcon");
    icons.addItem(iconsFolder + "Image2DAnimationIcon.png", "Image2DAnimationIcon");
    icons.addItem(iconsFolder + "Mesh3DAnimationIcon.png", "Mesh3DAnimationIcon");
    icons.addItem(iconsFolder + "Image2DIcon.png", "Image2DIcon");
    icons.addItem(iconsFolder + "Image3DIcon.png", "Image3DIcon");
    icons.addItem(iconsFolder + "BillboardAnimationIcon.png", "BillboardAnimationIcon");
    icons.addItem(iconsFolder + "remove.png", "removeIcon");
    icons.addItem(iconsFolder + "pause.png", "pauseIcon");
    icons.addItem(iconsFolder + "lock.png", "lockIcon");
    icons.addItem(iconsFolder + "unlock.png", "unlockIcon");
    icons.addItem(iconsFolder + "add.png", "addIcon");
    icons.addItem(iconsFolder + "scene.png", "sceneIcon");
    icons.addItem(iconsFolder + "save.png", "saveIcon");
    icons.addItem(iconsFolder + "gear.png", "gearIcon");
    icons.addItem(iconsFolder + "ghost.png", "ghostIcon");
    icons.addItem(iconsFolder + "shader.png", "shaderIcon");
    icons.addItem(iconsFolder + "spotlight.png", "spotLightIcon");
    icons.addItem(iconsFolder + "particles.png", "particlesIcon");
    icons.addItem(iconsFolder + "gravity.png", "gravityIcon");
    icons.addItem(iconsFolder + "click.png", "clickIcon");
    icons.addItem(iconsFolder + "DrawColliders.png", "drawCollidersIcon");
    icons.addItem(iconsFolder + "target.png", "targetIcon");
    icons.addItem(iconsFolder + "mouseIcon.png", "mouseLookIcon");
    icons.addItem(iconsFolder + "layoutDefault.png", "layoutDefaultIcon");
    icons.addItem(iconsFolder + "layoutCoding.png", "layoutCodingIcon");
    icons.addItem(iconsFolder + "layoutDesign.png", "layoutDesignIcon");
    icons.addItem(iconsFolder + "project.png", "projectIcon");
    icons.addItem(iconsFolder + "open.png", "openIcon");
    icons.addItem(iconsFolder + "gui.png", "guiIcon");
    icons.addItem(iconsFolder + "texture.png", "textureIcon");
}

void GUIManager::loadImagesFolder()
{
    auto images = Tools::getFolderFiles(EngineSetup::get()->IMAGES_FOLDER, "png");

    for (const auto& f: images) {
        imagesFolder.addItem(EngineSetup::get()->IMAGES_FOLDER + f, f);
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
    if ((int) customShaders.size() <= 0) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No shaders in selected object.");
    }

    for (int i = 0; i < (int) customShaders.size(); i++) {
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

    if ((int) objectScripts.size() <= 0) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No scripts in selected object.");
    }

    for (int i = 0; i < (int) objectScripts.size(); i++) {
        auto currentScript = objectScripts[i];
        ImGui::PushID(i);

        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "scriptIcon"), ImVec2(14, 14))) {
            LoadScriptDialog(currentScript->scriptFilename);
        }

        ImGui::SameLine();

        std::string optionText = std::to_string(i + 1) + ") " + Tools::removeSubstring(currentScript->scriptFilename, EngineSetup::get()->SCRIPTS_FOLDER);

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

void GUIManager::drawEditShaderWindow()
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
                Tools::writeToFile(shaderEditableManager.shader->vertexFilename, shaderEditableManager.shader->sourceVS.c_str());
            }
        }

        if (ImGui::CollapsingHeader("Fragment shader code")) {
            ImGui::InputTextMultiline("##sourceFS", shaderEditableManager.editableSourceFS, IM_ARRAYSIZE(shaderEditableManager.editableSourceFS), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10), flags);
            if (ImGui::Button(std::string("Save fragment shader").c_str())) {
                shaderEditableManager.shader->sourceFS = shaderEditableManager.editableSourceFS;
                Tools::writeToFile(shaderEditableManager.shader->fragmentFilename, shaderEditableManager.shader->sourceFS.c_str());
            }
        }
    }
    ImGui::End();
}

void GUIManager::drawEditScriptWindow()
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

    drawBrowserFolders(folder, EngineSetup::get()->SCRIPTS_FOLDER, currentScriptsFolderWidget, currentScriptsFolders, currentScriptsFolderFiles, "lua");

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
            LUADataValue LUAValue;
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

    if ((int) types.size() <= 0) {
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
            LUADataValue LUAValue;
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

    if ((int) scriptEditableManager.script->dataTypes.size() <= 0) {
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
            SceneLoader::createScene(folder + currentVariableToCreateCustomShader);
            currentScenesFolderFiles = Tools::getFolderFiles(currentScenesFolderWidget, "json");
        }
    }

    ImGui::Separator();
    drawBrowserFolders(folder, EngineSetup::get()->SCENES_FOLDER, currentScenesFolderWidget, currentScenesFolders, currentScenesFolderFiles, "json");
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
                    SceneLoader::clearScene();
                    ComponentsManager::get()->getComponentRender()->getSceneLoader().loadScene(folder + file);
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "saveIcon"), ImVec2(14, 14))) {
                    SceneLoader::saveScene(folder + file);
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "removeIcon"), ImVec2(14, 14))) {
                    ImGui::OpenPopup("Delete Scene?");
                }
                ShowDeletePopup("Delete Scene?", [folder, file, this] () {
                    SceneLoader::removeScene(folder + file);
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

void GUIManager::LoadShaderDialog(std::string &folder, std::string &file)
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
    const char* combo_preview_value = items[item_current_idx];

    if (ImGui::Combo("Type", &item_current_idx, items.data(), (int) items.size())) {
        std::cout << "Seleccionado: " << items[item_current_idx] << std::endl;
    }

    if (ImGui::Button(std::string("Create custom shader").c_str())) {
        if (!currentVariableToCreateCustomShader.empty()) {
            auto type = ShaderOpenGLCustom::getShaderTypeFromString(items[item_current_idx]);
            ShaderOpenGLCustom::createEmptyCustomShader(currentVariableToCreateCustomShader, folder, type);
            updateFolderFiles();
        }
    }

    ImGui::Separator();

    drawBrowserFolders(folder, EngineSetup::get()->CUSTOM_SHADERS_FOLDER, currentShadersFolderWidget, currentShadersFolders, currentShadersFolderFiles, "json");

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
                EngineSetup::DragDropCustomShaderData data = {folderCopy.c_str(), fileCopy.c_str()};
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
                ShaderOpenGLCustom::removeCustomShaderFiles(folder,  Tools::getFilenameWithoutExtension(file));
                updateFolderFiles();
            });

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void GUIManager::drawWidgets()
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
        drawObjectVariables();
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

void GUIManager::draw(float timedelta, bool &finish)
{
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

    drawWidgets();

    bool p_open = true;
    widgetConsole->Draw("Logging/Console", &p_open);
    widgetObjects3D->draw(selectedObjectIndex);
    widgetObject3DProperties->draw(selectedObjectIndex);
    widgetMenu->draw(finish, showAboutWindow);
    widgetToolbar->draw();
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

    drawEditShaderWindow();
    drawEditScriptWindow();

    ImGui::End();
}

ImGuiConsoleApp *GUIManager::getConsole()
{
    return widgetConsole;
}

TexturePackage *GUIManager::getImGuiTextures()
{
    return &icons;
}

void GUIManager::setSelectedObjectIndex(int selectedObjectIndex)
{
    GUIManager::selectedObjectIndex = selectedObjectIndex;
}

void GUIManager::setSelectedObject(Object3D *s) {
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

void GUIManager::drawObjectVariables()
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
                        ImGui::Text("%f", (float) luaEnvironment[key]);
                    } else if (type == "string") {
                        ImGui::Text("%s", std::string(luaEnvironment[key]).c_str());
                    } else if (type == "boolean") {
                        ImGui::Text("%d", (bool) luaEnvironment[key]);
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
}

void GUIManager::drawImages()
{
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame;

    auto imageFiles = imagesFolder.getItems();
    int columns = 8; // Máximo de 6 imágenes por fila
    int count = 0;

    if (ImGui::BeginTable("ImagesTable", columns, flags)) {
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
            ImGui::ImageButton((ImTextureID)image->texture->getOGLTextureID(), ImVec2(96, 96));

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
