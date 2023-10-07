
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include <dirent.h>
#include <vector>
#include "../Objects/Object3D.h"
#include "imgui.h"
#include "../ComponentsManager.h"
#include "GUIConsole.h"
#include "../Brakeza3D.h"
#include "../Misc/TexturePackage.h"
#include "../Render/Logging.h"

static char editableSource[1024 * 16];

class GUIManager {
private:
    std::vector<Object3D *> &gameObjects;

    int selectedObjectIndex = -1;
    std::string selectedScriptFilename;

    ScriptLUA *script;

    std::string directory_path;
    std::string directory_path_models;

    std::string selected_file;
    ImGuiConsoleApp *console;

    TexturePackage ImGuiTextures;
    
public:

    GUIManager(std::vector<Object3D *> &gameObjects)
    :
        gameObjects(gameObjects),
        directory_path(EngineSetup::get()->SCRIPTS_FOLDER),
        directory_path_models(EngineSetup::get()->MODELS_FOLDER),
        script(nullptr),
        console(new ImGuiConsoleApp(EngineBuffers::get()->getLua()))
    {

        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/stop.png", "stopIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/play.png", "playIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/reload.png", "reloadIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/object.png", "objectIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/light.png", "lightIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/script.png", "scriptIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/swarm.png", "swarmIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/mesh.png", "meshIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/player.png", "playerIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/sprite.png", "spriteIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/remove.png", "removeIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/pause.png", "pauseIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/lock.png", "lockIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/unlock.png", "unlockIcon");
    }

