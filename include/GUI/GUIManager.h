
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include <string>
#include <vector>
#include "../Objects/Object3D.h"
#include "imgui.h"
#include "../ComponentsManager.h"
#include "GUIConsole.h"
#include "../Brakeza3D.h"
#include "../Misc/TexturePackage.h"
#include "../Render/Logging.h"
#include "../FXEffect/FXOutliner.h"
#include "../FXEffect/FXBlink.h"
#include "../FXEffect/FXShockWave.h"
#include "GUIWidgetObjects3D.h"
#include "GUIWidgetObject3DProperties.h"
#include "GUIWidgetProjectSettings.h"
#include "GUIWidgetMenu.h"
#include "GUIWidgetToolbar.h"

class GUIManager {
private:
    std::vector<Object3D *> &gameObjects;

    int selectedObjectIndex = -1;
    ScriptEditableManager scriptEditableManager;

    ImGuiConsoleApp *widgetConsole;
    GUIWidgetObjects3D *widgetObjects3D;
    GUIWidgetObject3DProperties *widgetObject3DProperties;
    GUIWidgetProjectSettings *widgetProjectSettings;
    GUIWidgetMenu *widgetMenu;
    GUIWidgetToolbar *widgetToolbar;

    TexturePackage icons;
    TexturePackage imagesFolder;

    std::string selected_file;
    std::string currentVariableToAddName;
    std::string currentVariableToCreateCustomShader;
    ImGuizmo::OPERATION guizmoOperation;
    std::string currentScriptsFolderWidget;
    std::string currentScenesFolderWidget;
    std::string currentProjectsFolderWidget;

    bool show_about_window = false;

    const char *availableMesh3DShaders[4] = {"Edge", "Blink", "ShockWave", "Tint"};
public:

    void loadImagesFolder() {

        auto images = Tools::getFolderFiles(EngineSetup::get()->IMAGES_FOLDER, "png");

        for (const auto& f: images) {
            imagesFolder.addItem(EngineSetup::get()->IMAGES_FOLDER + f, f);
        }
    }

    explicit GUIManager(std::vector<Object3D *> &gameObjects)
        :
            gameObjects(gameObjects),
            widgetConsole(new ImGuiConsoleApp(ComponentsManager::get()->getComponentScripting()->getLua())),
            widgetObjects3D(new GUIWidgetObjects3D(icons, this->gameObjects)),
            widgetObject3DProperties(new GUIWidgetObject3DProperties(icons, this->gameObjects, scriptEditableManager)),
            widgetProjectSettings(new GUIWidgetProjectSettings(icons, scriptEditableManager)),
            widgetMenu(new GUIWidgetMenu(icons)),
            widgetToolbar(new GUIWidgetToolbar(icons)),
            guizmoOperation(ImGuizmo::TRANSLATE),
            currentScriptsFolderWidget(EngineSetup::get()->SCRIPTS_FOLDER),
            currentScenesFolderWidget(EngineSetup::get()->SCENES_FOLDER),
            currentProjectsFolderWidget(EngineSetup::get()->PROJECTS_FOLDER)
    {
        LoadUIIcons();
        loadImagesFolder();
    }

