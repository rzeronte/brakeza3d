
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
            widgetConsole(new ImGuiConsoleApp(LUAManager::get()->getLua())),
            widgetObjects3D(new GUIWidgetObjects3D(icons, this->gameObjects)),
            widgetObject3DProperties(new GUIWidgetObject3DProperties(icons, this->gameObjects, scriptEditableManager)),
            widgetProjectSettings(new GUIWidgetProjectSettings(icons, scriptEditableManager)),
            widgetMenu(new GUIWidgetMenu(icons)),
            widgetToolbar(new GUIWidgetToolbar(icons)),
            guizmoOperation(ImGuizmo::TRANSLATE),
            currentScriptsFolderWidget(EngineSetup::get()->SCRIPTS_FOLDER)
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
        icons.addItem(iconsFolder + "freelook.png", "freelookIcon");
    }

    void drawScriptsLuaFolderFiles(const std::string& folder)
    {
        auto files= Tools::getFolderFiles(folder, "lua");
        auto folders= Tools::getFolderFolders(folder);

        if (folder != EngineSetup::get()->SCRIPTS_FOLDER) {
            ImGui::Image(TexturePackage::getOGLTextureID(icons, "folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::Button("..")) {
                currentScriptsFolderWidget = Tools::GoBackFromFolder(currentScriptsFolderWidget);
            }

        }
        for (const auto & i : folders) {
            auto fullPathFolder = folder + i;
            ImGui::Image(TexturePackage::getOGLTextureID(icons, "folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::Button(i.c_str())) {
                currentScriptsFolderWidget = fullPathFolder + "/";
            }
        }

        for (int i = 0; i < files.size(); i++) {
            const auto& file = files[i];
            auto fullPath = folder + file;
            ImGui::PushID(i);
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

            ImGui::PopID();
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
                ImGui::Text("%s", type->name.c_str());

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

        std::string rotation_text = "Orientation##2";
        std::string movement_text = "Keyboard##3";
        std::string mouse_text = "Mouse##4";
        std::string autoscrollSpeed_text = "AutoScroll##5";

        ImGui::Checkbox("Free Look", &ComponentsManager::get()->getComponentCamera()->freeLook);

        ImGui::Separator();

        // position
        Tools::ImGuiVertex3D("Position##1", "X", "Y", "Z", &camera->getPosition(), range_sensibility, -range_min, range_max);

        ImGui::Separator();

        // rotation
        if (ImGui::TreeNode(rotation_text.c_str())) {
            ImGui::DragScalar("Yaw", ImGuiDataType_Float, &camera->getYaw(), range_sensibility, &range_min_yaw,&range_max_yaw, "%f", 1.0f);
            ImGui::DragScalar("Pitch", ImGuiDataType_Float, &camera->getPitch(), range_sensibility, &range_min_yaw,&range_max_yaw, "%f", 1.0f);
            ImGui::DragScalar("Roll", ImGuiDataType_Float, &camera->getRoll(), range_sensibility, &range_min_yaw,&range_max_yaw, "%f", 1.0f);
            ImGui::TreePop();
        }

        ImGui::Separator();

        if (ImGui::TreeNode(movement_text.c_str())) {
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

        if (ImGui::TreeNode(mouse_text.c_str())) {
            ImGui::DragScalar("Sens.", ImGuiDataType_Float, &EngineSetup::get()->MOUSE_SENSITIVITY,range_sensibility, &range_min_mouse_sensitivity, &range_max_mouse_sensitivity, "%f",1.0f);
            ImGui::TreePop();
        }

        ImGui::Separator();

        if (ImGui::TreeNode(autoscrollSpeed_text.c_str())) {
            ImGui::Checkbox("AutoScroll Enabled", &ComponentsManager::get()->getComponentCamera()->isAutoScroll());
            const float range_autoscroll_sensibility = 0.1;
            const float range_autoscroll_min = 0;
            const float range_autoscroll_max = 0;
            ImGui::DragScalar("Speed X", ImGuiDataType_Float, &ComponentsManager::get()->getComponentCamera()->getAutoScrollSpeed().x,range_autoscroll_sensibility, &range_autoscroll_min, &range_autoscroll_max, "%f", 1.0f);
            ImGui::DragScalar("Speed Y", ImGuiDataType_Float, &ComponentsManager::get()->getComponentCamera()->getAutoScrollSpeed().y,range_autoscroll_sensibility, &range_autoscroll_min, &range_autoscroll_max, "%f", 1.0f);
            ImGui::DragScalar("Speed Z", ImGuiDataType_Float, &ComponentsManager::get()->getComponentCamera()->getAutoScrollSpeed().z,range_autoscroll_sensibility, &range_autoscroll_min, &range_autoscroll_max, "%f", 1.0f);
            ImGui::TreePop();
        }

        ImGui::Separator();
    }

    void drawScenesFiles() {

        std::vector<std::string> result = Tools::getFolderFiles(EngineSetup::get()->SCENES_FOLDER, "json");

        std::sort( result.begin(), result.end() );

        for (int i = 0; i < result.size(); i++) {
            const auto& file = result[i];
            auto title = std::to_string(i+1) + ") " + file;
            if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                ImGui::PushID(i);
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "playIcon"), ImVec2(14, 14))) {
                    SceneLoader::clearScene();
                    ComponentsManager::get()->getComponentRender()->getSceneLoader().loadScene(EngineSetup::get()->SCENES_FOLDER + file);
                }

                ImGui::SameLine();

                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(icons, "saveIcon"), ImVec2(14, 14))) {
                    SceneLoader::saveScene(file);
                }
                ImGui::SameLine();

                ImGui::Text("%s", title.c_str());
                ImGui::PopID();
            }
        }
    }

    void drawCustomShaders() {

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

        if (ImGui::Begin("Custom Shaders")) {
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
        ImGui::End();
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

    void drawWidgets()
    {
        if (ImGui::Begin("Scripts")) {
            //drawScriptFiles(result);
            ImGui::Text((std::string("Folder: ") + currentScriptsFolderWidget).c_str());
            ImGui::Separator();
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

            if (hasSelectedIndex) {
                static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
                if (ImGui::BeginTable("ObjectVariablesTable", 2, flags)) {
                    auto o = gameObjects[selectedObjectIndex];
                    auto scripts = o->getScripts();
                    auto &lua = o->getLuaEnvironment();

                    for (auto currentScript : scripts) {
                        for (auto & dataType : currentScript->dataTypes) {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("%s", std::string(dataType.name).c_str());

                            ImGui::TableSetColumnIndex(1);
                            switch (EngineSetup::get()->LUADataTypesMapping[dataType.type]) {
                                case EngineSetup::LUADataType::INT:
                                case EngineSetup::LUADataType::FLOAT:
                                case EngineSetup::LUADataType::STRING: {
                                    ImGui::Text("%s", std::string(lua[dataType.name]).c_str());
                                    break;
                                }
                                case EngineSetup::LUADataType::VERTEX3D: {
                                    Vertex3D v = lua[dataType.name];
                                    std::string vTextX = std::string("x: " + std::to_string(v.x));
                                    std::string vTextY = std::string("y: " + std::to_string(v.y));
                                    std::string vTextZ = std::string("z: " + std::to_string(v.z));
                                    ImGui::Text("%s", vTextX.c_str());
                                    ImGui::Text("%s", vTextY.c_str());
                                    ImGui::Text("%s", vTextZ.c_str());
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                    }

                    if ((int) scripts.size() <= 0) {
                        ImGui::Text("%s", "There are not variables yet");
                    }

                    ImGui::EndTable();
                }
            }
        }
        ImGui::End();

        if (ImGui::Begin("Global variables")) {
            static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
            if (ImGui::BeginTable("GlobalVariablesTable", 2, flags)) {
                auto scripts = ComponentsManager::get()->getComponentRender()->getLUAScripts();
                auto &lua = LUAManager::get()->getLua();

                for (auto currentScript : scripts) {
                    for (auto & dataType : currentScript->dataTypes) {
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", std::string(dataType.name).c_str());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", std::string(lua[dataType.name]).c_str());
                    }
                }

                if ((int) scripts.size() <= 0) {
                    ImGui::Text("%s", "There are not variables yet");
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();

        widgetProjectSettings->draw();

        if (ImGui::Begin("Camera settings")) {
            drawCameraSettings();
        }
        ImGui::End();

        if (ImGui::Begin("Scene Shaders")) {
            drawCustomShaders();
        }
        ImGui::End();

        if (ImGui::Begin("Mesh3D Shaders")) {
            drawMesh3DShaders();
        }
        ImGui::End();

        if (ImGui::Begin("Scenes")) {
            drawScenesFiles();
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
        widgetMenu->draw(finish);
        widgetToolbar->draw();

        ImGui::End();
    }

    ImGuiConsoleApp *getConsole(){
        return widgetConsole;
    }

    TexturePackage *getImGuiTextures() {
        return &icons;
    }

    void setSelectedObjectIndex(int selectedObjectIndex) {
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

    [[nodiscard]] ImGuizmo::OPERATION getGuizmoOperation() const {
        return guizmoOperation;
    }

    void setGuizmoOperation(ImGuizmo::OPERATION guizmoOperation) {
        GUIManager::guizmoOperation = guizmoOperation;
    }
};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