    void drawFiles(std::vector<std::string> result)
    {
        static bool disable_mouse_wheel = false;
        static bool disable_menu = false;

        int items_count = (int) result.size();

        for (int n = 0; n < items_count; n++) {
            auto file = result[n];

            ImGui::PushID(n);
            std::string optionText = std::to_string(n + 1) + ") " + file;
            if (ImGui::Selectable(optionText.c_str())) {
                delete script;
                selectedScriptFilename = file;
                script = new ScriptLUA(selectedScriptFilename, ScriptLUA::dataTypesFileFor(selectedScriptFilename));
                strcpy(editableSource, script->content);
            }
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("SCRIPT_ITEM", file.c_str(), file.length());  // Asigna el nombre del elemento como carga útil
                ImGui::Text("%s", file.c_str());  // Esto es lo que se muestra mientras se arrastra
                ImGui::EndDragDropSource();
            }

            ImGui::PopID();
        }
    }

    void drawImGuiScriptProperties()
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Script properties for: ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", script->scriptFilename.c_str());

        ImGui::Separator();

        if (ImGui::BeginTable("ScriptProperties", 3)) {
            for (int i = 0; i < script->dataTypes.size(); i++) {
                ImGui::TableNextRow();
                auto type = &script->dataTypes[i];

                static char name[256];
                static char value[256];

                strncpy(name, type->name.c_str(), sizeof(name));
                strncpy(value, type->value.c_str(), sizeof(value));

                ImGui::TableSetColumnIndex(0);
                ImGui::InputText(std::string("Name##id" + std::to_string(i)).c_str(), name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll);
                if (ImGui::IsItemEdited()) {
                    type->name = name;
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::InputText(std::string("Value##id" + std::to_string(i)).c_str(), value, IM_ARRAYSIZE(value), ImGuiInputTextFlags_AutoSelectAll);
                if (ImGui::IsItemEdited()) {
                    type->value = value;
                }

                ImGui::TableSetColumnIndex(2);
                if (ImGui::Button(std::string("Remove##id" + std::to_string(i)).c_str())) {
                    script->removeDataType(*type);
                    script->updateFileTypes();
                }
            }

            ImGui::EndTable();
        }

        if (ImGui::Button(std::string("Apply").c_str())) {
            script->updateFileTypes();
        }

    }

    void drawCameraSettings()
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
        ImGui::Separator();
    }

    void drawModelFiles() {
        DIR *dir;
        struct dirent *ent;
        std::vector<std::string> result;
        if ((dir = opendir (directory_path_models.c_str())) != NULL) {
            int index = 1;
            while ((ent = readdir (dir)) != NULL) {
                //if (ent->d_type == DT_DIR) continue;
                result.push_back(ent->d_name);
            }
            std::sort( result.begin(), result.end() );

            closedir (dir);

            for (int i = 0; i < result.size(); i++) {
                auto file = result[i];
                auto title = std::to_string(i-1) + ") " + file;
                if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                    if(ImGui::Selectable(title.c_str(), selected_file == file)){
                        selected_file = file;
                    }

                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                        selected_file = file;
                    }
                }
            }
        } else {
            ImGui::Text("Could not open directory");
        }

        if (ImGui::Button("Load model")) {
            if (!selected_file.empty()) {
                Tools::addSceneObject(selected_file, "added_item");

                selected_file = "";
            } else {
                ImGui::Text("No file selected");
            }
        }
    }

    void drawObjectsWindow()
    {
        bool p_open = true;
        console->Draw("Logging/Console", &p_open);

        if (ImGui::Begin("Scene Objects")) {
            auto title = std::string("Number objects: ") + std::to_string(gameObjects.size());

            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", title.c_str());

            ImGui::Separator();

            for (int i = 0; i < gameObjects.size(); i++) {
                auto o = gameObjects[i];

                if (o->isRemoved()) {
                    continue;
                }

                std::string optionText = std::to_string(i + 1) + ") " + o->getLabel();
                if (ImGui::Selectable(optionText.c_str(), selectedObjectIndex == i)) {
                    selectedObjectIndex = i;
                }
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCRIPT_ITEM")) {
                        Logging::Message("Dropping script (%s) in %s", payload->Data, o->getLabel().c_str());
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        o->attachScript(new ScriptLUA(
                            std::string((char*) payload->Data),
                            ScriptLUA::dataTypesFileFor(std::string((char *)payload->Data)))
                        );
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine(250);
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel(o->getTypeIcon())->getTexture(), ImVec2(16, 16));
                ImGui::SameLine(270);
                ImGui::Text("%s", o->getTypeObject());
            }
        }
        ImGui::End();

        if (ImGui::Begin("Properties")) {
            if (selectedObjectIndex >= 0 && selectedObjectIndex < gameObjects.size()) {
                auto o = gameObjects[selectedObjectIndex];
                if (o->isRemoved()) {
                    return;
                }

                ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Selected object: ");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", o->getLabel().c_str());

                ImGui::Separator();

                auto objectScripts = o->getScripts();

                if ((int) objectScripts.size() <= 0) {
                    ImGui::Text("%s", "No scripts attached");
                }

                for (int i = 0; i < (int) objectScripts.size(); i++) {
                    auto currentScript = objectScripts[i];
                    ImGui::PushID(i);

                    std::string optionText = std::to_string(i + 1) + ") " + currentScript->scriptFilename;

                    ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("scriptIcon")->getTexture(), ImVec2(20, 20));
                    ImGui::SameLine(32);

                    if (ImGui::Button(optionText.c_str())) {
                        delete script;
                        selectedScriptFilename = currentScript->scriptFilename;
                        script = new ScriptLUA(selectedScriptFilename, ScriptLUA::dataTypesFileFor(selectedScriptFilename));
                        strcpy(editableSource, script->content);
                    }

                    ImGui::SameLine();

                    if (currentScript->isPaused()) {
                        if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("unlockIcon")->getTexture(), ImVec2(14, 14))) {
                            currentScript->setPaused(false);
                        }
                    } else {
                        if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("lockIcon")->getTexture(), ImVec2(14, 14))) {
                            currentScript->setPaused(true);
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("removeIcon")->getTexture(), ImVec2(14, 14))) {
                        o->removeScript(currentScript);
                    }

                    ImGui::PopID();
                }

                ImGui::Separator();

                o->drawImGuiProperties();

                ImGui::Button("Remove");
                if (ImGui::IsItemClicked()) {
                    o->setRemoved(true);
                }

            } else {
                ImGui::Text("Select an object...");
            }
        }
        ImGui::End();

        if (ImGui::Begin("Scripts")) {
            DIR *dir;
            struct dirent *ent;
            std::vector<std::string> result;

            if ((dir = opendir (directory_path.c_str())) != NULL) {
                while ((ent = readdir (dir)) != NULL) {
                    if (Tools::getExtensionFromFilename(ent->d_name) == "lua") {
                        result.emplace_back(ent->d_name);
                    }
                }

                std::sort( result.begin(), result.end() );
                closedir (dir);

                drawFiles(result);

            } else {
                ImGui::Text("Could not open directory");
            }

        }
        ImGui::End();

        if (ImGui::Begin("Script Preview")) {
            if (!selectedScriptFilename.empty()) {
                drawImGuiScriptProperties();
                drawImGuiScriptCode();
            }
        }
        ImGui::End();

        if (ImGui::Begin("Variables")) {
            static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
            if (ImGui::BeginTable("GlobalVariablesTable", 2, flags)) {
                auto scripts = ComponentsManager::get()->getComponentRender()->getScripts();
                auto luaEnvironment = ComponentsManager::get()->getComponentRender()->getLuaEnvironment();
                int count = 0;

                for (auto currentScript : scripts) {
                    for (auto & dataType : currentScript->dataTypes) {
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", std::string(dataType.name).c_str());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", std::string(luaEnvironment[dataType.name]).c_str());
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


        if (ImGui::Begin("Project")) {
            auto componentRender = ComponentsManager::get()->getComponentRender();
            static char name[256];

            strncpy(name, EngineSetup::get()->ENGINE_TITLE.c_str(), sizeof(name));

            ImGui::InputText("Project name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll);
            if (ImGui::IsItemEdited()) {
                EngineSetup::get()->ENGINE_TITLE = name;
            }

            ImGui::Separator();

            auto scripts = componentRender->getScripts();
            for (int i = 0; i < (int) scripts.size(); i++) {
                auto currentScript = scripts[i];
                ImGui::PushID(i);
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("scriptIcon")->getTexture(), ImVec2(20, 20));
                ImGui::SameLine(32);
                std::string optionText = std::to_string(i + 1) + ") " + currentScript->scriptFilename;
                if (ImGui::Button(optionText.c_str())) {
                    selectedScriptFilename = currentScript->scriptFilename;
                    delete script;
                    script = new ScriptLUA(selectedScriptFilename, ScriptLUA::dataTypesFileFor(selectedScriptFilename));
                    strcpy(editableSource, script->content);
                }
                ImGui::SameLine();

                if (currentScript->isPaused()) {
                    if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("unlockIcon")->getTexture(), ImVec2(14, 14))) {
                        currentScript->setPaused(false);
                    }
                } else {
                    if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("lockIcon")->getTexture(), ImVec2(14, 14))) {
                        currentScript->setPaused(true);
                    }
                }
                ImGui::SameLine();
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("removeIcon")->getTexture(), ImVec2(14, 14))) {
                    componentRender->removeScript(currentScript);
                }

                ImGui::PopID();
            }

            ImGui::Separator();

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Drop here LUA scripts...");
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCRIPT_ITEM")) {
                    Logging::Message("Dropping script (%s) in global space", payload->Data);
                    componentRender->addLUAScript(new ScriptLUA(
                        std::string((char*) payload->Data),
                        ScriptLUA::dataTypesFileFor(std::string((char *)payload->Data)))
                    );
                }
                ImGui::EndDragDropTarget();
            }
        }
        ImGui::End();


        if (ImGui::Begin("Camera settings")) {
            drawCameraSettings();
        }
        ImGui::End();


        if (ImGui::Begin("Models")) {
            drawModelFiles();
        }
        ImGui::End();

    }

    void drawImGuiScriptCode()
    {
        ImGui::Separator();

        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
        ImGui::InputTextMultiline("##source", editableSource, IM_ARRAYSIZE(editableSource), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 25), flags);
        if (ImGui::Button(std::string("Save").c_str())) {
            script->updateScriptCodeWith(editableSource);
            script->reloadScriptCode();
        }
    }

    virtual void draw(float timedelta, bool &finish)
    {
        //bool show_demo_window = true;
        //ImGui::ShowDemoWindow(&show_demo_window);

        {
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

            drawObjectsWindow();

            RenderMenu(finish);
            ToolbarUI();

            ImGui::End();
        }
    }

    void RenderMenu(bool &finish)
    {

        auto cam = ComponentsManager::get()->getComponentCamera()->getCamera();

        bool show_about_window = false;

        ImVec4 FOG_IMGUI_COLOR;

        const float range_min_lightmap_intensity = 0;
        const float range_max_lightmap_intensity = 1;
        const float range_sensibility_lightmap_intensity = 0.0001;

        const float range_min_fog_intensity = 0;
        const float range_max_fog_intensity = 1;
        const float range_sensibility_fog_intensity = 0.1;

        const float range_min_fog_distance = 0;
        const float range_max_fog_distance = 50000;
        const float range_sensibility_fog_distance = 5;

        const float range_sensibility_lightnin = 0.5;
        const float range_sensibility_lightnin_min = -10000;
        const float range_sensibility_lightnin_max = 100000;

        const float range_sensibility = 0.75f;
        const float range_test_sensibility = 0.1;
        const float range_max_sensibility = 999;
        const float range_min_sensibility = -999;

        const float range_sensibility_volume = 1;
        const float range_min_volume = 1;
        const float range_max_volume = 128;

        const float range_frustum_fardistance_sensibility = 1;
        const float range_min_frustum_fardistance = 1;
        const float range_max_frustum_fardistance = 10000;

        const float range_frustum_clipping_distance_sensibility = 0.00001;
        const float range_min_frustum_clipping_distance = 0;
        const float range_max_frustum_clipping_distance = 1;

        const int range_framerate_sensibility = 1;
        const int range_min_framerate_distance = 0;
        const int range_max_framerate_distance = 500;

        const float range_fov_sensibility = 1;
        const float range_min_fov = 20;
        const float range_max_fov = 160;

        bool changedFOGcolor = false;
        int misc_flags = ImGuiColorEditFlags_NoOptions;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Brakeza3D")) {
                if (ImGui::MenuItem("About Brakeza", "CTRL+I")) show_about_window = true;
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "CTRL+W")) finish = true;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Render")) {
                ImGui::Separator();

                ImGui::Checkbox("Limit frame rate", &EngineSetup::get()->LIMIT_FRAMERATE);
                if (EngineSetup::get()->LIMIT_FRAMERATE) {
                    ImGui::DragScalar("Limite frames to:", ImGuiDataType_S32, &EngineSetup::get()->FRAMERATE, range_framerate_sensibility, &range_min_framerate_distance,&range_max_framerate_distance, "%d", 1.0f);

                }
                ImGui::Separator();

                ImGui::Checkbox("update Objects", &EngineSetup::get()->EXECUTE_GAMEOBJECTS_ONUPDATE);
                ImGui::Checkbox("Draw Main Z-Buffer", &EngineSetup::get()->DRAW_MAIN_DEEP_MAPPING);

                ImGui::Separator();
                ImGui::DragScalar("FOV", ImGuiDataType_Float, &EngineSetup::get()->HORIZONTAL_FOV, range_fov_sensibility, &range_min_fov, &range_max_fov, "%f", 1.0f);

                if (ImGui::IsItemEdited()) {
                    cam->getFrustum()->setup(
                            cam->getPosition(),
                            Vertex3D(0, 0, 1),
                            EngineSetup::get()->up,
                            EngineSetup::get()->right,
                            EngineSetup::get()->HORIZONTAL_FOV,
                            ((float) EngineSetup::get()->screenHeight / (float) EngineSetup::get()->screenWidth),
                            EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE
                    );
                    cam->updateFrustum();
                }
                ImGui::Separator();

                ImGui::Checkbox("Tiled Based", &EngineSetup::get()->BASED_TILE_RENDER);

                if (EngineSetup::get()->BASED_TILE_RENDER) {
                    ImGui::Checkbox("Tiled Based Threaded", &EngineSetup::get()->BASED_TILE_RENDER_THREADED);
                    ImGui::Checkbox("Show Tiles Grid", &EngineSetup::get()->DRAW_TILES_GRID);
                }
                ImGui::Separator();
                ImGui::Checkbox("Frustum Clipping", &EngineSetup::get()->ENABLE_CLIPPING);
                if (EngineSetup::get()->ENABLE_CLIPPING) {
                    ImGui::DragScalar("Frustum Clipping Distance", ImGuiDataType_Float,&EngineSetup::get()->FRUSTUM_CLIPPING_DISTANCE,range_frustum_clipping_distance_sensibility, &range_min_frustum_clipping_distance,&range_max_frustum_clipping_distance, "%f", 1.0f);
                    ImGui::DragScalar("EPSILON", ImGuiDataType_Float, &EngineSetup::get()->EPSILON,range_frustum_clipping_distance_sensibility, &range_min_frustum_clipping_distance,&range_max_frustum_clipping_distance, "%f", 1.0f);
                }

                ImGui::Separator();
                ImGui::DragScalar("Frustum FarDistance", ImGuiDataType_Float,&EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE,range_frustum_fardistance_sensibility, &range_min_frustum_fardistance,&range_max_frustum_fardistance, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    cam->getFrustum()->setup(
                            cam->getPosition(),
                            Vertex3D(0, 0, 1),
                            EngineSetup::get()->up,
                            EngineSetup::get()->right,
                            EngineSetup::get()->HORIZONTAL_FOV,
                            ((float) EngineSetup::get()->screenHeight / (float) EngineSetup::get()->screenWidth),
                            EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE
                    );
                    cam->updateFrustum();
                }

                ImGui::Separator();

                ImGui::Checkbox("Enable FOG", &EngineSetup::get()->ENABLE_FOG);
                changedFOGcolor = ImGui::ColorEdit4("FOG Color", (float *) &FOG_IMGUI_COLOR,
                                                    misc_flags);
                if (changedFOGcolor) {
                    EngineSetup::get()->FOG_COLOR = Color(
                            FOG_IMGUI_COLOR.x * 256,
                            FOG_IMGUI_COLOR.y * 256,
                            FOG_IMGUI_COLOR.z * 256
                    );
                }

                ImGui::Separator();

                ImGui::DragScalar("FOG Intensity", ImGuiDataType_Float, &EngineSetup::get()->FOG_INTENSITY,range_sensibility_fog_intensity, &range_min_fog_intensity, &range_max_fog_intensity,"%f", 1.0f);
                ImGui::DragScalar("FOG Distance", ImGuiDataType_Float, &EngineSetup::get()->FOG_DISTANCE,range_sensibility_fog_distance, &range_min_fog_distance, &range_max_fog_distance,"%f", 1.0f);

                ImGui::Separator();
                ImGui::Checkbox("Vertex", &EngineSetup::get()->TRIANGLE_MODE_PIXELS);
                ImGui::Checkbox("WireFrame", &EngineSetup::get()->TRIANGLE_MODE_WIREFRAME);
                ImGui::Checkbox("Solid", &EngineSetup::get()->TRIANGLE_MODE_COLOR_SOLID);
                ImGui::Separator();
                ImGui::Checkbox("Textures", &EngineSetup::get()->TRIANGLE_MODE_TEXTURIZED);
                if (EngineSetup::get()->TRIANGLE_MODE_TEXTURIZED) {
                    ImGui::Separator();
                    ImGui::Checkbox("UV Bilinear", &EngineSetup::get()->TEXTURES_BILINEAR_INTERPOLATION);
                    ImGui::Separator();
                    ImGui::Checkbox("Depth Of Field", &EngineSetup::get()->ENABLE_DEPTH_OF_FIELD);
                    if (EngineSetup::get()->ENABLE_DEPTH_OF_FIELD) {
                        auto shader = ComponentsManager::get()->getComponentRender()->shaderDepthOfField;
                        ImGui::DragScalar("focusPlaneDepth", ImGuiDataType_Float, &shader->focusPlaneDepth, 10.0f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                        ImGui::DragScalar("focusRange", ImGuiDataType_Float, &shader->focusRange, 10.0f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                        ImGui::DragScalar("blurSize", ImGuiDataType_Float, &shader->blurSize, 0.000001f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                        ImGui::DragScalar("intensity", ImGuiDataType_Float, &shader->intensity, 0.0001f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    }
                }
                ImGui::Separator();
                ImGui::Checkbox("Draw Bones", &EngineSetup::get()->DRAW_ANIMATION_BONES);
                ImGui::Separator();
                ImGui::Checkbox("Internal click selection", &EngineSetup::get()->CLICK_SELECT_OBJECT3D);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Sound")) {
                ImGui::Checkbox("Global enable", &EngineSetup::get()->SOUND_ENABLED);
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    if (!EngineSetup::get()->SOUND_ENABLED) {
                        Mix_Volume(EngineSetup::SoundChannels::SND_GLOBAL, 0);
                        Mix_VolumeMusic((int) EngineSetup::get()->SOUND_VOLUME_MUSIC);
                        Mix_VolumeMusic(0);
                    } else {
                        Mix_Volume(EngineSetup::SoundChannels::SND_GLOBAL, (int) EngineSetup::get()->SOUND_CHANNEL_GLOBAL);
                        Mix_VolumeMusic((int) EngineSetup::get()->SOUND_VOLUME_MUSIC);
                    }
                }

                ImGui::DragScalar("Music volume", ImGuiDataType_Float, &EngineSetup::get()->SOUND_VOLUME_MUSIC, range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) { Mix_VolumeMusic((int )EngineSetup::get()->SOUND_VOLUME_MUSIC); }

                ImGui::DragScalar("Global Channel volume", ImGuiDataType_Float, &EngineSetup::get()->SOUND_CHANNEL_GLOBAL, range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    Mix_Volume(EngineSetup::SoundChannels::SND_GLOBAL, (int) EngineSetup::get()->SOUND_CHANNEL_GLOBAL);
                }

                /*ImGui::DragScalar("Player vol.", ImGuiDataType_Float, &EngineSetup::get()->SOUND_VOLUME_PLAYER,range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    Mix_Volume(EngineSetup::SoundChannels::SND_PLAYER, EngineSetup::get()->SOUND_VOLUME_PLAYER);
                }

                ImGui::DragScalar("Enemies vol.", ImGuiDataType_Float, &EngineSetup::get()->SOUND_VOLUME_ENEMIES,range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    Mix_Volume(EngineSetup::SoundChannels::SND_ENEMIES, EngineSetup::get()->SOUND_VOLUME_ENEMIES);
                }

                ImGui::DragScalar("Environment vol.", ImGuiDataType_Float,&EngineSetup::get()->SOUND_VOLUME_ENVIRONMENT, range_sensibility_volume,&range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    Mix_Volume(EngineSetup::SoundChannels::SND_ENVIRONMENT,EngineSetup::get()->SOUND_VOLUME_ENVIRONMENT);
                }*/

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Bullet")) {
                ImGui::Checkbox("Step Simulation", &EngineSetup::get()->BULLET_STEP_SIMULATION);
                ImGui::Checkbox("Collisions between objects", &EngineSetup::get()->BULLET_CHECK_ALL_PAIRS);
                ImGui::Separator();
                ImGui::Checkbox("Draw debug mode", &EngineSetup::get()->BULLET_DEBUG_MODE);
                ImGui::Separator();
                // gravity
                ImGui::DragScalar("X Gravity", ImGuiDataType_Float, &EngineSetup::get()->gravity.x, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::DragScalar("Y Gravity", ImGuiDataType_Float, &EngineSetup::get()->gravity.y, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::DragScalar("Z Gravity", ImGuiDataType_Float, &EngineSetup::get()->gravity.z, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::Separator();
                ImGui::DragScalar("Projectile Impulse", ImGuiDataType_Float, &EngineSetup::get()->PROJECTILE_DEMO_IMPULSE, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::DragScalar("Projectile Accuracy", ImGuiDataType_Float, &EngineSetup::get()->PROJECTILE_DEMO_ACCURACY, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::DragScalar("Projectile Mass", ImGuiDataType_Float, &EngineSetup::get()->PROJECTILE_DEMO_MASS, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Logging")) {
                ImGui::Checkbox("Output to Console", &EngineSetup::get()->LOGGING);
                ImGui::Separator();
                ImGui::Checkbox("Show debug data", &EngineSetup::get()->DEBUG_RENDER_INFO);
                ImGui::Separator();
                ImGui::Checkbox("Collision objects", &EngineSetup::get()->LOG_COLLISION_OBJECTS);
                ImGui::Separator();
                ImGui::Checkbox("Weapon System", &EngineSetup::get()->LOG_WEAPONS_SYSTEM);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Lights")) {
                ImGui::Checkbox("Light System", &EngineSetup::get()->ENABLE_LIGHTS);
                ImGui::Checkbox("Create lights Z-Buffer", &EngineSetup::get()->CREATE_LIGHT_ZBUFFER);
                if (EngineSetup::get()->CREATE_LIGHT_ZBUFFER) {
                    ImGui::Checkbox("Lights Deep Test", &EngineSetup::get()->ENABLE_SHADOW_MAPPING);
                }
                ImGui::Separator();
                ImGui::Checkbox("Specularity", &EngineSetup::get()->ENABLE_LIGHTS_SPECULAR);

                ImGui::Separator();
                ImGui::Checkbox("Draw Lights Billboards", &EngineSetup::get()->DRAW_LIGHTPOINTS_BILLBOARD);
                ImGui::Separator();
                ImGui::DragScalar("Lightmap Blend Intensity", ImGuiDataType_Float,
                                  &EngineSetup::get()->LIGHTMAPPING_BLEND_INTENSITY,
                                  range_sensibility_lightmap_intensity, &range_min_lightmap_intensity,
                                  &range_max_lightmap_intensity, "%f", 1.0f);
                ImGui::DragScalar("Lightmap Intensity", ImGuiDataType_Float,
                                  &EngineSetup::get()->LIGHTMAPPING_INTENSITY,
                                  range_sensibility_lightmap_intensity, &range_min_lightmap_intensity,
                                  &range_max_lightmap_intensity, "%f", 1.0f);
                ImGui::EndMenu();
            }


            if (ImGui::BeginMenu("Effects FX")) {
                if (ImGui::BeginMenu("Shader Particles")) {
                    ImGui::DragScalar("PARTICLES_SHADER_GRAVITY", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_GRAVITY, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_STEP_ADD_PARTICLE", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_STEP_ADD_PARTICLE, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_PARTICLE_LIFESPAN", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_PARTICLE_LIFESPAN, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_SMOKE_ANGLE_RANGE", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_SMOKE_ANGLE_RANGE, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_MIN_VELOCITY", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_MIN_VELOCITY, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_MAX_VELOCITY", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_MAX_VELOCITY, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_ALPHA_MIN", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_ALPHA_MIN, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_ALPHA_MAX", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_ALPHA_MAX, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_POSITION_NOISE", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_POSITION_NOISE, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_VELOCITY_NOISE", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_VELOCITY_NOISE, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_DECELERATION_FACTOR", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_DECELERATION_FACTOR, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::Separator();
                    ImGui::DragScalar("SHADER EXPLOSION TTL", ImGuiDataType_Float, &EngineSetup::get()->SHADER_PARTICLE_EXPLOSION_TTL, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("SHADER_PARTICLE_EXPLOSION_EMISSION_TIME", ImGuiDataType_Float, &EngineSetup::get()->SHADER_PARTICLE_EXPLOSION_EMISSION_TIME, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Lightning")) {
                    ImGui::DragScalar("Generations", ImGuiDataType_Float,
                                      &EngineSetup::get()->LIGHTNING_GENERATIONS, range_sensibility_lightnin,
                                      &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("Offset reduction", ImGuiDataType_Float,
                                      &EngineSetup::get()->LIGHTNING_OFFSET_REDUCTION,
                                      range_sensibility_lightnin, &range_sensibility_lightnin_min,
                                      &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("Probability branch", ImGuiDataType_Float,
                                      &EngineSetup::get()->LIGHTNING_PROBABILITY_BRANCH,
                                      range_sensibility_lightnin, &range_sensibility_lightnin_min,
                                      &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("Segment shift ", ImGuiDataType_Float,
                                      &EngineSetup::get()->LIGHTNING_SEGMENT_SHIFT, range_sensibility_lightnin,
                                      &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::Checkbox("FullScreen", &EngineSetup::get()->FULLSCREEN);
                if (ImGui::IsItemEdited()) {
                    if (EngineSetup::get()->FULLSCREEN) {
                        SDL_SetWindowFullscreen(ComponentsManager::get()->getComponentWindow()->getWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);

                    } else {
                        SDL_SetWindowFullscreen(ComponentsManager::get()->getComponentWindow()->getWindow(), 0);
                    }
                }
                ImGui::Separator();
                //ImGui::Checkbox("Camera Inspector", &show_camera_info);
                //ImGui::Checkbox("3D Objects Inspector", &show_window_inspector);
                //ImGui::Checkbox("Camera Inspector", &show_camera_info);
                ImGui::Separator();
                //ImGui::Checkbox("Tiles", &show_window_physics);
                ImGui::Separator();
                ImGui::Checkbox("Draw main Frustum", &EngineSetup::get()->DRAW_FRUSTUM);
                ImGui::Separator();
                ImGui::Checkbox("Draw Main Axis", &EngineSetup::get()->RENDER_MAIN_AXIS);
                ImGui::Checkbox("Draw Object3D Axis", &EngineSetup::get()->RENDER_OBJECTS_AXIS);
                if (EngineSetup::get()->RENDER_OBJECTS_AXIS) {
                    ImGui::Checkbox("Draw Lights Axis", &EngineSetup::get()->DRAW_LIGHTPOINTS_AXIS);
                    ImGui::DragScalar("Size Axis", ImGuiDataType_Float, &EngineSetup::get()->OBJECT_AXIS_SIZE,
                                      range_test_sensibility, &range_min_sensibility, &range_max_sensibility, "%f", 1.0f);

                }
                ImGui::Separator();
                ImGui::Separator();
                ImGui::Checkbox("Draw light direction", &EngineSetup::get()->DRAW_LIGHTS_DIRECTION);
                if (EngineSetup::get()->DRAW_LIGHTS_DIRECTION) {
                    ImGui::DragScalar("Size Direction", ImGuiDataType_Float, &EngineSetup::get()->LIGHTS_DIRECTION_SIZE,
                                      range_test_sensibility, &range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                }

                ImGui::Separator();

                ImGui::Checkbox("Draw Mesh3D AABB", &EngineSetup::get()->DRAW_MESH3D_AABB);
                ImGui::Checkbox("Draw Mesh3D Octree", &EngineSetup::get()->DRAW_MESH3D_OCTREE);
                ImGui::Checkbox("Draw Mesh3D Grid", &EngineSetup::get()->DRAW_MESH3D_GRID);
                if (EngineSetup::get()->DRAW_MESH3D_GRID) {
                    ImGui::Separator();
                    ImGui::Checkbox("Grid3D: Empty", &EngineSetup::get()->DRAW_MESH3D_GRID_EMPTY);
                    ImGui::Checkbox("Grid3D: No Empty", &EngineSetup::get()->DRAW_MESH3D_GRID_NO_EMPTY);
                    ImGui::Separator();
                    ImGui::Checkbox("Grid3D: Draw Center", &EngineSetup::get()->DRAW_MESH3D_GRID_POINTS);
                    ImGui::Checkbox("Grid3D: Draw AABB", &EngineSetup::get()->DRAW_MESH3D_GRID_CUBES);
                    ImGui::Separator();
                }
                ImGui::Separator();
                ImGui::Checkbox("Pendulum thread", &EngineSetup::get()->DRAW_PENDULUM_THREAD);
                ImGui::Checkbox("Draw Object3D Billboards", &EngineSetup::get()->DRAW_OBJECT3D_BILLBOARD);
                ImGui::Checkbox("Draw Decals wireframe", &EngineSetup::get()->DRAW_DECAL_WIREFRAMES);
                ImGui::Separator();
                ImGui::Checkbox("Object3D Text Label", &EngineSetup::get()->TEXT_ON_OBJECT3D);
                ImGui::Separator();
                ImGui::Checkbox("Show CrossHair", &EngineSetup::get()->DRAW_CROSSHAIR);
                ImGui::Separator();
                ImGui::Checkbox("Show FPS", &EngineSetup::get()->DRAW_FPS);
                ImGui::Separator();
                ImGui::EndMenu();
            }
            static const char *itemCurrent = "--Niveles--"; // Here our selection is a single pointer stored outside the object.
            static ImGuiComboFlags flags = 0;

            ImGui::EndMainMenuBar();
        }
    }

    void ToolbarUI()
    {
        if (ImGui::Begin("MainToolBar")) {
            if (ComponentsManager::get()->getComponentRender()->getStateScripts() == EngineSetup::LuaStateScripts::LUA_STOP) {
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("playIcon")->getTexture(), ImVec2(24, 24))) {
                    ComponentsManager::get()->getComponentRender()->playScripts();
                }
                ImGui::SameLine();
            } else {
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("stopIcon")->getTexture(), ImVec2(24, 24))) {
                    ComponentsManager::get()->getComponentRender()->stopScripts();
                }
                ImGui::SameLine();
            }
            if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("reloadIcon")->getTexture(), ImVec2(24, 24))) {
                ComponentsManager::get()->getComponentRender()->reloadScripts();
            }
        }
        ImGui::End();
    }

    ImGuiConsoleApp *getConsole(){
        return console;
    }

};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
