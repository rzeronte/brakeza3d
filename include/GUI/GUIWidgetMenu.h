//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETMENU_H
#define BRAKEZA3D_GUIWIDGETMENU_H

#include <SDL_mixer.h>
#include <dirent.h>
#include <sys/stat.h>
#include "imgui.h"
#include "../EngineSetup.h"
#include "../Render/Logging.h"
#include "../Misc/SceneLoader.h"
#include "../ComponentsManager.h"
#include "imgui_internal.h"
#include "../Misc/TexturePackage.h"

struct GUIWidgetMenu
{
    TexturePackage &ImGuiTextures;

    explicit GUIWidgetMenu(TexturePackage &imGuiTextures)
    :
        ImGuiTextures(imGuiTextures)
    {
    }

    void draw(bool &finish, bool &show_about_window)
    {
        const float range_sensibility = 0.75f;
        const float range_max_sensibility = 9999;
        const float range_min_sensibility = -9999;

        const float range_max_light_direction_size = 10;
        const float range_min_light_direction_size = 1;

        const float range_sensibility_volume = 1;
        const float range_min_volume = 1;
        const float range_max_volume = 128;

        const float range_far_plane_distance_sensibility = 1.0f;
        const float range_far_plane_min = 0;
        const float range_max_plane_max = 1000000;

        const int range_framerate_sensibility = 1;
        const int range_min_framerate_distance = 0;
        const int range_max_framerate_distance = 500;

        const float range_min_fov = 0;
        const float range_max_fov = 160;

        if (show_about_window) {
            ImGui::OpenPopup("About Brakeza3D");
        }

        if (ImGui::BeginPopupModal("About Brakeza3D", &show_about_window)) {
            ImGui::Text("Welcome to Brakeza3D!");
            ImGui::Text((EngineSetup::get()->ENGINE_VERSION).c_str());
            ImGui::Text("By Eduardo Rodríguez Álvarez <eduardo@brakeza.com>");
            ImGui::Text("Website: https://brakeza.com");
            ImGui::Text("GitHub: https://github.com/rzeronte/brakeza3d");
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
                show_about_window = false;
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Brakeza3D")) {
                if (ImGui::MenuItem("About Brakeza3D")) {
                    show_about_window = true;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) finish = true;
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Control")) {
                auto scripting = ComponentsManager::get()->getComponentScripting();
                auto state = scripting->getStateLUAScripts();
                ImGui::Image(icon("playIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Play scripts", "F1", false,  state != EngineSetup::LuaStateScripts::LUA_PLAY)) {
                    scripting->playLUAScripts();
                }
                ImGui::Separator();
                ImGui::Image(icon("stopIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Stop scripts", "F1", false, state != EngineSetup::LuaStateScripts::LUA_STOP)) {
                    scripting->stopLUAScripts();
                }
                ImGui::Separator();
                ImGui::Image(icon("reloadIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Reload scripts", "F2")) {
                    scripting->reloadLUAScripts();
                }
                ImGui::Separator();
                ImGui::Image(icon("removeIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Clear scene objects", "F3")) {
                    SceneLoader::clearScene();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Add object")) {

                ImGui::Image(icon("objectIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Object3D (Empty)")) {
                    SceneLoader::createObjectInScene();
                }

                ImGui::Separator();

                ImGui::Image(icon("particlesIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("ParticleEmitter")) {
                    SceneLoader::createParticleEmitterInScene();
                    ImGui::EndMenu();
                }
                ImGui::Separator();

                ImGui::Image(icon("Image2DIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::BeginMenu("2D Objects")) {

                    ImGui::Separator();

                    ImGui::Image(icon("Image2DIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::BeginMenu("Image2D")) {
                        drawImage2DItemsToLoad(EngineSetup::get()->IMAGES_FOLDER);
                        ImGui::EndMenu();
                    }
                    ImGui::Image(icon("Image2DAnimationIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::BeginMenu("Image2D Animation")) {
                        drawImage2DAnimationItemsToLoad(EngineSetup::get()->SPRITES_FOLDER);
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                ImGui::Image(icon("meshIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::BeginMenu("3D Objects")) {
                    ImGui::Image(icon("Image3DIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::BeginMenu("Image3D")) {
                        drawImage3DItemsToLoad(EngineSetup::get()->IMAGES_FOLDER);
                        ImGui::EndMenu();
                    }

                    ImGui::Separator();

                    ImGui::Image(icon("BillboardAnimationIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::BeginMenu("Billboard Animation")) {
                        drawBillboardAnimationItemsToLoad(EngineSetup::get()->SPRITES_FOLDER);
                        ImGui::EndMenu();
                    }
                    ImGui::Image(icon("BillboardAnimation8DirectionsIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::MenuItem("Billboard 8-Directions")) {
                        SceneLoader::createBillboardAnimation8Directions();
                        ImGui::EndMenu();
                    }

                    ImGui::Separator();

                    ImGui::Image(icon("meshIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::BeginMenu("Mesh3D")) {
                        drawMesh3DItemsToLoad(EngineSetup::get()->MODELS_FOLDER);
                        ImGui::EndMenu();
                    }

                    ImGui::Separator();

                    ImGui::Image(icon("Mesh3DAnimationIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::BeginMenu("Mesh3DAnimation")) {
                        drawMesh3DAnimationItemsToLoad(EngineSetup::get()->ANIMATIONS_FOLDER);
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                ImGui::Image(icon("lightIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::BeginMenu("Light Objects")) {
                    ImGui::Image(icon("lightIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::MenuItem("PointLight")) {
                        SceneLoader::createPointLight3DInScene();
                        Logging::Message("Add PointLight");
                    }
                    ImGui::Image(icon("spotLightIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::MenuItem("SpotLight")) {
                        SceneLoader::createSpotLight3DInScene();
                        Logging::Message("Add SpotLight");
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Render")) {
                ImGui::Checkbox("v-Sync", &EngineSetup::get()->V_SYNC);
                if (ImGui::IsItemEdited()) {
                    if (EngineSetup::get()->V_SYNC) {
                        Logging::Message("Set V-Sync enabled");
                        SDL_GL_SetSwapInterval(1);
                        SDL_RenderSetVSync(ComponentsManager::get()->getComponentWindow()->getRenderer(), 1);
                    } else {
                        Logging::Message("Set V-Sync disabled");
                        SDL_GL_SetSwapInterval(0);
                        SDL_RenderSetVSync(ComponentsManager::get()->getComponentWindow()->getRenderer(), 0);
                    }
                }
                ImGui::Separator();
                ImGui::DragScalar("FOV", ImGuiDataType_Float, &EngineSetup::get()->HORIZONTAL_FOV, 1, &range_min_fov,&range_max_fov, "%f", 1.0f);
                ImGui::Separator();
                ImGui::Checkbox("Limit frame rate", &EngineSetup::get()->LIMIT_FRAMERATE);
                if (EngineSetup::get()->LIMIT_FRAMERATE) {
                    ImGui::DragScalar("Limite frames to:", ImGuiDataType_S32, &EngineSetup::get()->FRAMERATE, range_framerate_sensibility, &range_min_framerate_distance,&range_max_framerate_distance, "%d", 1.0f);

                }
                ImGui::Separator();
                ImGui::DragScalar("Frustum Far Plane Distance", ImGuiDataType_Float, &EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE, range_far_plane_distance_sensibility, &range_far_plane_min, &range_max_plane_max, "%f", 1.0f);
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
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Physics & Collisions")) {
                int minSubsteps = 0;
                int maxSubsteps = 10;
                int minFixedTime = 1;
                int maxFixedTime = 60;
                ImGui::Checkbox("Enable physics", &EngineSetup::get()->BULLET_STEP_SIMULATION);
                ImGui::Separator();
                ImGui::DragScalar("Max sub-steps", ImGuiDataType_S32, &EngineSetup::get()->BULLET_MAX_SUBSTEPS, 1,&minSubsteps, &maxSubsteps, "%d", 1.0f);
                ImGui::DragScalar("Fixed time step (1/x)", ImGuiDataType_S32, &EngineSetup::get()->BULLET_FIXED_TIME_STEPS, 1, &minFixedTime, &maxFixedTime, "%d", 1.0f);
                ImGui::Separator();
                ImGui::Checkbox("Draw debug mode", &EngineSetup::get()->BULLET_DEBUG_MODE);
                if (EngineSetup::get()->BULLET_STEP_SIMULATION) {
                    ImGui::Separator();
                    ImGui::Checkbox("Handle object collisions", &EngineSetup::get()->BULLET_CHECK_ALL_PAIRS);
                    ImGui::Separator();
                    // gravity
                    if (ImGui::DragScalar("X Gravity", ImGuiDataType_Float, &EngineSetup::get()->gravity.x, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f)) {
                        ComponentsManager::get()->getComponentCollisions()->setGravity(EngineSetup::get()->gravity);
                    }
                    if (ImGui::DragScalar("Y Gravity", ImGuiDataType_Float, &EngineSetup::get()->gravity.y, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f)) {
                        ComponentsManager::get()->getComponentCollisions()->setGravity(EngineSetup::get()->gravity);
                    }
                    if (ImGui::DragScalar("Z Gravity", ImGuiDataType_Float, &EngineSetup::get()->gravity.z, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f)) {
                        ComponentsManager::get()->getComponentCollisions()->setGravity(EngineSetup::get()->gravity);
                    }
                    ImGui::Separator();
                    ImGui::DragScalar("ProjectileDemo Impulse", ImGuiDataType_Float, &EngineSetup::get()->PROJECTILE_DEMO_IMPULSE, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                    ImGui::DragScalar("ProjectileDemo Accuracy", ImGuiDataType_Float, &EngineSetup::get()->PROJECTILE_DEMO_ACCURACY, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                    ImGui::DragScalar("ProjectileDemo Mass", ImGuiDataType_Float, &EngineSetup::get()->PROJECTILE_DEMO_MASS, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                    ImGui::Separator();
                    ImGui::Checkbox("ProjectileDemo SimpleMesh", &EngineSetup::get()->PROJECTILE_SIMPLE_MESH);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Logging")) {
                ImGui::Checkbox("Output to Console", &EngineSetup::get()->LOGGING);
                ImGui::Separator();
                ImGui::Checkbox("Log collisions", &EngineSetup::get()->LOG_COLLISION_OBJECTS);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Layout")) {
                if (ImGui::MenuItem("Default", "F5")) {
                    ComponentsManager::get()->getComponentWindow()->ImGuiConfigChanged = ImGUIConfigs::DEFAULT;
                }
                if (ImGui::MenuItem("Coding", "F6")) {
                    ComponentsManager::get()->getComponentWindow()->ImGuiConfigChanged = ImGUIConfigs::CODING;
                }
                if (ImGui::MenuItem("Design", "F7")) {
                    ComponentsManager::get()->getComponentWindow()->ImGuiConfigChanged = ImGUIConfigs::DESIGN;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Save current layout")) {
                    ComponentsManager::get()->getComponentWindow()->saveImGuiCurrentLayout();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::Checkbox("FullScreen (F11)", &EngineSetup::get()->FULLSCREEN);
                if (ImGui::IsItemEdited()) {
                    ComponentsManager::get()->getComponentWindow()->toggleFullScreen();
                }
                ImGui::Separator();
                ImGui::Checkbox("UI (F4)", &EngineSetup::get()->IMGUI_ENABLED);

                ImGui::Separator();
                ImGui::Checkbox("Draw Main Axis", &EngineSetup::get()->RENDER_MAIN_AXIS);
                ImGui::Checkbox("Draw Object3D Axis", &EngineSetup::get()->RENDER_OBJECTS_AXIS);
                if (EngineSetup::get()->RENDER_OBJECTS_AXIS) {
                    const float sizeAxisMin = 0;
                    const float sizeAxisMax = 1;
                    const float sizeAxisSens = 0.01;

                    ImGui::DragScalar("Size Axis", ImGuiDataType_Float, &EngineSetup::get()->OBJECT_AXIS_SIZE,
                                      sizeAxisSens, &sizeAxisMin, &sizeAxisMax, "%f", 1.0f);

                }
                ImGui::Separator();
                ImGui::Checkbox("Light System", &EngineSetup::get()->ENABLE_LIGHTS);
                if (EngineSetup::get()->ENABLE_LIGHTS) {
                    ImGui::Checkbox("Draw light direction", &EngineSetup::get()->DRAW_LIGHTS_DIRECTION);
                }
                if (EngineSetup::get()->DRAW_LIGHTS_DIRECTION) {
                    ImGui::DragScalar("Size Direction", ImGuiDataType_Float, &EngineSetup::get()->LIGHTS_DIRECTION_SIZE,
                                      0.1f, &range_min_light_direction_size, &range_max_light_direction_size, "%f", 1.0f);
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
                ImGui::Checkbox("Show FPS", &EngineSetup::get()->DRAW_FPS);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void drawImage2DItemsToLoad(const std::string& folder)
    {

        auto files= Tools::getFolderFiles(folder, "png");
        auto folders = Tools::getFolderFolders(folder);

        for (const auto & i : folders) {
            auto fullPath = folder + "/" + i;
            ImGui::Image(icon("folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::BeginMenu(i.c_str())) {
                drawImage2DItemsToLoad(fullPath);
                ImGui::EndMenu();
            }
        }

        for (int i = 0; i < files.size(); i++) {
            auto file = files[i];
            auto title = std::to_string(i-1) + ") " + file;

            auto fullPath = folder + "/" + file;
            ImGui::Image(icon("Image2DIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::MenuItem(file.c_str())) {
                SceneLoader::createImage2DInScene(fullPath);
            }
        }
    }

    void drawImage2DAnimationItemsToLoad(const std::string& folder) {

        auto files= Tools::getFolderFiles(folder, "png");
        auto folders = Tools::getFolderFolders(folder);

        for (const auto & i : folders) {
            auto fullPath = folder + "/" + i;
            ImGui::Image(icon("folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::BeginMenu(i.c_str())) {
                drawImage2DAnimationItemsToLoad(fullPath);
                ImGui::EndMenu();
            }
        }

        for (int i = 0; i < files.size(); i++) {
            auto file = files[i];
            auto title = std::to_string(i-1) + ") " + file;

            auto fullPath = folder + "/" + file;
            ImGui::Image(icon("Image2DAnimationIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::MenuItem(file.c_str())) {
                SceneLoader::createImage2DAnimatedInScene(fullPath);
            }
        }
    }

    void drawImage3DItemsToLoad(const std::string& folder) {
        auto files = Tools::getFolderFiles(folder, "png");
        auto folders = Tools::getFolderFolders(folder);

        for (const auto & i : folders) {
            auto fullPath = folder + "/" + i;
            ImGui::Image(icon("folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::BeginMenu(i.c_str())) {
                drawImage3DItemsToLoad(fullPath);
                ImGui::EndMenu();
            }
        }

        for (int i = 0; i < files.size(); i++) {
            auto file = files[i];
            auto title = std::to_string(i-1) + ") " + file;

            auto fullPath = folder + file;
            ImGui::Image(icon("Image3DIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::MenuItem(file.c_str())) {
                SceneLoader::createImage3DToScene(fullPath);
            }
        }
    }

    ImTextureID icon(const char* iconTag)
    {
        return TexturePackage::getOGLTextureID(ImGuiTextures, iconTag);
    }
    
    void drawBillboardAnimationItemsToLoad(const std::string& folder) {

        auto files = Tools::getFolderFiles(folder, "png");
        auto folders = Tools::getFolderFolders(folder);

        for (const auto & i : folders) {
            auto fullPath = folder + "/" + i;
            ImGui::Image(icon("folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::BeginMenu(i.c_str())) {
                drawBillboardAnimationItemsToLoad(fullPath);
                ImGui::EndMenu();
            }
        }

        for (int i = 0; i < files.size(); i++) {
            auto file = files[i];
            auto title = std::to_string(i-1) + ") " + file;

            auto fullPath = folder + "/" + file;
            ImGui::Image(icon("BillboardAnimationIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::MenuItem(file.c_str())) {
                SceneLoader::createBillboardAnimationInScene(fullPath);
            }
        }
    }

    void drawMesh3DItemsToLoad(const std::string& folder)
    {
        auto files= Tools::getFolderFiles(folder, "fbx");
        auto folders = Tools::getFolderFolders(folder);

        for (const auto & i : folders) {
            auto fullPath = folder + "/" + i;
            ImGui::Image(icon("folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::BeginMenu(i.c_str())) {
                drawMesh3DItemsToLoad(fullPath);
                ImGui::EndMenu();
            }
        }

        for (int i = 0; i < files.size(); i++) {
            auto file = files[i];
            auto title = std::to_string(i-1) + ") " + file;

            auto fullPath = folder + "/" + file;
            ImGui::Image(icon("meshIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::MenuItem(file.c_str())) {
                SceneLoader::createMesh3D(fullPath);
            }
        }
    }

    void drawMesh3DAnimationItemsToLoad(const std::string& folder)
    {
        auto files= Tools::getFolderFiles(folder, "fbx");
        auto folders= Tools::getFolderFolders(folder);

        for (const auto & i : folders) {
            auto fullPath = folder + "/" + i;
            ImGui::Image(icon("folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::BeginMenu(i.c_str())) {
                drawMesh3DAnimationItemsToLoad(fullPath);
                ImGui::EndMenu();
            }
        }

        for (int i = 0; i < files.size(); i++) {
            auto file = files[i];
            auto title = std::to_string(i-1) + ") " + file;

            auto fullPath = folder + "/" + file;
            ImGui::Image(icon("Mesh3DAnimationIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::MenuItem(file.c_str())) {
                SceneLoader::createMesh3DAnimationToScene(fullPath);
            }
        }
    }

};

#endif //BRAKEZA3D_GUIWIDGETMENU_H
