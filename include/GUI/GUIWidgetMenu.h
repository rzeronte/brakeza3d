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

    void draw(bool &finish)
    {
        bool show_about_window = false;

        const float range_sensibility = 0.75f;
        const float range_test_sensibility = 0.1;
        const float range_max_sensibility = 999;
        const float range_min_sensibility = -999;

        const float range_sensibility_volume = 1;
        const float range_min_volume = 1;
        const float range_max_volume = 128;

        const float range_far_plane_distance_sensibility = 1.0f;
        const float range_far_plane_min = 0;
        const float range_max_plane_max = 1000000;

        const int range_framerate_sensibility = 1;
        const int range_min_framerate_distance = 0;
        const int range_max_framerate_distance = 500;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Brakeza3D")) {
                if (ImGui::MenuItem("About Brakeza3D", "CTRL+I")) show_about_window = true;
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "CTRL+W")) finish = true;
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Add object")) {

                ImGui::Image(icon("objectIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Object3D (Empty)", "CTRL+O")) {
                    SceneLoader::createObjectInScene();
                }

                ImGui::Separator();

                ImGui::Image(icon("particlesIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("ParticleEmitter", "CTRL+x")) {
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
                    if (ImGui::MenuItem("Billboard 8-Directions", "CTRL+L")) {
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
                    if (ImGui::MenuItem("PointLight", "CTRL+O")) {
                        SceneLoader::createPointLight3DInScene();
                        Logging::Message("Add PointLight");
                    }
                    ImGui::Image(icon("spotLightIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::MenuItem("SpotLight", "CTRL+O")) {
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
                ImGui::Separator();
                if (ImGui::Checkbox("Free Look", &EngineSetup::get()->FREE_LOOK)) {
                    ComponentsManager::get()->getComponentCamera()->setFreeLook(EngineSetup::get()->FREE_LOOK);
                    ComponentsManager::get()->getComponentInput()->setEnabled(EngineSetup::get()->FREE_LOOK);
                }

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
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Layout")) {
                if (ImGui::MenuItem("Default", "CTRL+D")) {
                    ComponentsManager::get()->getComponentWindow()->ImGuiConfigChanged = ImGUIConfigs::DEFAULT;
                }
                if (ImGui::MenuItem("Coding", "CTRL+C")) {
                    ComponentsManager::get()->getComponentWindow()->ImGuiConfigChanged = ImGUIConfigs::CODING;
                }
                if (ImGui::MenuItem("Design", "CTRL+D")) {
                    ComponentsManager::get()->getComponentWindow()->ImGuiConfigChanged = ImGUIConfigs::DESIGN;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Save current layout", "CTRL+D")) {
                    ComponentsManager::get()->getComponentWindow()->saveImGuiCurrentLayout();
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
                ImGui::Checkbox("Show FPS", &EngineSetup::get()->DRAW_FPS);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void drawImage2DItemsToLoad(const std::string& folder) {

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
        auto folders = Tools::getFolderFolders(folder);

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
