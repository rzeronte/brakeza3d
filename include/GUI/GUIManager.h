
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include <string>
#include <dirent.h>
#include <vector>
#include "../Objects/Object3D.h"
#include "imgui.h"
#include "../ComponentsManager.h"
#include "GUIConsole.h"
#include "../Brakeza3D.h"
#include "../Misc/TexturePackage.h"
#include "../Render/Logging.h"
#include "../Shaders/ShaderEdgeObject.h"
#include "../Shaders/ShaderBlink.h"

static char editableSource[1024 * 16];

class GUIManager {
private:
    std::vector<Object3D *> &gameObjects;

    int selectedObjectIndex = -1;
    std::string selectedScriptFilename;
    ShaderOpenCL *selectedShader;
    ScriptLUA *script;

    std::string directory_path;
    std::string directory_path_models;
    std::string directory_path_images;
    std::string directory_path_scenes;

    std::string selected_file;
    ImGuiConsoleApp *console;

    TexturePackage ImGuiTextures;
    TexturePackage imagesFolder;

    const char *availableMesh3DShaders[2] = {"Edge", "Blink"};
public:

    void loadImagesFolder() {
        DIR *dir;
        struct dirent *ent;

        if ((dir = opendir (directory_path_images.c_str())) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                if (Tools::getExtensionFromFilename(ent->d_name) == "png") {
                    imagesFolder.addItem(EngineSetup::get()->IMAGES_FOLDER + ent->d_name, ent->d_name);
                }
            }

            closedir (dir);
        }
    }

    GUIManager(std::vector<Object3D *> &gameObjects)
    :
        gameObjects(gameObjects),
        directory_path(EngineSetup::get()->SCRIPTS_FOLDER),
        directory_path_models(EngineSetup::get()->MODELS_FOLDER),
        directory_path_images(EngineSetup::get()->IMAGES_FOLDER),
        directory_path_scenes(EngineSetup::get()->SCENES_FOLDER),
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
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/sprite2d.png", "sprite2DIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/sprite3d.png", "sprite3DIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/remove.png", "removeIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/pause.png", "pauseIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/lock.png", "lockIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/unlock.png", "unlockIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/add.png", "addIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/scene.png", "sceneIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/save.png", "saveIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/gear.png", "gearIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/ghost.png", "ghostIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/shader.png", "shaderIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/spotlight.png", "spotLightIcon");
        ImGuiTextures.addItem(EngineSetup::get()->ICONS_FOLDER + "interface/particles.png", "particlesIcon");

        loadImagesFolder();
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
                ImGui::SetDragDropPayload("SCRIPT_ITEM", file.c_str(), file.size() + 1);  // Asigna el nombre del elemento como carga útil
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
        ImGui::Checkbox("Free Look", &ComponentsManager::get()->getComponentCamera()->freeLook);

        ImGui::Separator();
    }

    void drawScenesFiles() {
        DIR *dir;
        struct dirent *ent;
        std::vector<std::string> result;
        if ((dir = opendir (directory_path_scenes.c_str())) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                if (Tools::getExtensionFromFilename(ent->d_name) == "json") {
                    result.emplace_back(ent->d_name);
                }
            }
            std::sort( result.begin(), result.end() );

            closedir (dir);

            for (int i = 0; i < result.size(); i++) {
                auto file = result[i];
                auto title = std::to_string(i+1) + ") " + file;
                if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                    ImGui::PushID(i);
                    if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("sceneIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                        ComponentsManager::get()->getComponentRender()->getSceneLoader().loadScene(file);
                    }
                    ImGui::SameLine();
                    if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("saveIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                        ComponentsManager::get()->getComponentRender()->getSceneLoader().saveScene(file);
                    }
                    ImGui::SameLine();

                    ImGui::Text("%s", title.c_str());
                    ImGui::PopID();
                }
            }
        } else {
            ImGui::Text("Could not open directory");
        }

    }

    void drawSceneShaders() {

        auto shaders = ComponentsManager::get()->getComponentRender()->getSceneShaders();

        for (int i = 0; i < shaders.size(); i++) {
            auto s = shaders[i];
            ImGui::PushID(i);
            ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("shaderIcon")->getOGLTextureID(), ImVec2(26, 26));
            ImGui::SameLine(46);
            if (!s->isEnabled()) {
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("unlockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                    s->setEnabled(true);
                }
            } else {
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("lockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                    s->setEnabled(false);
                }
            }
            ImGui::SameLine();
            if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("removeIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                ComponentsManager::get()->getComponentRender()->removeShader(i);
            }
            ImGui::SameLine();
            if (ImGui::CollapsingHeader(s->getLabel().c_str(), ImGuiTreeNodeFlags_None)) {
                ImGui::PushID(i);
                s->drawImGuiProperties();
                ImGui::PopID();
            }
            ImGui::PopID();
            ImGui::Separator();
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
                            std::string((const char*) payload->Data),
                            ScriptLUA::dataTypesFileFor(std::string((char *)payload->Data)))
                        );
                    }
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESH3D_SHADER_ITEM")) {
                        Logging::Message("Dropping shader (%s) in %s", payload->Data, o->getLabel().c_str());
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        auto mesh = dynamic_cast<Mesh3D*> (o);
                        if (mesh) {
                            int selection = std::stoi((char*) payload->Data);
                            switch(selection) {
                                case 0: {
                                    auto shader = new ShaderEdgeObject(true, mesh, Color::green(), 1);
                                    mesh->addMesh3DShader(shader);
                                }
                                case 1: {
                                    auto shader = new ShaderBlink(true, mesh, 0.1f ,Color::green());
                                    mesh->addMesh3DShader(shader);
                                    break;
                                }

                            }
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::SameLine();
                if (o->isEnabled()) {
                    ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("lockIcon")->getOGLTextureID(), ImVec2(14, 14));
                } else {
                    ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("unlockIcon")->getOGLTextureID(), ImVec2(14, 14));
                }
                ImGui::SameLine();

                ImGui::SameLine(200);
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel(o->getTypeIcon())->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine(220);
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
                ImGui::SameLine();
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel(o->getTypeIcon())->getOGLTextureID(), ImVec2(16, 16));

                ImGui::Separator();

                o->drawImGuiProperties();

                ImGui::Separator();

                auto objectScripts = o->getScripts();

                if ((int) objectScripts.size() <= 0) {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No scripts attached");
                }

                for (int i = 0; i < (int) objectScripts.size(); i++) {
                    auto currentScript = objectScripts[i];
                    ImGui::PushID(i);

                    std::string optionText = std::to_string(i + 1) + ") " + currentScript->scriptFilename;

                    ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("scriptIcon")->getOGLTextureID(), ImVec2(24, 24));
                    ImGui::SameLine(48);

                    if (ImGui::Button(optionText.c_str())) {
                        delete script;
                        selectedScriptFilename = currentScript->scriptFilename;
                        script = new ScriptLUA(selectedScriptFilename, ScriptLUA::dataTypesFileFor(selectedScriptFilename));
                        strcpy(editableSource, script->content);
                    }

                    ImGui::SameLine();

                    if (currentScript->isPaused()) {
                        if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("unlockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                            currentScript->setPaused(false);
                        }
                    } else {
                        if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("lockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                            currentScript->setPaused(true);
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("removeIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                        o->removeScript(currentScript);
                    }

                    ImGui::PopID();
                }

                ImGui::Separator();


                ImGui::Button("Remove");
                if (ImGui::IsItemClicked()) {
                    o->setRemoved(true);
                }
                ImGui::SameLine();

                ImGui::Button("Get JSON!");
                if (ImGui::IsItemClicked()) {
                    Logging::Message("getJSON");
                    std::string json = cJSON_Print(o->getJSON());
                    Logging::Message(json.c_str());
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
                auto scripts = ComponentsManager::get()->getComponentRender()->getLUAScripts();
                auto &lua = EngineBuffers::get()->getLua();
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


        if (ImGui::Begin("Project")) {
            auto componentRender = ComponentsManager::get()->getComponentRender();
            static char name[256];

            strncpy(name, EngineSetup::get()->ENGINE_TITLE.c_str(), sizeof(name));

            ImGui::InputText("Project name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll);
            if (ImGui::IsItemEdited()) {
                EngineSetup::get()->ENGINE_TITLE = name;
            }
            ImGui::Separator();

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

                if (ImGui::TreeNode("Ambient")) {
                    ImGui::DragScalar("x", ImGuiDataType_Float, &dirLight->ambient.x, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("y", ImGuiDataType_Float, &dirLight->ambient.y, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("z", ImGuiDataType_Float, &dirLight->ambient.z, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Diffuse")) {
                    ImGui::DragScalar("x", ImGuiDataType_Float, &dirLight->diffuse.x, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("y", ImGuiDataType_Float, &dirLight->diffuse.y, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("z", ImGuiDataType_Float, &dirLight->diffuse.z, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Specular")) {
                    ImGui::DragScalar("x", ImGuiDataType_Float, &dirLight->specular.x, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("y", ImGuiDataType_Float, &dirLight->specular.y, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("z", ImGuiDataType_Float, &dirLight->specular.z, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::TreePop();
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

                ImGui::DragScalar("Focal range", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->focalRange, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
                ImGui::DragScalar("Focal distance", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->focalDistance, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
                ImGui::DragScalar("Blur radius", ImGuiDataType_S32, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->blurRadius,1.0f, &minBlurRadius, &maxBlurRadius, "%d", 1.0f);
                ImGui::DragScalar("Intensity", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->intensity, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
                ImGui::DragScalar("Far Plane", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->farPlane, depthValueSens, &depthMinValues, &depthMaxValues, "%f", 1.0f);
                ImGui::Separator();
                ImGui::Checkbox("Depth of Field", &EngineSetup::get()->ENABLE_DEPTH_OF_FIELD);
                if (EngineSetup::get()->ENABLE_DEPTH_OF_FIELD) {
                    ImGui::Checkbox("Show Depth Map", &EngineSetup::get()->SHOW_DEPTH_OF_FIELD);
                }
                ImGui::Separator();

                ImGui::TreePop();
            }
            ImGui::Separator();

            auto scripts = componentRender->getLUAScripts();
            for (int i = 0; i < (int) scripts.size(); i++) {
                auto currentScript = scripts[i];
                ImGui::PushID(i);
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("scriptIcon")->getOGLTextureID(), ImVec2(24, 24));
                ImGui::SameLine(48);
                std::string optionText = std::to_string(i + 1) + ") " + currentScript->scriptFilename;
                if (ImGui::Button(optionText.c_str())) {
                    selectedScriptFilename = currentScript->scriptFilename;
                    delete script;
                    script = new ScriptLUA(selectedScriptFilename, ScriptLUA::dataTypesFileFor(selectedScriptFilename));
                    strcpy(editableSource, script->content);
                }
                ImGui::SameLine();

                if (currentScript->isPaused()) {
                    if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("unlockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                        currentScript->setPaused(false);
                    }
                } else {
                    if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("lockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                        currentScript->setPaused(true);
                    }
                }
                ImGui::SameLine();
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("removeIcon")->getOGLTextureID(), ImVec2(14, 14))) {
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
            ImGui::Separator();
        }
        ImGui::End();


        if (ImGui::Begin("Camera settings")) {
            drawCameraSettings();
        }
        ImGui::End();


        if (ImGui::Begin("Shaders")) {
            drawSceneShaders();
        }

        if (ImGui::Begin("Mesh3D Shaders")) {
            drawMesh3DShaders();
        }


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

                ImGui::Separator();
            }
        }
        ImGui::End();
    }

    void drawImGuiScriptCode()
    {
        ImGui::Separator();

        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
        ImGui::InputTextMultiline("##source", editableSource, IM_ARRAYSIZE(editableSource), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10), flags);
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

    void drawMesh3DItemsToLoad() {
        DIR *dir;
        struct dirent *ent;
        std::vector<std::string> result;
        if ((dir = opendir (directory_path_models.c_str())) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                result.emplace_back(ent->d_name);
            }
            std::sort( result.begin(), result.end() );

            closedir (dir);

            for (int i = 0; i < result.size(); i++) {
                auto file = result[i];
                auto title = std::to_string(i-1) + ") " + file;
                if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                    ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("meshIcon")->getOGLTextureID(), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::MenuItem(file.c_str())) {
                        Tools::addSceneObject(file, "added_item");
                    }
                }
            }
        }
    }

    void drawRigidBodiesItemsToLoad() {
        DIR *dir;
        struct dirent *ent;
        std::vector<std::string> result;
        if ((dir = opendir (directory_path_models.c_str())) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                result.emplace_back(ent->d_name);
            }
            std::sort( result.begin(), result.end() );

            closedir (dir);

            for (int i = 0; i < result.size(); i++) {
                auto file = result[i];
                auto title = std::to_string(i-1) + ") " + file;
                if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                    ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("gearIcon")->getOGLTextureID(), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::MenuItem(file.c_str())) {
                        SceneLoader::createMesh3DBodyToScene(file, "added_item");
                    }
                }
            }
        }
    }

    void drawGhostItemsToLoad() {
        DIR *dir;
        struct dirent *ent;
        std::vector<std::string> result;
        if ((dir = opendir (directory_path_models.c_str())) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                result.emplace_back(ent->d_name);
            }
            std::sort( result.begin(), result.end() );

            closedir (dir);

            for (int i = 0; i < result.size(); i++) {
                auto file = result[i];
                auto title = std::to_string(i-1) + ") " + file;
                if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                    ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("ghostIcon")->getOGLTextureID(), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::MenuItem(file.c_str())) {
                        SceneLoader::createGhostBody3DToScene(file, "added_item");
                    }
                }
            }
        }
    }

    void RenderMenu(bool &finish)
    {

        auto cam = ComponentsManager::get()->getComponentCamera()->getCamera();

        bool show_about_window = false;

        ImVec4 FOG_IMGUI_COLOR;

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

        const float range_far_plane_distance_sensibility = 1.0f;
        const float range_far_plane_min = 0;
        const float range_max_plane_max = 1000000;

        const int range_framerate_sensibility = 1;
        const int range_min_framerate_distance = 0;
        const int range_max_framerate_distance = 500;

        bool changedFOGcolor = false;
        int misc_flags = ImGuiColorEditFlags_NoOptions;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Brakeza3D")) {
                if (ImGui::MenuItem("About Brakeza", "CTRL+I")) show_about_window = true;
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "CTRL+W")) finish = true;
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Add object")) {
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("objectIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Object3D", "CTRL+O")) {
                    SceneLoader::createObjectInScene();
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("lightIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("PointLight", "CTRL+O")) {
                    SceneLoader::createPointLight3DInScene();
                    Logging::Message("Add PointLight");
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("spotLightIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("SpotLight", "CTRL+O")) {
                    SceneLoader::createSpotLight3DInScene();
                    Logging::Message("Add SpotLight");
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("meshIcon")->getOGLTextureID(), ImVec2(16, 16));

                ImGui::SameLine();
                if (ImGui::BeginMenu("Mesh3D")) {
                    drawMesh3DItemsToLoad();
                    ImGui::EndMenu();
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("gearIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::BeginMenu("RigidBody")) {
                    drawRigidBodiesItemsToLoad();
                    ImGui::EndMenu();
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("ghostIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::BeginMenu("GhostBody")) {
                    drawGhostItemsToLoad();
                    ImGui::EndMenu();
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("particlesIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("ParticleEmitter", "CTRL+x")) {
                    SceneLoader::createParticleEmitterInScene();
                    ImGui::EndMenu();
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("sprite3DIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Sprite3D", "CTRL+2")) {
                    SceneLoader::createSprite3DInScene();
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("sprite2DIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Sprite2D", "CTRL+2")) {
                    SceneLoader::createSprite2DInScene();
                }

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

                ImGui::Checkbox("Tiled Based", &EngineSetup::get()->BASED_TILE_RENDER);

                if (EngineSetup::get()->BASED_TILE_RENDER) {
                    ImGui::Checkbox("Tiled Based Threaded", &EngineSetup::get()->BASED_TILE_RENDER_THREADED);
                    ImGui::Checkbox("Show Tiles Grid", &EngineSetup::get()->DRAW_TILES_GRID);
                }
                ImGui::Separator();
                ImGui::Checkbox("Frustum Clipping", &EngineSetup::get()->ENABLE_CLIPPING);
                if (EngineSetup::get()->ENABLE_CLIPPING) {
                    ImGui::DragScalar("Frustum Far Plane Distance", ImGuiDataType_Float, &EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE, range_far_plane_distance_sensibility, &range_far_plane_min, &range_max_plane_max, "%f", 1.0f);
                    ImGui::DragScalar("EPSILON", ImGuiDataType_Float, &EngineSetup::get()->EPSILON, range_far_plane_distance_sensibility, &range_far_plane_min, &range_max_plane_max, "%f", 1.0f);
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
                ImGui::Separator();
                ImGui::Checkbox("Draw Lights Billboards", &EngineSetup::get()->DRAW_LIGHTPOINTS_BILLBOARD);
                ImGui::Separator();
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
                ImGui::Checkbox("Draw Main Axis", &EngineSetup::get()->RENDER_MAIN_AXIS);
                ImGui::Checkbox("Draw Object3D Axis", &EngineSetup::get()->RENDER_OBJECTS_AXIS);
                if (EngineSetup::get()->RENDER_OBJECTS_AXIS) {
                    const float focalMinValues = 0;
                    const float focalMaxValues = 1000;
                    const float focalValueSens = 0.01;

                    ImGui::DragScalar("Size Axis", ImGuiDataType_Float, &EngineSetup::get()->OBJECT_AXIS_SIZE,
                                      focalMinValues, &focalMinValues, &focalMaxValues, "%f", 1.0f);

                }
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
            if (ComponentsManager::get()->getComponentRender()->getStateLUAScripts() == EngineSetup::LuaStateScripts::LUA_STOP) {
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("playIcon")->getOGLTextureID(), ImVec2(24, 24))) {
                    ComponentsManager::get()->getComponentRender()->playLUAScripts();
                }
                ImGui::SameLine();
            } else {
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("stopIcon")->getOGLTextureID(), ImVec2(24, 24))) {
                    ComponentsManager::get()->getComponentRender()->stopLUAScripts();
                }
                ImGui::SameLine();
            }
            if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("reloadIcon")->getOGLTextureID(), ImVec2(24, 24))) {
                ComponentsManager::get()->getComponentRender()->reloadLUAScripts();
            }
            ImGui::SameLine();
            if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("removeIcon")->getOGLTextureID(), ImVec2(24, 24))) {
                ComponentsManager::get()->getComponentRender()->getSceneLoader().clearScene();
            }
        }
        ImGui::End();
    }

    ImGuiConsoleApp *getConsole(){
        return console;
    }

    TexturePackage *getImGuiTextures() {
        return &ImGuiTextures;
    }

    void setSelectedObjectIndex(int selectedObjectIndex) {
        GUIManager::selectedObjectIndex = selectedObjectIndex;
    }
};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