    void LoadUIIcons()
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
    }

    void drawSelectedObjectScripts()
    {
        bool hasSelectedIndex = selectedObjectIndex >= 0 && selectedObjectIndex < gameObjects.size();

        if (!hasSelectedIndex) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No object selected");
            return;
        }

        auto o = gameObjects[selectedObjectIndex];
        auto objectScripts = o->getScripts();

        if ((int) objectScripts.size() <= 0) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "The selected object has no associated scripts");
        }

        for (int i = 0; i < (int) objectScripts.size(); i++) {
            auto currentScript = objectScripts[i];
            ImGui::PushID(i);

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
            if (ImGui::Button(optionText.c_str())) {
                delete scriptEditableManager.script;
                scriptEditableManager.selectedScriptFilename = currentScript->scriptFilename;
                scriptEditableManager.script = new ScriptLUA(
                        scriptEditableManager.selectedScriptFilename,
                        ScriptLUA::dataTypesFileFor(scriptEditableManager.selectedScriptFilename)
                );
                strcpy(scriptEditableManager.editableSource, scriptEditableManager.script->content.c_str());
            }

            currentScript->drawImGuiProperties();

            ImGui::Separator();

            ImGui::PopID();
        }
    }

    void drawScriptsLuaFolderFiles(const std::string& folder)
    {
        static char name[256];
        strncpy(name, currentVariableToAddName.c_str(), sizeof(name));
        if (ImGui::InputText("Script name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
            currentVariableToAddName = name;
        }
        if (ImGui::Button(std::string("Create script").c_str())) {
            if (!currentVariableToAddName.empty()) {
                ComponentScripting::createScriptLUAFile(EngineSetup::get()->SCRIPTS_FOLDER + currentVariableToAddName);
            }
        }

        ImGui::Separator();

        drawBrowserFolders(folder, EngineSetup::get()->SCRIPTS_FOLDER, currentScriptsFolderWidget);

        auto files= Tools::getFolderFiles(folder, "lua");
        static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
        if (ImGui::BeginTable("ScriptsFolderTable", 2, flags)) {
            for (int i = 0; i < files.size(); i++) {
                ImGui::PushID(i);
                const auto& file = files[i];
                auto fullPath = folder + file;
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Image(TexturePackage::getOGLTextureID(icons, "scriptIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                std::string optionText = std::to_string(i + 1) + ") " + file;
                if (ImGui::Selectable(optionText.c_str())) {
                    delete scriptEditableManager.script;
                    scriptEditableManager.selectedScriptFilename = fullPath;
                    scriptEditableManager.script = new ScriptLUA(
                        scriptEditableManager.selectedScriptFilename,
                        ScriptLUA::dataTypesFileFor(scriptEditableManager.selectedScriptFilename)
                    );
                    strcpy(scriptEditableManager.editableSource, scriptEditableManager.script->content.c_str());
                }
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    ImGui::SetDragDropPayload("SCRIPT_ITEM",fullPath.c_str(),fullPath.size() + 1);
                    ImGui::Text("%s", fullPath.c_str()); // Esto es lo que se muestra mientras se arrastra
                    ImGui::EndDragDropSource();
                }
                ImGui::TableSetColumnIndex(1);
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "removeIcon"), ImVec2(14, 14))) {
                    ImGui::OpenPopup("Delete Script?");
                }
                ShowDeletePopup("Delete Script?", [folder, file] () {ComponentScripting::removeScriptLUAFile(folder + file);});
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }

    void drawWidgetScriptProperties()
    {
        ImGui::Text("Current script properties for: ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", scriptEditableManager.script->scriptFilename.c_str());

        ImGui::Separator();
        static char name[256];

        strncpy(name, currentVariableToAddName.c_str(), sizeof(name));

        if (ImGui::InputText("Variable name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
            currentVariableToAddName = name;
        }

        const char* items[] = { "int", "float", "string", "Vertex3D" };
        static int selectedItem = 0;
        ImGui::Combo("Type", &selectedItem, items, IM_ARRAYSIZE(items));

        if (ImGui::Button(std::string("Create variable").c_str())) {
            if (!currentVariableToAddName.empty()) {
                LUADataValue LUAValue;
                scriptEditableManager.script->addDataTypeEmpty(currentVariableToAddName.c_str(), items[selectedItem]);
            }
        }

        ImGui::Separator();
        if (ImGui::BeginTable("ScriptProperties", 3)) {
            for (int i = 0; i < scriptEditableManager.script->dataTypes.size(); i++) {
                ImGui::TableNextRow();
                auto type = &scriptEditableManager.script->dataTypes[i];

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d) %s", i + 1, type->name.c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", type->type.c_str());

                ImGui::TableSetColumnIndex(2);
                if (ImGui::Button(std::string("Remove##id" + std::to_string(i)).c_str())) {
                    scriptEditableManager.script->removeDataType(*type);
                    scriptEditableManager.script->updateFileTypes();
                }
            }
            ImGui::EndTable();
        }

        if (ImGui::Button(std::string("Save script types").c_str())) {
            scriptEditableManager.script->updateFileTypes();
        }
    }

    static void drawCameraSettings()
    {
        auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

        const float range_min = 0;
        const float range_max = 1000;

        const float range_sensibility = -0.1;
        const float range_min_yaw = 0;
        const float range_max_yaw = 0;

        const float range_min_movement = 0;
        const float range_max_movement = 500;

        const float range_min_mouse_sensitivity = 0;
        const float range_max_mouse_sensitivity = 3;


        if (ImGui::TreeNode("Mouse##4")) {
            ImGui::Checkbox("Mouse Look", &EngineSetup::get()->MOUSE_LOOK);
            if (EngineSetup::get()->MOUSE_LOOK) {
                ImGui::DragScalar("Sens.", ImGuiDataType_Float, &EngineSetup::get()->MOUSE_SENSITIVITY,range_sensibility, &range_min_mouse_sensitivity, &range_max_mouse_sensitivity, "%f",1.0f);
            }
            ImGui::TreePop();
        }
        ImGui::Separator();

        // position
        Tools::ImGuiVertex3D("Position##1", "X", "Y", "Z", &camera->getPosition(), range_sensibility, -99999, 99999);
        ImGui::Separator();

        // rotation
        if (ImGui::TreeNode("Orientation##2")) {
            ImGui::DragScalar("Yaw", ImGuiDataType_Float, &camera->getYaw(), range_sensibility, &range_min_yaw,&range_max_yaw, "%f", 1.0f);
            ImGui::DragScalar("Pitch", ImGuiDataType_Float, &camera->getPitch(), range_sensibility, &range_min_yaw,&range_max_yaw, "%f", 1.0f);
            ImGui::DragScalar("Roll", ImGuiDataType_Float, &camera->getRoll(), range_sensibility, &range_min_yaw,&range_max_yaw, "%f", 1.0f);
            ImGui::Separator();
            ImGui::Text("Values from Rotation Matrix Getters: ");
            ImGui::Text(("Pitch: " + std::to_string(camera->getRotation().getPitchDegree())).c_str()); ImGui::SameLine();
            ImGui::Text(("Yaw: " + std::to_string(camera->getRotation().getYawDegree())).c_str()); ImGui::SameLine();
            ImGui::Text(("Roll: " + std::to_string(camera->getRotation().getRollDegree())).c_str()); ImGui::SameLine();
            ImGui::TreePop();
        }

        ImGui::Separator();

        if (ImGui::TreeNode("Keyboard##3")) {
            ImGui::DragScalar("Walking", ImGuiDataType_Float, &EngineSetup::get()->WALKING_SPEED,range_sensibility, &range_min_movement, &range_max_movement, "%f", 1.0f);
            ImGui::DragScalar("Turn", ImGuiDataType_Float, &EngineSetup::get()->TURN_SPEED,range_sensibility, &range_min_movement, &range_max_movement, "%f", 1.0f);
            ImGui::DragScalar("Pitch", ImGuiDataType_Float, &EngineSetup::get()->PITCH_SPEED,range_sensibility, &range_min_movement, &range_max_movement, "%f", 1.0f);
            ImGui::DragScalar("Strafe", ImGuiDataType_Float, &EngineSetup::get()->STRAFE_SPEED,range_sensibility, &range_min_movement, &range_max_movement, "%f", 1.0f);
            ImGui::TreePop();
        }
        ImGui::Separator();

        if (ImGui::TreeNode(std::string("Velocity").c_str())) {
            ImGui::Text("%s", (std::string("X: ") + std::to_string(camera->getVelocity().getComponent().x)).c_str());
            ImGui::Text("%s", (std::string("Y: ") + std::to_string(camera->getVelocity().getComponent().y)).c_str());
            ImGui::Text("%s", (std::string("Z: ") + std::to_string(camera->getVelocity().getComponent().z)).c_str());
            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    void drawProjectsFiles(const std::string& folder)
    {
        static char name[256];
        strncpy(name, currentVariableToCreateCustomShader.c_str(), sizeof(name));
        if (ImGui::InputText("File name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
            currentVariableToCreateCustomShader = name;
        }
        if (ImGui::Button(std::string("Create Project").c_str())) {
            if (!currentVariableToCreateCustomShader.empty()) {
                ProjectLoader::createProject(folder + currentVariableToCreateCustomShader);
            }
        }

        ImGui::Separator();

        std::vector<std::string> files = Tools::getFolderFiles(folder, "json");
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
                    ShowDeletePopup("Delete Project?", [folder, file] () { ProjectLoader::removeProject(folder + file); });

                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }

    void drawScenesFolder(const std::string& folder)
    {
        static char name[256];
        strncpy(name, currentVariableToCreateCustomShader.c_str(), sizeof(name));
        if (ImGui::InputText("File name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
            currentVariableToCreateCustomShader = name;
        }
        if (ImGui::Button(std::string("Create Scene").c_str())) {
            if (!currentVariableToCreateCustomShader.empty()) {
                SceneLoader::createScene(folder + currentVariableToCreateCustomShader);
            }
        }

        ImGui::Separator();
        drawBrowserFolders(folder, EngineSetup::get()->SCENES_FOLDER, currentScenesFolderWidget);
        ImGui::Separator();

        auto files = Tools::getFolderFiles(folder, "json");
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
                    ShowDeletePopup("Delete Scene?", [folder, file] () { SceneLoader::removeScene(folder + file); });
            }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }

    void drawBrowserFolders(const std::string& folder, const std::string& baseFolder, std::string& destiny)
    {
        ImGui::Text("Current: %s", folder.c_str());

        ImGui::Separator();

        auto folders = Tools::getFolderFolders(folder);
        if (folder != baseFolder) {
            ImGui::Image(TexturePackage::getOGLTextureID(icons, "folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::Button("..")) {
                destiny = Tools::GoBackFromFolder(destiny);
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
            }
        }
    }

    void drawSceneShaders()
    {
        static char name[256];
        strncpy(name, currentVariableToCreateCustomShader.c_str(), sizeof(name));
        if (ImGui::InputText("Shader name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
            currentVariableToCreateCustomShader = name;
        }
        if (ImGui::Button(std::string("Create custom shader").c_str())) {
            if (!currentVariableToCreateCustomShader.empty()) {
                ComponentsManager::get()->getComponentRender()->addShaderToScene(
                    ShaderOpenGLCustom::createEmptyCustomShader(currentVariableToCreateCustomShader)
                );
            }
        }

        ImGui::Separator();

        auto shaders = ComponentsManager::get()->getComponentRender()->getSceneShaders();
        for (int i = 0; i < shaders.size(); i++) {
            auto s = shaders[i];
            ImGui::PushID(i);
            ImGui::Image(TexturePackage::getOGLTextureID(icons, "shaderIcon"), ImVec2(26, 26));
            ImGui::SameLine(46);
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
                s->compile();
            }
            ImGui::SameLine();
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "removeIcon"), ImVec2(14, 14))) {
                ComponentsManager::get()->getComponentRender()->removeShader(i);
            }
            ImGui::SameLine();
            if (ImGui::CollapsingHeader(s->getLabel().c_str(), ImGuiTreeNodeFlags_None)) {
                ImGui::PushID(i);
                s->drawImGuiProperties();
                ImGui::PopID();
            }
            ImGui::PopID();
        }
    }

    void drawMesh3DShaders() {

        int i = 0;
        ImGui::PushID(i);
        std::string optionText = std::to_string(i + 1) + ") " + availableMesh3DShaders[i];
        if (ImGui::Selectable(optionText.c_str())) {
        }
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("MESH3D_SHADER_ITEM", std::to_string(i).c_str(), sizeof(int));
            ImGui::Text("%s", availableMesh3DShaders[i]);
            ImGui::EndDragDropSource();
        }
        ImGui::PopID();

        i = 1;
        ImGui::PushID(i);
        optionText = std::to_string(i + 1) + ") " + availableMesh3DShaders[i];
        if (ImGui::Selectable(optionText.c_str())) {
        }
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("MESH3D_SHADER_ITEM", std::to_string(i).c_str(), sizeof(int));
            ImGui::Text("%s", availableMesh3DShaders[i]);
            ImGui::EndDragDropSource();
        }
        ImGui::PopID();

        i = 2;
        ImGui::PushID(i);
        optionText = std::to_string(i + 1) + ") " + availableMesh3DShaders[i];
        if (ImGui::Selectable(optionText.c_str())) {
        }
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("MESH3D_SHADER_ITEM", std::to_string(i).c_str(), sizeof(int));
            ImGui::Text("%s", availableMesh3DShaders[i]);
            ImGui::EndDragDropSource();
        }
        ImGui::PopID();

        i = 3;
        ImGui::PushID(i);
        optionText = std::to_string(i + 1) + ") " + availableMesh3DShaders[i];
        if (ImGui::Selectable(optionText.c_str())) {
        }
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("MESH3D_SHADER_ITEM", std::to_string(i).c_str(), sizeof(int));
            ImGui::Text("%s", availableMesh3DShaders[i]);
            ImGui::EndDragDropSource();
        }
        ImGui::PopID();
    }

    void drawCustomShaders()
    {
        std::vector<std::string> result = Tools::getFolderFiles(EngineSetup::get()->CUSTOM_SHADERS_FOLDER, "fs");

        std::sort( result.begin(), result.end() );

        for (int i = 0; i < result.size(); i++) {
            const auto& file = result[i];
            auto title = std::to_string(i+1) + ") " + file;
            if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                ImGui::PushID(i);
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "shaderIcon"), ImVec2(14, 14))) {
                    std::string name = Tools::getFilenameWithoutExtension(file);
                    ComponentsManager::get()->getComponentRender()->addShaderToScene(
                            new ShaderOpenGLCustom(name, EngineSetup::get()->CUSTOM_SHADERS_FOLDER + file)
                    );
                }
                ImGui::SameLine();
                ImGui::Text("%s", title.c_str());
                ImGui::PopID();
            }
        }
    }

    void drawWidgets()
    {
        if (ImGui::Begin("Object Scripts")) {
            drawSelectedObjectScripts();
        }
        ImGui::End();

        if (ImGui::Begin("Scripts")) {
            drawScriptsLuaFolderFiles(currentScriptsFolderWidget);
        }
        ImGui::End();

        if (ImGui::Begin("Illumination/FX")) {
            if (ImGui::TreeNode("Global illumination")) {
                const float range_illumination_min_settings = -1.0f;
                const float range_illumination_max_settings = 1.0f;
                const float sens_illumination_settings = 0.01f;
                auto dirLight = ComponentsManager::get()->getComponentWindow()->getShaderOGLRender()->getDirectionalLight();

                if (ImGui::TreeNode("Direction")) {
                    ImGui::DragScalar("x", ImGuiDataType_Float, &dirLight->direction.x, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("y", ImGuiDataType_Float, &dirLight->direction.y, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("z", ImGuiDataType_Float, &dirLight->direction.z, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::TreePop();
                }

                ImVec4 color = {dirLight->ambient.x, dirLight->ambient.y, dirLight->ambient.z, 1};
                bool changed_color = ImGui::ColorEdit4("Ambient##", (float *) &color, ImGuiColorEditFlags_NoOptions);
                if (changed_color) {
                    dirLight->ambient = {color.x, color.y, color.z};
                }
                color = {dirLight->specular.x, dirLight->specular.y, dirLight->specular.z, 1};
                changed_color = ImGui::ColorEdit4("Specular##", (float *) &color, ImGuiColorEditFlags_NoOptions);
                if (changed_color) {
                    dirLight->specular = {color.x, color.y, color.z};
                }

                color = {dirLight->diffuse.x, dirLight->diffuse.y, dirLight->diffuse.z, 1};
                changed_color = ImGui::ColorEdit4("Diffuse##", (float *) &color, ImGuiColorEditFlags_NoOptions);
                if (changed_color) {
                    dirLight->diffuse = {color.x, color.y, color.z};
                }

                ImGui::TreePop();
            }

            ImGui::Separator();
            if (ImGui::TreeNode("DOF Settings")) {
                const float focalMinValues = 0;
                const float focalMaxValues = 1;
                const float focalValueSens = 0.001;

                const float depthMinValues = 0;
                const float depthMaxValues = 100;
                const float depthValueSens = 0.1;

                const int minBlurRadius = 0;
                const int maxBlurRadius = 10;

                ImGui::Checkbox("Enable DOF", &EngineSetup::get()->ENABLE_DEPTH_OF_FIELD);

                if (EngineSetup::get()->ENABLE_DEPTH_OF_FIELD) {
                    ImGui::DragScalar("Focal range", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->focalRange, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
                    ImGui::DragScalar("Focal distance", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->focalDistance, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
                    ImGui::DragScalar("Blur radius", ImGuiDataType_S32, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->blurRadius,1.0f, &minBlurRadius, &maxBlurRadius, "%d", 1.0f);
                    ImGui::DragScalar("Intensity", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->intensity, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
                    ImGui::DragScalar("Far Plane", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->farPlane, depthValueSens, &depthMinValues, &depthMaxValues, "%f", 1.0f);
                }
                ImGui::TreePop();
            }
            ImGui::Separator();
            if (ImGui::TreeNode("FOG Settings")) {
                ImGui::Checkbox("Enable FOG", &EngineSetup::get()->ENABLE_FOG);

                if (EngineSetup::get()->ENABLE_FOG) {
                    const float rangeFogSens = 0.1;
                    const float rangeFogMin = 0;
                    const float rangeFogMax = 0;

                    ImGui::DragScalar("FOG min distance", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLFOG()->fogMinDist, rangeFogSens, &rangeFogMin, &rangeFogMax, "%f", 1.0f);
                    ImGui::DragScalar("FOG max distance", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLFOG()->fogMaxDist, rangeFogSens, &rangeFogMin, &rangeFogMax, "%f", 1.0f);
                    ImGui::DragScalar("FOG intensity", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLFOG()->intensity, rangeFogSens, &rangeFogMin, &rangeFogMax, "%f", 1.0f);

                    if (ImGui::TreeNode("FOG Color")) {
                        const float fogColorSend = 0.01;
                        const float fogColorMin = 0;
                        const float fogColorMax = 1;

                        ImGui::DragScalar("x", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLFOG()->fogColor.r, fogColorSend,&fogColorMin, &fogColorMax, "%f", 1.0f);
                        ImGui::DragScalar("y", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLFOG()->fogColor.g, fogColorSend,&fogColorMin, &fogColorMax, "%f", 1.0f);
                        ImGui::DragScalar("z", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLFOG()->fogColor.b, fogColorSend,&fogColorMin, &fogColorMax, "%f", 1.0f);
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
            ImGui::Separator();
            ImGui::Checkbox("Show Depth Map", &EngineSetup::get()->SHOW_DEPTH_OF_FIELD);
        }
        ImGui::End();

        if (ImGui::Begin("Script Preview")) {
            if (!scriptEditableManager.selectedScriptFilename.empty()) {
                drawWidgetScriptProperties();
                drawImGuiScriptCode();
            }
        }
        ImGui::End();

        if (ImGui::Begin("Object variables")) {
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
                            ImGui::Text("%s", std::string(luaEnvironment[key]).c_str());
                        }
                    }
                    ImGui::EndTable();
                    if (numVarFound <= 0) {
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "The selected object has defined variables");
                    }
                }
            }
        }
        ImGui::End();

        if (ImGui::Begin("Global variables")) {
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
        ImGui::End();

        widgetProjectSettings->draw();

        if (ImGui::Begin("Keyboard/Mouse")) {
            auto input = ComponentsManager::get()->getComponentInput();

            ImGui::Text(("Mouse motion: " + std::to_string(input->isMouseMotion())).c_str());
            ImGui::Text(("Mouse motion RelX: " + std::to_string(input->getMouseMotionXRel())).c_str());
            ImGui::Text(("Mouse motion RelY: " + std::to_string(input->getMouseMotionYRel())).c_str());
            ImGui::Separator();

            ImGui::Text(("Click Left: " + std::to_string(input->isClickLeft())).c_str());
            ImGui::Text(("Click Right: " + std::to_string(input->isClickRight())).c_str());
            ImGui::Text(("Click Left pressed: " + std::to_string(input->isLeftMouseButtonPressed())).c_str());
            ImGui::Text(("Click Right pressed: " + std::to_string(input->isRightMouseButtonPressed())).c_str());

            ImGui::Separator();
            ImGui::Text(("Key event Down: " + std::to_string(input->isKeyEventDown())).c_str());
            ImGui::Text(("Key event Up: " + std::to_string(input->isKeyEventUp())).c_str());
            ImGui::Separator();
            ImGui::Text(("Enabled: " + std::to_string(input->isEnabled())).c_str());
        }
        ImGui::End();

        if (ImGui::Begin("Camera settings")) {
            drawCameraSettings();
        }
        ImGui::End();

        if (ImGui::Begin("Scene Shaders")) {
            drawSceneShaders();
        }
        ImGui::End();
        if (ImGui::Begin("Custom Shaders")) {
            drawCustomShaders();
        }
        ImGui::End();

        if (ImGui::Begin("Mesh3D Shaders")) {
            drawMesh3DShaders();
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
            static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;

            auto items = imagesFolder.getItems();
            for (auto a : items) {
                ImGui::Image((ImTextureID)a->texture, ImVec2(96, 96));
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Selectable(a->label.c_str());
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    ImGui::SetDragDropPayload("IMAGE_ITEM", a->label.c_str(), a->label.size() + 1);
                    ImGui::Text("%s", a->label.c_str());
                    ImGui::EndDragDropSource();
                }
                ImGui::Text("%d x %d", a->texture->width(), a->texture->height());
                ImGui::EndGroup();
            }
        }
        ImGui::End();
    }

    void drawImGuiScriptCode()
    {
        ImGui::Separator();

        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
        ImGui::InputTextMultiline("##source", scriptEditableManager.editableSource, IM_ARRAYSIZE(scriptEditableManager.editableSource), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 25), flags);
        if (ImGui::Button(std::string("Save").c_str())) {
            scriptEditableManager.script->updateScriptCodeWith(scriptEditableManager.editableSource);
            scriptEditableManager.script->reloadScriptCode();
        }
    }

    virtual void draw(float timedelta, bool &finish)
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
        widgetObject3DProperties->draw(selectedObjectIndex, guizmoOperation);
        widgetMenu->draw(finish, show_about_window);
        widgetToolbar->draw();

        ImGui::End();
    }

    ImGuiConsoleApp *getConsole(){
        return widgetConsole;
    }

    TexturePackage *getImGuiTextures() {
        return &icons;
    }

    void setSelectedObjectIndex(int selectedObjectIndex)
    {
        GUIManager::selectedObjectIndex = selectedObjectIndex;
    }

    void setSelectedObject(Object3D *s) {
        int i = 0;
        for (auto o: gameObjects) {
            if (s == o) {
                setSelectedObjectIndex(i);
            }
            i++;
        }
    }

    void setGuizmoOperation(ImGuizmo::OPERATION guizmoOperation) {
        GUIManager::guizmoOperation = guizmoOperation;
    }

    static void ShowDeletePopup(const char* title, const std::function<void()>& onConfirm)
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
};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
