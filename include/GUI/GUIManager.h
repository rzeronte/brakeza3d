
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

    TexturePackage packageIcons;
    TexturePackage imagesFolder;

    std::string selected_file;
    std::string currentVariableToAddName;
    std::string currentVariableToCreateCustomShader;
    ImGuizmo::OPERATION guizmoOperation;

    const char *availableMesh3DShaders[4] = {"Edge", "Blink", "ShockWave", "Tint"};
public:

    void loadImagesFolder() {

        auto images = Tools::getFolderFiles(EngineSetup::get()->IMAGES_FOLDER.c_str(), "png");

        for (auto f: images) {
            imagesFolder.addItem(EngineSetup::get()->IMAGES_FOLDER + f, f);
        }
    }

    GUIManager(std::vector<Object3D *> &gameObjects)
            :
            gameObjects(gameObjects),
            widgetConsole(new ImGuiConsoleApp(LUAManager::get()->getLua())),
            widgetObjects3D(new GUIWidgetObjects3D(packageIcons, this->gameObjects)),
            widgetObject3DProperties(new GUIWidgetObject3DProperties(packageIcons, this->gameObjects, scriptEditableManager)),
            widgetProjectSettings(new GUIWidgetProjectSettings(packageIcons, scriptEditableManager)),
            widgetMenu(new GUIWidgetMenu(packageIcons)),
            widgetToolbar(new GUIWidgetToolbar(packageIcons)),
            guizmoOperation(ImGuizmo::TRANSLATE)
    {
        LoadUIIcons();
        loadImagesFolder();
    }

    void LoadUIIcons()
    {
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/stop.png", "stopIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/play.png", "playIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/reload.png", "reloadIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/rebuild.png", "rebuildIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/object.png", "objectIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/light.png", "lightIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/script.png", "scriptIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/swarm.png", "swarmIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/mesh.png", "meshIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/folder.png", "folderIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/player.png", "playerIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/sprite3ddirectional.png", "sprite3DDirectionalIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/sprite2d.png", "image2DIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/sprite2d.png", "sprite2DIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/sprite3d.png", "sprite3DIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/remove.png", "removeIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/pause.png", "pauseIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/lock.png", "lockIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/unlock.png", "unlockIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/add.png", "addIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/scene.png", "sceneIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/save.png", "saveIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/gear.png", "gearIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/ghost.png", "ghostIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/shader.png", "shaderIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/spotlight.png", "spotLightIcon");
        packageIcons.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/particles.png", "particlesIcon");
    }

    void drawScriptFiles(std::vector<std::string> result)
    {
        static bool disable_mouse_wheel = false;
        static bool disable_menu = false;

        int items_count = (int) result.size();

        for (int n = 0; n < items_count; n++) {
            auto file = result[n];

            ImGui::PushID(n);
            std::string optionText = std::to_string(n + 1) + ") " + file;
            if (ImGui::Selectable(optionText.c_str())) {
                delete scriptEditableManager.script;
                scriptEditableManager.selectedScriptFilename = file;
                scriptEditableManager.script = new ScriptLUA(
                        scriptEditableManager.selectedScriptFilename,
                        ScriptLUA::dataTypesFileFor(scriptEditableManager.selectedScriptFilename)
                );
                strcpy(scriptEditableManager.editableSource, scriptEditableManager.script->content.c_str());
            }
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("SCRIPT_ITEM", file.c_str(), file.size() + 1);  // Asigna el nombre del elemento como carga útil
                ImGui::Text("%s", file.c_str());  // Esto es lo que se muestra mientras se arrastra
                ImGui::EndDragDropSource();
            }

            ImGui::PopID();
        }
    }

    void drawWidgetScriptProperties()
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Script properties for: ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", scriptEditableManager.script->scriptFilename.c_str());

        ImGui::Separator();
        static char name[256];

        strncpy(name, currentVariableToAddName.c_str(), sizeof(name));

        if (ImGui::InputText("Variable name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
            currentVariableToAddName = name;
        }

        const char* items[] = { "int", "float", "Vertex3D" };
        static int selectedItem = 0;
        ImGui::Combo("Type", &selectedItem, items, IM_ARRAYSIZE(items));

        if (ImGui::Button(std::string("Create").c_str())) {
            LUADataValue LUAValue;
            scriptEditableManager.script->addDataTypeEmpty(currentVariableToAddName.c_str(), items[selectedItem]);
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


        std::string position_text = "Position##1";
        std::string rotation_text = "Orientation##2";
        std::string movement_text = "Keyboard##3";
        std::string mouse_text = "Mouse##4";
        std::string autoscrollSpeed_text = "AutoScroll##5";

        // position
        if (ImGui::TreeNode(position_text.c_str())) {
            ImGui::DragScalar("X", ImGuiDataType_Float, &camera->getPosition().x, range_sensibility, &range_min,&range_max, "%f", 1.0f);
            ImGui::DragScalar("Y", ImGuiDataType_Float, &camera->getPosition().y, range_sensibility, &range_min,&range_max, "%f", 1.0f);
            ImGui::DragScalar("Z", ImGuiDataType_Float, &camera->getPosition().z, range_sensibility, &range_min,&range_max, "%f", 1.0f);
            ImGui::TreePop();
        }

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
        ImGui::Checkbox("Free Look", &ComponentsManager::get()->getComponentCamera()->freeLook);

        ImGui::Separator();
    }

    void drawScenesFiles() {

        std::vector<std::string> result = Tools::getFolderFiles(EngineSetup::get()->SCENES_FOLDER, "json");

        std::sort( result.begin(), result.end() );

        for (int i = 0; i < result.size(); i++) {
            auto file = result[i];
            auto title = std::to_string(i+1) + ") " + file;
            if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                ImGui::PushID(i);
                if (ImGui::ImageButton((ImTextureID)packageIcons.getTextureByLabel("sceneIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                    ComponentsManager::get()->getComponentRender()->getSceneLoader().clearScene();
                    ComponentsManager::get()->getComponentRender()->getSceneLoader().loadScene(EngineSetup::get()->SCENES_FOLDER + file);
                }

                ImGui::SameLine();

                if (ImGui::ImageButton((ImTextureID)packageIcons.getTextureByLabel("saveIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                    ComponentsManager::get()->getComponentRender()->getSceneLoader().saveScene(file);
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
            ComponentsManager::get()->getComponentRender()->addShaderToScene(
                    ShaderOpenGLCustom::createEmptyCustomShader(currentVariableToCreateCustomShader)
            );
        }

        ImGui::Separator();

        auto shaders = ComponentsManager::get()->getComponentRender()->getSceneShaders();

        for (int i = 0; i < shaders.size(); i++) {
            auto s = shaders[i];
            ImGui::PushID(i);
            ImGui::Image((ImTextureID)packageIcons.getTextureByLabel("shaderIcon")->getOGLTextureID(), ImVec2(26, 26));
            ImGui::SameLine(46);
            if (!s->isEnabled()) {
                if (ImGui::ImageButton((ImTextureID)packageIcons.getTextureByLabel("unlockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                    s->setEnabled(true);
                }
            } else {
                if (ImGui::ImageButton((ImTextureID)packageIcons.getTextureByLabel("lockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                    s->setEnabled(false);
                }
            }
            ImGui::SameLine();
            if (ImGui::ImageButton((ImTextureID)packageIcons.getTextureByLabel("rebuildIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                s->compile();
            }
            ImGui::SameLine();
            if (ImGui::ImageButton((ImTextureID)packageIcons.getTextureByLabel("removeIcon")->getOGLTextureID(), ImVec2(14, 14))) {
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

        ImGui::Separator();
        if (ImGui::Begin("Custom Shaders")) {
            std::vector<std::string> result = Tools::getFolderFiles(EngineSetup::get()->CUSTOM_SHADERS, "fs");

            std::sort( result.begin(), result.end() );

            for (int i = 0; i < result.size(); i++) {
                auto file = result[i];
                auto title = std::to_string(i+1) + ") " + file;
                if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                    ImGui::PushID(i);
                    if (ImGui::ImageButton((ImTextureID)packageIcons.getTextureByLabel("shaderIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                        std::string name = Tools::getFilenameWithoutExtension(file.c_str());
                        ComponentsManager::get()->getComponentRender()->addShaderToScene(
                                new ShaderOpenGLCustom(name, EngineSetup::get()->CUSTOM_SHADERS + file)
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
            std::vector<std::string> result = Tools::getFolderFiles(EngineSetup::get()->SCRIPTS_FOLDER, "lua");
            std::sort( result.begin(), result.end() );
            drawScriptFiles(result);
        }
        ImGui::End();

        if (ImGui::Begin("Script Preview")) {
            if (!scriptEditableManager.selectedScriptFilename.empty()) {
                drawWidgetScriptProperties();
                drawImGuiScriptCode();
            }
        }
        ImGui::End();

        if (ImGui::Begin("Variables")) {
            static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
            if (ImGui::BeginTable("GlobalVariablesTable", 2, flags)) {
                auto scripts = ComponentsManager::get()->getComponentRender()->getLUAScripts();
                auto &lua = LUAManager::get()->getLua();
                int count = 0;

                for (auto currentScript : scripts) {
                    for (auto & dataType : currentScript->dataTypes) {
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", std::string(dataType.name).c_str());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", std::string(lua[dataType.name]).c_str());
                        count++;
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
            auto tableName = "ImageProperties###";
            static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;

            auto items = imagesFolder.getItems();
            for (int i = 0; i < items.size(); i++) {
                auto a = items[i];

                ImGui::Image((ImTextureID)a->texture->getOGLTextureID(), ImVec2(96, 96));
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
        ImGui::InputTextMultiline("##source", scriptEditableManager.editableSource, IM_ARRAYSIZE(scriptEditableManager.editableSource), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 40), flags);
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
        ImGuiIO& io = ImGui::GetIO();
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        bool p_open = true;
        widgetConsole->Draw("Logging/Console", &p_open);
        widgetObjects3D->draw(selectedObjectIndex);
        widgetObject3DProperties->draw(selectedObjectIndex, guizmoOperation);
        widgetMenu->draw(finish);
        widgetToolbar->draw();

        drawWidgets();

        ImGui::End();
    }

    ImGuiConsoleApp *getConsole(){
        return widgetConsole;
    }

    TexturePackage *getImGuiTextures() {
        return &packageIcons;
    }

    void setSelectedObjectIndex(int selectedObjectIndex) {
        GUIManager::selectedObjectIndex = selectedObjectIndex;
    }

    ImGuizmo::OPERATION getGuizmoOperation() const {
        return guizmoOperation;
    }

    void setGuizmoOperation(ImGuizmo::OPERATION guizmoOperation) {
        GUIManager::guizmoOperation = guizmoOperation;
    }
};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
